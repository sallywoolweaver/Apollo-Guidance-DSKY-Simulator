#include "AgcCpu.h"

#include <algorithm>

#include "AgcMemoryImage.h"

namespace apollo {

namespace {
constexpr double kAgcInstructionsPerSecond = 85000.0;
constexpr uint16_t kWordMask = 077777;
constexpr uint16_t kAddressMask12 = 07777;
constexpr uint16_t kAddressMask10 = 01777;
constexpr uint16_t kRegA = 00;
constexpr uint16_t kRegL = 01;
constexpr uint16_t kRegQ = 02;
constexpr uint16_t kRegEB = 03;
constexpr uint16_t kRegFB = 04;
constexpr uint16_t kRegZ = 05;
constexpr uint16_t kRegBB = 06;
constexpr uint16_t kRegZERO = 07;
constexpr uint16_t kRegBRUPT = 017;
constexpr uint16_t kReg16Limit = 03;
constexpr uint16_t kResumeInstructionWord = 050017;

uint16_t maskWord(uint32_t value) {
    return static_cast<uint16_t>(value & kWordMask);
}

uint16_t addOnesComplement(uint16_t lhs, uint16_t rhs) {
    uint32_t sum = static_cast<uint32_t>(lhs & kWordMask) + static_cast<uint32_t>(rhs & kWordMask);
    while (sum > kWordMask) {
        sum = (sum & kWordMask) + 1U;
    }
    return static_cast<uint16_t>(sum & kWordMask);
}

uint16_t negateOnesComplement(uint16_t value) {
    return static_cast<uint16_t>(~value) & kWordMask;
}
}

void AgcCpu::initialize() {
    state_ = AgcCpuState();
    ioChannels_.fill(0);
    dskyRelayWords_.fill(0);
    ioChannels_[032] = 020000;
    state_.accumulator = 0;
    state_.lRegister = 0;
    state_.qRegister = 0;
    state_.ebRegister = 0;
    state_.fbRegister = 0;
    state_.bbRegister = 0;
    state_.indexRegister = 0;
    state_.interruptsEnabled = true;
    state_.outputChannel7 = 0;
    state_.extendFlag = false;
    state_.initialized = true;
}

void AgcCpu::loadProgramContext(const AgcProgramImage& image, const std::string& requestedProgram) {
    if (!state_.initialized) {
        initialize();
    }
    state_.runState = CpuRunState::IDLE;
    state_.currentMajorMode = std::stoi(requestedProgram);
    state_.programCounterBank = image.firstRopeBank;
    state_.programCounterOffset = image.firstRopeOffset;
    state_.executionNote = "Program image loaded";
}

void AgcCpu::resetForScenario(const AgcProgramImage& image, const std::string& requestedProgram) {
    if (!state_.initialized) {
        initialize();
    }
    ioChannels_.fill(0);
    dskyRelayWords_.fill(0);
    ioChannels_[032] = 020000;
    state_.runState = CpuRunState::EXECUTING;
    state_.cycleCounter = 0;
    state_.executedInstructions = 0;
    state_.restartCounter = 0;
    state_.currentMajorMode = std::stoi(requestedProgram);
    state_.accumulator = 0;
    state_.lRegister = 0;
    state_.qRegister = 0;
    state_.ebRegister = 0;
    state_.fbRegister = 0;
    state_.bbRegister = 0;
    state_.indexRegister = 0;
    state_.interruptsEnabled = true;
    state_.outputChannel7 = 0;
    state_.extendFlag = false;
    state_.programCounterBank = image.firstRopeBank;
    state_.programCounterOffset = image.firstRopeOffset;
    state_.lastFetchedWord = 0;
    state_.lastOpcodeClass = 0;
    state_.unsupportedOpcodeCount = 0;
    state_.supportedOpcodeSkeletonCount = 0;
    state_.currentLabel.clear();
    state_.executedLabels.clear();
    state_.executionNote = "Instruction-step skeleton active";
}

void AgcCpu::requestMajorMode(int majorMode) {
    if (!state_.initialized) {
        initialize();
    }
    state_.currentMajorMode = majorMode;
}

void AgcCpu::jumpToBankOffset(uint16_t bank, uint16_t offset) {
    if (!state_.initialized) {
        initialize();
    }
    state_.programCounterBank = bank & 077;
    state_.programCounterOffset = offset & 01777;
    state_.runState = CpuRunState::EXECUTING;
}

void AgcCpu::setSwitchedFixedBank(uint16_t bank) {
    if (!state_.initialized) {
        initialize();
    }
    state_.fbRegister = static_cast<uint16_t>((bank & 037) << 10);
    syncBankRegisters();
}

void AgcCpu::setAccumulator(uint16_t word) {
    if (!state_.initialized) {
        initialize();
    }
    state_.accumulator = maskWord(word);
}

void AgcCpu::setLRegister(uint16_t word) {
    if (!state_.initialized) {
        initialize();
    }
    state_.lRegister = maskWord(word);
}

void AgcCpu::setInputChannel(uint16_t channel, uint16_t word) {
    writeChannel(channel, word);
}

void AgcCpu::setInputChannelBits(uint16_t channel, uint16_t mask, bool asserted) {
    const uint16_t normalizedChannel = static_cast<uint16_t>(channel & 0177);
    const uint16_t normalizedMask = static_cast<uint16_t>(mask & kWordMask);
    uint16_t value = readChannel(normalizedChannel);
    if (asserted) {
        value = static_cast<uint16_t>(value | normalizedMask);
    } else {
        value = static_cast<uint16_t>(value & ~normalizedMask);
    }
    writeChannel(normalizedChannel, value);
}

uint16_t AgcCpu::channelValue(uint16_t channel) const {
    return readChannel(channel);
}

uint16_t AgcCpu::dskyRelayWord(uint16_t row) const {
    return dskyRelayWords_[row & 017];
}

uint16_t AgcCpu::switchedFixedBank() const {
    return static_cast<uint16_t>((state_.fbRegister >> 10) & 037);
}

void AgcCpu::step(double deltaSeconds, AgcMemoryImage& memoryImage) {
    if (!state_.initialized) {
        initialize();
    }
    if (state_.runState == CpuRunState::HALTED) {
        return;
    }
    state_.runState = CpuRunState::EXECUTING;
    const auto requestedInstructions = std::max<uint64_t>(1, static_cast<uint64_t>(deltaSeconds * kAgcInstructionsPerSecond));
    const auto instructionsToStep = std::min<uint64_t>(requestedInstructions, 4096);
    if (!memoryImage.metadata().containsApolloDerivedWords) {
        state_.cycleCounter += instructionsToStep;
        state_.executionNote = "No Apollo-derived rope words loaded";
        return;
    }

    for (uint64_t index = 0; index < instructionsToStep; ++index) {
        stepInstruction(memoryImage);
    }
    state_.cycleCounter += instructionsToStep;
}

void AgcCpu::stepInstruction(AgcMemoryImage& memoryImage) {
    if (!state_.initialized) {
        initialize();
    }
    if (!memoryImage.metadata().containsApolloDerivedWords) {
        state_.executionNote = "No Apollo-derived rope words loaded";
        return;
    }

    branchTaken_ = false;
    const uint16_t ropeWord = memoryImage.ropeWord(state_.programCounterBank, state_.programCounterOffset);
    state_.currentLabel = memoryImage.ropeLabel(state_.programCounterBank, state_.programCounterOffset);
    const uint16_t effectiveWord = applyIndexToInstruction(ropeWord);
    state_.lastFetchedWord = effectiveWord;
    executeFetchedWord(effectiveWord, memoryImage);
    if (!state_.currentLabel.empty()) {
        state_.executedLabels.push_back(state_.currentLabel);
        if (state_.executedLabels.size() > 4) {
            state_.executedLabels.erase(state_.executedLabels.begin());
        }
    }
    if (!branchTaken_) {
        advanceProgramCounter();
    }
    state_.executedInstructions += 1;
    state_.cycleCounter += 1;
}

uint16_t AgcCpu::applyIndexToInstruction(uint16_t word) {
    if (state_.indexRegister == 0) {
        return word;
    }
    const uint16_t indexed = maskWord(static_cast<uint32_t>(word) + static_cast<uint32_t>(state_.indexRegister));
    state_.indexRegister = 0;
    return indexed;
}

uint16_t AgcCpu::readOperand12(uint16_t address12, const AgcMemoryImage& memoryImage) const {
    const uint16_t address = address12 & kAddressMask12;
    if (address <= kRegBB) {
        switch (address) {
            case kRegA:
                return state_.accumulator;
            case kRegL:
                return state_.lRegister;
            case kRegQ:
                return state_.qRegister;
            case kRegEB:
                return state_.ebRegister;
            case kRegFB:
                return state_.fbRegister;
            case kRegZ:
                return static_cast<uint16_t>(state_.programCounterOffset & kAddressMask12);
            case kRegBB:
                return state_.bbRegister;
            default:
                break;
        }
    }
    if (address == kRegZERO) {
        return 0;
    }
    if (address < 00400) {
        return memoryImage.erasableBankWord(0, address);
    }
    if (address < 01000) {
        return memoryImage.erasableBankWord(1, address);
    }
    if (address < 01400) {
        return memoryImage.erasableBankWord(2, address);
    }
    if (address < 02000) {
        return memoryImage.erasableBankWord((state_.ebRegister >> 8) & 07, address);
    }
    return readFixedWord(address, memoryImage);
}

uint16_t AgcCpu::readOperand10(uint16_t address10, const AgcMemoryImage& memoryImage) const {
    return readOperand12(address10 & kAddressMask10, memoryImage);
}

void AgcCpu::writeOperand10(uint16_t address10, uint16_t word, AgcMemoryImage& memoryImage) {
    const uint16_t address = address10 & kAddressMask10;
    const uint16_t normalized = maskWord(word);
    switch (address) {
        case kRegA:
            state_.accumulator = normalized;
            return;
        case kRegL:
            state_.lRegister = normalized;
            return;
        case kRegQ:
            state_.qRegister = normalized;
            return;
        case kRegEB:
            state_.ebRegister = normalized & 03400;
            syncBankRegisters();
            return;
        case kRegFB:
            state_.fbRegister = normalized & 076000;
            syncBankRegisters();
            return;
        case kRegZ:
            setProgramCounterFromAddress(normalized);
            return;
        case kRegBB:
            state_.bbRegister = normalized & 076007;
            syncBankRegisters();
            return;
        case kRegZERO:
            return;
        default:
            break;
    }

    if (address < 00400) {
        memoryImage.writeErasableBankWord(0, address, normalized);
    } else if (address < 01000) {
        memoryImage.writeErasableBankWord(1, address, normalized);
    } else if (address < 01400) {
        memoryImage.writeErasableBankWord(2, address, normalized);
    } else if (address < 02000) {
        memoryImage.writeErasableBankWord((state_.ebRegister >> 8) & 07, address, normalized);
    }
}

uint16_t AgcCpu::readChannel(uint16_t channel) const {
    const uint16_t normalizedChannel = static_cast<uint16_t>(channel & 0177);
    if (normalizedChannel == kRegL) {
        return state_.lRegister;
    }
    if (normalizedChannel == kRegQ) {
        return state_.qRegister;
    }
    if (normalizedChannel == 07) {
        return state_.outputChannel7;
    }
    return ioChannels_[normalizedChannel];
}

void AgcCpu::writeChannel(uint16_t channel, uint16_t word) {
    const uint16_t normalizedChannel = static_cast<uint16_t>(channel & 0177);
    const uint16_t normalizedWord = maskWord(word);
    if (normalizedChannel == kRegL) {
        state_.lRegister = normalizedWord;
        return;
    }
    if (normalizedChannel == kRegQ) {
        state_.qRegister = normalizedWord;
        return;
    }
    ioChannels_[normalizedChannel] = normalizedWord;
    if (normalizedChannel == 07) {
        state_.outputChannel7 = normalizedWord;
    } else if (normalizedChannel == 010) {
        const uint16_t row = static_cast<uint16_t>((normalizedWord >> 11) & 017);
        if (row != 0) {
            dskyRelayWords_[row] = normalizedWord;
        }
    }
}

void AgcCpu::setProgramCounterFromAddress(uint16_t address12) {
    const uint16_t address = address12 & kAddressMask12;
    if (address < 02000) {
        state_.programCounterOffset = address;
        return;
    }
    if (address < 04000) {
        state_.programCounterBank = fixedBankForAddress(address);
        state_.programCounterOffset = address & 01777;
        return;
    }
    if (address < 06000) {
        state_.programCounterBank = 02;
        state_.programCounterOffset = address & 01777;
        return;
    }
    state_.programCounterBank = 03;
    state_.programCounterOffset = address & 01777;
}

void AgcCpu::syncBankRegisters() {
    if ((state_.bbRegister & 076007) != ((state_.fbRegister & 076000) | ((state_.ebRegister & 03400) >> 8))) {
        if (state_.bbRegister != 0) {
            state_.fbRegister = state_.bbRegister & 076000;
            state_.ebRegister = (state_.bbRegister & 07) << 8;
        } else {
            state_.bbRegister = (state_.fbRegister & 076000) | ((state_.ebRegister & 03400) >> 8);
        }
    }
    state_.bbRegister &= 076007;
    state_.ebRegister &= 03400;
    state_.fbRegister &= 076000;
}

uint16_t AgcCpu::fixedBankForAddress(uint16_t address12) const {
    const uint16_t address = address12 & kAddressMask12;
    if (address < 04000) {
        uint16_t bank = static_cast<uint16_t>((state_.fbRegister >> 10) & 037);
        if ((bank & 030) == 030 && (state_.outputChannel7 & 0100) != 0) {
            bank = static_cast<uint16_t>(bank + 010);
        }
        return bank;
    }
    if (address < 06000) {
        return 02;
    }
    return 03;
}

uint16_t AgcCpu::readFixedWord(uint16_t address12, const AgcMemoryImage& memoryImage) const {
    const uint16_t bank = fixedBankForAddress(address12);
    const uint16_t offset = (address12 < 04000) ? (address12 & 01777) : (address12 & 01777);
    return memoryImage.ropeWord(bank, offset);
}

uint16_t AgcCpu::readOperandPairHigh(uint16_t address10, const AgcMemoryImage& memoryImage) const {
    return readOperand10(address10, memoryImage);
}

uint16_t AgcCpu::readOperandPairLow(uint16_t address10, const AgcMemoryImage& memoryImage) const {
    return readOperand10(static_cast<uint16_t>((address10 + 1) & kAddressMask10), memoryImage);
}

void AgcCpu::writeOperandPair(uint16_t address10, uint16_t highWord, uint16_t lowWord, AgcMemoryImage& memoryImage) {
    writeOperand10(address10, highWord, memoryImage);
    writeOperand10(static_cast<uint16_t>((address10 + 1) & kAddressMask10), lowWord, memoryImage);
}

void AgcCpu::advanceProgramCounter() {
    state_.programCounterOffset += 1;
    if (state_.programCounterOffset >= 02000) {
        state_.programCounterOffset = 0;
        state_.programCounterBank += 1;
    }
}

void AgcCpu::executeFetchedWord(uint16_t word, AgcMemoryImage& memoryImage) {
    if (word == 0) {
        state_.executionNote = state_.currentLabel.empty() ? "Fetched empty rope word" : ("Fetched " + state_.currentLabel);
        return;
    }
    if (word == kResumeInstructionWord) {
        setProgramCounterFromAddress(memoryImage.erasableWord(kRegBRUPT));
        branchTaken_ = true;
        state_.supportedOpcodeSkeletonCount++;
        state_.executionNote = "RESUME";
        return;
    }

    uint16_t extendedOpcode = static_cast<uint16_t>((word >> 9) & 077);
    const uint16_t address12 = word & kAddressMask12;
    const uint16_t address10 = word & kAddressMask10;
    const uint16_t address9 = word & 0777;
    if (state_.extendFlag) {
        extendedOpcode = static_cast<uint16_t>(extendedOpcode | 0100);
        state_.extendFlag = false;
    }
    state_.lastOpcodeClass = extendedOpcode;

    switch (extendedOpcode) {
        case 000:
        case 001:
        case 002:
        case 003:
        case 004:
        case 005:
        case 006:
        case 007:
            if (address12 == 3) {
                state_.interruptsEnabled = true;
                state_.supportedOpcodeSkeletonCount++;
                state_.executionNote = "RELINT";
            } else if (address12 == 4) {
                state_.interruptsEnabled = false;
                state_.supportedOpcodeSkeletonCount++;
                state_.executionNote = "INHINT";
            } else if (address12 == 6) {
                state_.extendFlag = true;
                state_.supportedOpcodeSkeletonCount++;
                state_.executionNote = "EXTEND";
            } else if (address12 == kRegQ) {
                setProgramCounterFromAddress(state_.qRegister);
                branchTaken_ = true;
                state_.supportedOpcodeSkeletonCount++;
                state_.executionNote = "TC Q";
            } else {
                state_.qRegister = static_cast<uint16_t>((state_.programCounterOffset + 1) & kAddressMask12);
                setProgramCounterFromAddress(address12);
                branchTaken_ = true;
                state_.supportedOpcodeSkeletonCount++;
                state_.executionNote = "TC " + (state_.currentLabel.empty() ? std::to_string(address12) : state_.currentLabel);
            }
            break;
        case 010:
        case 011: {
            const uint16_t value = readOperand10(address10, memoryImage);
            if (value == 0) {
                state_.accumulator = 0;
                state_.programCounterOffset += 1;
            } else if (value == kWordMask) {
                state_.accumulator = 0;
                state_.programCounterOffset += 3;
            } else if ((value & 040000) != 0) {
                state_.accumulator = maskWord(~value) - 1;
                state_.programCounterOffset += 2;
            } else {
                state_.accumulator = maskWord(value - 1);
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "CCS " + std::to_string(address10);
            break;
        }
        case 012:
        case 013:
        case 014:
        case 015:
        case 016:
        case 017:
            setProgramCounterFromAddress(address12);
            branchTaken_ = true;
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "TCF " + (state_.currentLabel.empty() ? std::to_string(address12) : state_.currentLabel);
            break;
        case 020:
        case 021: {
            const uint16_t high = maskWord(static_cast<uint32_t>(state_.accumulator) + readOperandPairHigh(address10, memoryImage));
            const uint16_t low = maskWord(static_cast<uint32_t>(state_.lRegister) + readOperandPairLow(address10, memoryImage));
            writeOperandPair(address10, high, low, memoryImage);
            state_.accumulator = high;
            state_.lRegister = low;
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "DAS " + std::to_string(address10);
            break;
        }
        case 022:
        case 023: {
            if (address10 != kRegL) {
                const uint16_t operand = readOperand10(address10, memoryImage);
                writeOperand10(address10, state_.lRegister, memoryImage);
                state_.lRegister = operand;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "LXCH " + std::to_string(address10);
            break;
        }
        case 024:
        case 025: {
            const uint16_t result = maskWord(static_cast<uint32_t>(readOperand10(address10, memoryImage)) + 1U);
            writeOperand10(address10, result, memoryImage);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "INCR " + std::to_string(address10);
            break;
        }
        case 026:
        case 027: {
            const uint16_t result = addOnesComplement(state_.accumulator, readOperand10(address10, memoryImage));
            state_.accumulator = result;
            if (address10 != kRegA) {
                writeOperand10(address10, result, memoryImage);
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "ADS " + std::to_string(address10);
            break;
        }
        case 030:
        case 031:
        case 032:
        case 033:
        case 034:
        case 035:
        case 036:
        case 037:
            if (address12 != kRegA) {
                state_.accumulator = readOperand12(address12, memoryImage);
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "CA " + std::to_string(address12);
            break;
        case 040:
        case 041:
        case 042:
        case 043:
        case 044:
        case 045:
        case 046:
        case 047:
            state_.accumulator = maskWord(~readOperand12(address12, memoryImage));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "CS " + std::to_string(address12);
            break;
        case 050:
        case 051:
            state_.indexRegister = readOperand10(address10, memoryImage);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "INDEX " + std::to_string(address10);
            break;
        case 052:
        case 053: {
            const uint16_t highOperand = readOperandPairHigh(address10, memoryImage);
            const uint16_t lowOperand = readOperandPairLow(address10, memoryImage);
            writeOperandPair(address10, state_.accumulator, state_.lRegister, memoryImage);
            state_.accumulator = highOperand;
            state_.lRegister = lowOperand;
            if (address10 == kRegZ || address10 == kRegFB) {
                branchTaken_ = true;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "DXCH " + std::to_string(address10);
            break;
        }
        case 054:
        case 055:
            if (address10 == kRegZ) {
                setProgramCounterFromAddress(state_.accumulator);
                branchTaken_ = true;
            } else if (address10 != kRegA) {
                writeOperand10(address10, state_.accumulator, memoryImage);
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "TS " + std::to_string(address10);
            break;
        case 056:
        case 057: {
            if (address10 != kRegA) {
                const uint16_t operand = readOperand10(address10, memoryImage);
                writeOperand10(address10, state_.accumulator, memoryImage);
                state_.accumulator = operand;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "XCH " + std::to_string(address10);
            break;
        }
        case 060:
        case 061:
        case 062:
        case 063:
        case 064:
        case 065:
        case 066:
        case 067:
            state_.accumulator = addOnesComplement(state_.accumulator, readOperand12(address12, memoryImage));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "AD " + std::to_string(address12);
            break;
        case 070:
        case 071:
        case 072:
        case 073:
        case 074:
        case 075:
        case 076:
        case 077:
            state_.accumulator = static_cast<uint16_t>(state_.accumulator & readOperand12(address12, memoryImage));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "MASK " + std::to_string(address12);
            break;
        case 0100:
            state_.accumulator = readChannel(address9);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "READ " + std::to_string(address9);
            break;
        case 0101:
            writeChannel(address9, state_.accumulator);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "WRITE " + std::to_string(address9);
            break;
        case 0102:
            state_.accumulator = static_cast<uint16_t>(state_.accumulator & readChannel(address9));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "RAND " + std::to_string(address9);
            break;
        case 0103: {
            const uint16_t result = static_cast<uint16_t>(state_.accumulator & readChannel(address9));
            writeChannel(address9, result);
            state_.accumulator = result;
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "WAND " + std::to_string(address9);
            break;
        }
        case 0104:
            state_.accumulator = static_cast<uint16_t>(state_.accumulator | readChannel(address9));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "ROR " + std::to_string(address9);
            break;
        case 0105: {
            const uint16_t result = static_cast<uint16_t>(state_.accumulator | readChannel(address9));
            writeChannel(address9, result);
            state_.accumulator = result;
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "WOR " + std::to_string(address9);
            break;
        }
        case 0106:
            state_.accumulator = static_cast<uint16_t>(state_.accumulator ^ readChannel(address9));
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "RXOR " + std::to_string(address9);
            break;
        case 0107:
            state_.unsupportedOpcodeCount++;
            state_.executionNote = "EDRUPT";
            break;
        case 0112:
        case 0113:
        case 0114:
        case 0115:
        case 0116:
        case 0117:
            if (state_.accumulator == 0 || state_.accumulator == kWordMask) {
                setProgramCounterFromAddress(address12);
                branchTaken_ = true;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "BZF " + std::to_string(address12);
            break;
        case 0122:
        case 0123: {
            if (address10 != kRegQ) {
                const uint16_t operand = readOperand10(address10, memoryImage);
                writeOperand10(address10, state_.qRegister, memoryImage);
                state_.qRegister = operand;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "QXCH " + std::to_string(address10);
            break;
        }
        case 0130:
        case 0131:
        case 0132:
        case 0133:
        case 0134:
        case 0135:
        case 0136:
        case 0137: {
            state_.accumulator = readOperandPairHigh(address10, memoryImage);
            state_.lRegister = readOperandPairLow(address10, memoryImage);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "DCA " + std::to_string(address10);
            break;
        }
        case 0150:
        case 0151:
        case 0152:
        case 0153:
        case 0154:
        case 0155:
        case 0156:
        case 0157:
            state_.indexRegister = readOperand12(address12, memoryImage);
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "INDEX* " + std::to_string(address12);
            break;
        case 0160:
        case 0161:
            state_.accumulator = addOnesComplement(
                state_.accumulator,
                negateOnesComplement(readOperand10(address10, memoryImage))
            );
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "SU " + std::to_string(address10);
            break;
        case 0162:
        case 0163:
        case 0164:
        case 0165:
        case 0166:
        case 0167:
            if (state_.accumulator == 0 || (state_.accumulator & 040000) != 0) {
                setProgramCounterFromAddress(address12);
                branchTaken_ = true;
            }
            state_.supportedOpcodeSkeletonCount++;
            state_.executionNote = "BZMF " + std::to_string(address12);
            break;
        default:
            state_.unsupportedOpcodeCount++;
            state_.executionNote = "UNSUPPORTED " + std::to_string(extendedOpcode);
            break;
    }
}

}  // namespace apollo
