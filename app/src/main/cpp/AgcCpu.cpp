#include "AgcCpu.h"

#include <algorithm>

#include "AgcMemoryImage.h"

namespace apollo {

namespace {
constexpr double kAgcInstructionsPerSecond = 85000.0;
}

void AgcCpu::initialize() {
    state_ = AgcCpuState();
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
    state_.runState = CpuRunState::EXECUTING;
    state_.cycleCounter = 0;
    state_.executedInstructions = 0;
    state_.restartCounter = 0;
    state_.currentMajorMode = std::stoi(requestedProgram);
    state_.accumulator = 0;
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
        branchTaken_ = false;
        const uint16_t word = memoryImage.ropeWord(state_.programCounterBank, state_.programCounterOffset);
        state_.currentLabel = memoryImage.ropeLabel(state_.programCounterBank, state_.programCounterOffset);
        state_.lastFetchedWord = word;
        executeFetchedWord(word, memoryImage);
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
    }
    state_.cycleCounter += instructionsToStep;
}

void AgcCpu::advanceProgramCounter() {
    state_.programCounterOffset += 1;
    if (state_.programCounterOffset >= 010000) {
        state_.programCounterOffset = 0;
        state_.programCounterBank += 1;
    }
}

void AgcCpu::executeFetchedWord(uint16_t word, AgcMemoryImage& memoryImage) {
    if (word == 0) {
        state_.executionNote = state_.currentLabel.empty() ? "Fetched empty rope word" : ("Fetched " + state_.currentLabel);
        return;
    }

    const uint16_t opcodeClass = static_cast<uint16_t>((word >> 12) & 0xF);
    state_.lastOpcodeClass = opcodeClass;
    switch (opcodeClass) {
        case 0:
            state_.supportedOpcodeSkeletonCount += 1;
            state_.executionNote = state_.currentLabel.empty() ? "Executed class-0 control/basic opcode" : ("Executed " + state_.currentLabel);
            break;
        case 2:
            state_.supportedOpcodeSkeletonCount += 1;
            state_.accumulator = static_cast<uint16_t>((state_.accumulator + (word & 07777)) & 077777);
            state_.executionNote = state_.currentLabel.empty() ? "Executed class-2 arithmetic opcode" : ("Executed " + state_.currentLabel);
            break;
        case 4:
            state_.supportedOpcodeSkeletonCount += 1;
            if ((word & 07777) != 0) {
                state_.programCounterOffset = static_cast<int>(word & 07777);
                branchTaken_ = true;
            }
            state_.executionNote = state_.currentLabel.empty() ? "Executed class-4 transfer/branch opcode" : ("Executed " + state_.currentLabel);
            break;
        case 5:
            state_.supportedOpcodeSkeletonCount += 1;
            memoryImage.writeErasableWord(0100, state_.accumulator);
            state_.executionNote = state_.currentLabel.empty() ? "Executed class-5 TS-like opcode" : ("Executed " + state_.currentLabel);
            break;
        default:
            state_.unsupportedOpcodeCount += 1;
            state_.executionNote = state_.currentLabel.empty() ? "Fetched unsupported opcode class" : ("Unsupported opcode at " + state_.currentLabel);
            break;
    }
}

}  // namespace apollo
