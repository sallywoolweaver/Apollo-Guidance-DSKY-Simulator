#include "NativeApolloCore.h"

#include "AgcCpu.h"
#include "AgcMemoryImage.h"
#include "AlarmExecutive.h"
#include "CompatibilityScenario.h"
#include "DskyIo.h"
#include "ScenarioBootstrap.h"
#include "SnapshotBuilder.h"

#include <algorithm>
#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <string>

#include <android/log.h>

namespace apollo {

namespace {
constexpr uint16_t kMpacAddress = 0154;
constexpr uint16_t kNewLocAddress = 065;
constexpr uint16_t kAruptAddress = 010;
constexpr uint16_t kLruptAddress = 011;
constexpr uint16_t kBruptAddress = 017;
constexpr int kPostDispatchTargetBudget = 128;
constexpr uint16_t kSupdxchzOffset = 03165 & 01777;
constexpr uint16_t kSupdxchzPlusOneOffset = 03166 & 01777;
constexpr uint16_t kNewJobAddress = 067;
constexpr uint16_t kLocAddress = 0164;
constexpr uint16_t kBankSetAddress = 0165;
constexpr uint16_t kPriorityAddress = 0167;
constexpr char kExecTraceTag[] = "ApolloExecTrace";

void logExecutiveTrace(const char* message) {
    __android_log_write(ANDROID_LOG_INFO, kExecTraceTag, message);
}

void logExecutiveTracef(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    logExecutiveTrace(buffer);
}

bool isSchedulerDispatchBoundaryLabel(const std::string& label) {
    return label == "ADVAN" || label == "NUDIRECT" || label == "CHANJOB";
}

bool isExecutiveCompletionBoundaryLabel(const std::string& label) {
    return label == "ENDPRCHG" || label == "INTRSM" || label == "TASKOVER";
}

bool isFinalPreTransferTransitionLabel(const std::string& label) {
    return label == "JOBSLP1" || label == "JOBSLP2" || label == "NUCHANG2" ||
        label == "DUMMYJOB" || label == "ADVAN" || label == "NUDIRECT";
}

bool isNaturalSupdxchzTransferState(const AgcCpu& cpu) {
    const auto& state = cpu.state();
    return state.programCounterBank == 02 &&
        (state.programCounterOffset == kSupdxchzOffset || state.programCounterOffset == kSupdxchzPlusOneOffset);
}

bool isExecutiveTraceLabel(const std::string& label) {
    return label == "KEYRUPT1" || label == "KEYCOM" || label == "ACCEPTUP" || label == "LODSAMPT" ||
        label == "NOVAC" || label == "NOVAC2" || label == "NOVAC3" || label == "CORFOUND" ||
        label == "RESUME" || label == "NOQRSM" || label == "NOQBRSM" ||
        label == "SPECTEST" || label == "SETLOC" || label == "CHANJOB" ||
        label == "ENDPRCHG" || label == "JOBSLP1" || label == "JOBSLP2" || label == "NUCHANG2" ||
        label == "DUMMYJOB" || label == "ADVAN" || label == "SELFBANK" || label == "NUDIRECT" ||
        label == "SUPDXCHZ" || label == "TASKOVER" || label == "INTRSM";
}

struct ProgramPackageSections {
    std::vector<uint8_t> ropeBytes;
    std::vector<uint8_t> ropeMetadataBytes;
    std::vector<uint8_t> ropeLabelBytes;
    std::vector<uint8_t> erasableBytes;
    std::vector<uint8_t> bootstrapBytes;
    bool ropeIsBinary = false;
    bool erasableIsBinary = false;
};

bool consumeLengthDelimitedSection(
    const std::vector<uint8_t>& bytes,
    size_t& cursor,
    std::string& sectionName,
    std::vector<uint8_t>& sectionBytes
) {
    size_t headerEnd = cursor;
    while (headerEnd < bytes.size() && bytes[headerEnd] != '\n') {
        headerEnd += 1;
    }
    if (headerEnd >= bytes.size()) {
        return false;
    }

    const std::string header(bytes.begin() + static_cast<std::ptrdiff_t>(cursor), bytes.begin() + static_cast<std::ptrdiff_t>(headerEnd));
    const auto separator = header.find(' ');
    if (separator == std::string::npos) {
        return false;
    }

    sectionName = header.substr(0, separator);
    const auto length = static_cast<size_t>(std::stoull(header.substr(separator + 1)));
    const size_t payloadStart = headerEnd + 1;
    const size_t payloadEnd = payloadStart + length;
    if (payloadEnd > bytes.size()) {
        return false;
    }

    sectionBytes.assign(bytes.begin() + static_cast<std::ptrdiff_t>(payloadStart), bytes.begin() + static_cast<std::ptrdiff_t>(payloadEnd));
    cursor = payloadEnd;
    return true;
}

ProgramPackageSections splitProgramPackage(const std::vector<uint8_t>& bytes) {
    const std::string packageHeader = "APOLLOPKG\n";
    if (bytes.size() >= packageHeader.size() &&
        std::equal(packageHeader.begin(), packageHeader.end(), bytes.begin())) {
        ProgramPackageSections sections;
        size_t cursor = packageHeader.size();
        while (cursor < bytes.size()) {
            std::string sectionName;
            std::vector<uint8_t> sectionBytes;
            if (!consumeLengthDelimitedSection(bytes, cursor, sectionName, sectionBytes)) {
                break;
            }

            if (sectionName == "ROPE_BIN") {
                sections.ropeBytes = std::move(sectionBytes);
                sections.ropeIsBinary = true;
            } else if (sectionName == "ROPE_TEXT") {
                sections.ropeBytes = std::move(sectionBytes);
            } else if (sectionName == "ROPE_METADATA_TEXT") {
                sections.ropeMetadataBytes = std::move(sectionBytes);
            } else if (sectionName == "ROPE_LABELS_TEXT") {
                sections.ropeLabelBytes = std::move(sectionBytes);
            } else if (sectionName == "ERASABLE_BIN") {
                sections.erasableBytes = std::move(sectionBytes);
                sections.erasableIsBinary = true;
            } else if (sectionName == "ERASABLE_TEXT") {
                sections.erasableBytes = std::move(sectionBytes);
            } else if (sectionName == "BOOTSTRAP_TEXT") {
                sections.bootstrapBytes = std::move(sectionBytes);
            }
        }
        return sections;
    }

    const std::string text(bytes.begin(), bytes.end());
    const std::string ropeMarker = "[[ROPE]]";
    const std::string ropeBinMarker = "[[ROPE_BIN]]";
    const std::string erasableMarker = "[[ERASABLE]]";
    const std::string erasableBinMarker = "[[ERASABLE_BIN]]";
    const std::string bootstrapMarker = "[[BOOTSTRAP]]";

    ProgramPackageSections sections;

    auto findMarker = [&](const std::string& marker) {
        return text.find(marker);
    };

    size_t ropePos = findMarker(ropeMarker);
    if (ropePos == std::string::npos) {
        ropePos = findMarker(ropeBinMarker);
        if (ropePos != std::string::npos) sections.ropeIsBinary = true;
    }

    size_t erasablePos = findMarker(erasableMarker);
    if (erasablePos == std::string::npos) {
        erasablePos = findMarker(erasableBinMarker);
        if (erasablePos != std::string::npos) sections.erasableIsBinary = true;
    }

    const auto bootstrapPos = findMarker(bootstrapMarker);

    if (ropePos == std::string::npos || erasablePos == std::string::npos || bootstrapPos == std::string::npos) {
        sections.ropeBytes = bytes;
        return sections;
    }

    size_t ropeStart = ropePos + (sections.ropeIsBinary ? ropeBinMarker.size() : ropeMarker.size());
    size_t erasableStart = erasablePos + (sections.erasableIsBinary ? erasableBinMarker.size() : erasableMarker.size());
    size_t bootstrapStart = bootstrapPos + bootstrapMarker.size();

    sections.ropeBytes.assign(bytes.begin() + ropeStart, bytes.begin() + erasablePos);
    sections.erasableBytes.assign(bytes.begin() + erasableStart, bytes.begin() + bootstrapPos);
    sections.bootstrapBytes.assign(bytes.begin() + bootstrapStart, bytes.end());

    return sections;
}

uint16_t dskyKeycodeForKey(const std::string& key) {
    if (key == "VERB") return 17;
    if (key == "RSET") return 18;
    if (key == "KEY REL") return 25;
    if (key == "+") return 26;
    if (key == "-") return 27;
    if (key == "ENTR") return 28;
    if (key == "CLR") return 30;
    if (key == "NOUN") return 31;
    if (key.size() == 1 && std::isdigit(static_cast<unsigned char>(key[0]))) {
        const int digit = key[0] - '0';
        return digit == 0 ? 16 : static_cast<uint16_t>(digit);
    }
    return 0;
}
}  // namespace

NativeApolloCore::NativeApolloCore()
    : cpu_(new AgcCpu()),
      memoryImage_(new AgcMemoryImage()),
      scenarioBootstrap_(new ScenarioBootstrap()),
      dskyIo_(new DskyIo()),
      alarmExecutive_(new AlarmExecutive()),
      compatibilityScenario_(new CompatibilityScenario()) {}

NativeApolloCore::~NativeApolloCore() {
    delete compatibilityScenario_;
    delete dskyIo_;
    delete alarmExecutive_;
    delete scenarioBootstrap_;
    delete memoryImage_;
    delete cpu_;
}

void NativeApolloCore::initCore() {
    std::scoped_lock lock(mutex_);
    cpu_->initialize();
    memoryImage_->initialize();
    scenarioBootstrap_->initialize();
    dskyIo_->initialize();
    alarmExecutive_->initialize();
}

bool NativeApolloCore::loadProgramImage(const std::vector<uint8_t>& image) {
    std::scoped_lock lock(mutex_);
    if (!cpu_->state().initialized) {
        cpu_->initialize();
    }
    const auto sections = splitProgramPackage(image);

    bool ropeLoaded = false;
    if (sections.ropeIsBinary) {
        ropeLoaded = memoryImage_->loadRopeBinary(sections.ropeBytes);
    } else {
        ropeLoaded = memoryImage_->loadRopeFromBytes(sections.ropeBytes);
    }

    bool erasableLoaded = false;
    if (sections.erasableIsBinary) {
        erasableLoaded = memoryImage_->loadErasableBinary(sections.erasableBytes);
    } else {
        erasableLoaded = memoryImage_->loadErasableFromBytes(sections.erasableBytes);
    }
    const bool ropeMetadataLoaded = memoryImage_->loadRopeMetadataFromBytes(sections.ropeMetadataBytes);
    const bool ropeLabelsLoaded = memoryImage_->loadRopeLabelsFromBytes(sections.ropeLabelBytes);

    const bool bootstrapLoaded = scenarioBootstrap_->loadFromBytes(sections.bootstrapBytes);

    if (!ropeLoaded || !ropeMetadataLoaded || !ropeLabelsLoaded || !erasableLoaded || !bootstrapLoaded) {
        return false;
    }
    cpu_->loadProgramContext(memoryImage_->metadata(), scenarioBootstrap_->data().requestedInitialProgram);
    alarmExecutive_->reset();
    pendingExecutiveRequest_ = PendingExecutiveRequest{};
    activeDispatchTarget_ = ActiveDispatchTarget{};
    compatibilityScenario_->resetFromBootstrap(scenarioBootstrap_->data(), memoryImage_->metadata(), *cpu_, *dskyIo_);
    dskyIo_->setApolloInputRoutingEnabled(hasApolloDskyEntryPoints());
    return true;
}

void NativeApolloCore::resetScenario() {
    std::scoped_lock lock(mutex_);
    if (!memoryImage_->loaded()) {
        return;
    }
    memoryImage_->resetErasable();
    alarmExecutive_->reset();
    pendingExecutiveRequest_ = PendingExecutiveRequest{};
    activeDispatchTarget_ = ActiveDispatchTarget{};
    compatibilityScenario_->resetFromBootstrap(scenarioBootstrap_->data(), memoryImage_->metadata(), *cpu_, *dskyIo_);
    dskyIo_->setApolloInputRoutingEnabled(hasApolloDskyEntryPoints());
}

void NativeApolloCore::pressKey(const std::string& key) {
    std::scoped_lock lock(mutex_);
    if (key == "TRIM_UP" || key == "TRIM_DOWN") {
        compatibilityScenario_->handleKey(key, *dskyIo_);
        return;
    }
    DskyEvent event = dskyIo_->pressKey(key, *cpu_);
    const bool routedToApollo = routeApolloDskyInput(key);
    if (!routedToApollo && event.type == DskyEventType::EXECUTE_PENDING_COMMAND) {
        event = dskyIo_->executePendingCommand(*cpu_);
    }
    compatibilityScenario_->handleDskyEvent(event, *cpu_, *dskyIo_);
}

void NativeApolloCore::stepSimulation(double deltaSeconds) {
    std::scoped_lock lock(mutex_);
    if (!memoryImage_->loaded()) {
        return;
    }
    compatibilityScenario_->step(deltaSeconds, *memoryImage_, *cpu_, *dskyIo_);
    dskyIo_->releaseMomentaryInputs(*cpu_);
    alarmExecutive_->update(*cpu_, *dskyIo_);
    dskyIo_->syncProgramFromCpu(*cpu_);
    dskyIo_->syncExecutionFromCpu(*cpu_, *memoryImage_);
}

CoreState NativeApolloCore::getSnapshot() const {
    std::scoped_lock lock(mutex_);
    return SnapshotBuilder::build(*cpu_, *memoryImage_, *dskyIo_, *compatibilityScenario_);
}

bool NativeApolloCore::hasApolloDskyEntryPoints() const {
    int bank = 0;
    int offset = 0;
    return memoryImage_->findRopeLabel("KEYRUPT1", bank, offset) &&
        memoryImage_->findRopeLabel("ACCEPTUP", bank, offset) &&
        memoryImage_->findRopeLabel("CHARIN", bank, offset) &&
        memoryImage_->findRopeLabel("PROCKEY", bank, offset);
}

bool NativeApolloCore::jumpToLabel(const std::string& label) {
    int bank = 0;
    int offset = 0;
    if (!memoryImage_->findRopeLabel(label, bank, offset)) {
        return false;
    }
    cpu_->jumpToBankOffset(static_cast<uint16_t>(bank), static_cast<uint16_t>(offset));
    return true;
}

bool NativeApolloCore::jumpToLabelWithSwitchedBank(const std::string& label, uint16_t switchedBank) {
    cpu_->setSwitchedFixedBank(switchedBank);
    return jumpToLabel(label);
}

int NativeApolloCore::normalizeFixedAddressForBank(int bank, uint16_t address12) {
    const int address = address12 & 07777;
    if (bank == 02 && address >= 04000) {
        return address - 04000;
    }
    if (bank == 03 && address >= 06000) {
        return address - 06000;
    }
    if (bank >= 04 && address >= 02000) {
        return address - 02000;
    }
    return address & 01777;
}

uint16_t NativeApolloCore::fixedAddressForBankOffset(int bank, int offset) {
    const uint16_t normalizedOffset = static_cast<uint16_t>(offset & 01777);
    if (bank == 02) {
        return static_cast<uint16_t>(04000 | normalizedOffset);
    }
    if (bank == 03) {
        return static_cast<uint16_t>(06000 | normalizedOffset);
    }
    if (bank >= 04) {
        return static_cast<uint16_t>(02000 | normalizedOffset);
    }
    return normalizedOffset;
}

void NativeApolloCore::primeApolloKeyruptLeadInState() {
    const auto& state = cpu_->state();
    memoryImage_->writeErasableWord(kAruptAddress, state.accumulator);
    memoryImage_->writeErasableWord(kLruptAddress, state.lRegister);
    memoryImage_->writeErasableWord(
        kBruptAddress,
        fixedAddressForBankOffset(state.programCounterBank, state.programCounterOffset)
    );
    cpu_->setAccumulator(state.bbRegister);
}

bool NativeApolloCore::dispatchCapturedNovacRequest() {
    const int requestSiteBank = static_cast<int>(cpu_->switchedFixedBank());
    const uint16_t requestSiteAddress = cpu_->state().qRegister & 07777;
    const int requestSiteOffset = normalizeFixedAddressForBank(requestSiteBank, requestSiteAddress);

    PendingExecutiveRequest request;
    request.active = true;
    request.requestSiteLabel = memoryImage_->ropeLabel(requestSiteBank, requestSiteOffset);
    request.targetAddress = memoryImage_->erasableWord(kNewLocAddress) & 07777;
    request.bankWord = memoryImage_->erasableWord(kNewLocAddress + 1) & 077777;

    if (request.targetAddress < 02000) {
        request.targetBank = static_cast<uint16_t>((request.bankWord >> 10) & 037);
        request.targetOffset = static_cast<uint16_t>(request.targetAddress & 01777);
    } else if (request.targetAddress < 04000) {
        request.targetBank = static_cast<uint16_t>((request.bankWord >> 10) & 037);
        request.targetOffset = static_cast<uint16_t>(request.targetAddress & 01777);
    } else if (request.targetAddress < 06000) {
        request.targetBank = 02;
        request.targetOffset = static_cast<uint16_t>(request.targetAddress & 01777);
    } else {
        request.targetBank = 03;
        request.targetOffset = static_cast<uint16_t>(request.targetAddress & 01777);
    }

    request.targetLabel = memoryImage_->ropeLabel(request.targetBank, request.targetOffset);
    pendingExecutiveRequest_ = request;
    logExecutiveTracef(
        "captured request site=%s target=%02o:%04o label=%s targetAddress=%04o bankWord=%05o",
        request.requestSiteLabel.c_str(),
        request.targetBank,
        request.targetOffset,
        request.targetLabel.c_str(),
        request.targetAddress,
        request.bankWord
    );
    return pendingExecutiveRequest_.active;
}

bool NativeApolloCore::dispatchPendingExecutiveRequest() {
    if (!pendingExecutiveRequest_.active) {
        return false;
    }

    activeDispatchTarget_.active = true;
    activeDispatchTarget_.bank = pendingExecutiveRequest_.targetBank;
    activeDispatchTarget_.offset = pendingExecutiveRequest_.targetOffset;
    activeDispatchTarget_.label = pendingExecutiveRequest_.targetLabel;
    cpu_->setAccumulator(pendingExecutiveRequest_.targetAddress);
    cpu_->setLRegister(pendingExecutiveRequest_.bankWord);
    logExecutiveTracef(
        "forced handoff target=%02o:%04o label=%s targetAddress=%04o bankWord=%05o",
        activeDispatchTarget_.bank,
        activeDispatchTarget_.offset,
        activeDispatchTarget_.label.c_str(),
        pendingExecutiveRequest_.targetAddress,
        pendingExecutiveRequest_.bankWord
    );
    if (!jumpToLabel("SUPDXCHZ")) {
        activeDispatchTarget_ = ActiveDispatchTarget{};
        return false;
    }
    pendingExecutiveRequest_ = PendingExecutiveRequest{};
    return true;
}

bool NativeApolloCore::armPendingExecutiveRequestAtCurrentTransferState(bool atSupdxchzPlusOne) {
    if (!pendingExecutiveRequest_.active) {
        return false;
    }

    activeDispatchTarget_.active = true;
    activeDispatchTarget_.bank = pendingExecutiveRequest_.targetBank;
    activeDispatchTarget_.offset = pendingExecutiveRequest_.targetOffset;
    activeDispatchTarget_.label = pendingExecutiveRequest_.targetLabel;

    if (atSupdxchzPlusOne) {
        cpu_->setAccumulator(pendingExecutiveRequest_.bankWord);
        cpu_->setLRegister(pendingExecutiveRequest_.targetAddress);
    } else {
        cpu_->setAccumulator(pendingExecutiveRequest_.targetAddress);
        cpu_->setLRegister(pendingExecutiveRequest_.bankWord);
    }

    logExecutiveTracef(
        "natural transfer state=%s target=%02o:%04o label=%s targetAddress=%04o bankWord=%05o",
        atSupdxchzPlusOne ? "SUPDXCHZ+1" : "SUPDXCHZ",
        activeDispatchTarget_.bank,
        activeDispatchTarget_.offset,
        activeDispatchTarget_.label.c_str(),
        pendingExecutiveRequest_.targetAddress,
        pendingExecutiveRequest_.bankWord
    );

    pendingExecutiveRequest_ = PendingExecutiveRequest{};
    return true;
}

bool NativeApolloCore::continueAfterExecutiveDispatch(int maxInstructions) {
    bool reachedTarget = false;
    int postTargetInstructions = 0;
    uint32_t lastUnsupportedCount = cpu_->state().unsupportedOpcodeCount;
    std::string lastTraceLabel;
    for (int instruction = 0; instruction < maxInstructions; ++instruction) {
        cpu_->stepInstruction(*memoryImage_);
        const auto& state = cpu_->state();
        if (state.unsupportedOpcodeCount != lastUnsupportedCount) {
            lastUnsupportedCount = state.unsupportedOpcodeCount;
            logExecutiveTracef(
                "unsupported post-dispatch pc=%02o:%04o label=%s note=%s",
                state.programCounterBank,
                state.programCounterOffset,
                state.currentLabel.c_str(),
                state.executionNote.c_str()
            );
        }
        if (isExecutiveTraceLabel(state.currentLabel) && state.currentLabel != lastTraceLabel) {
            lastTraceLabel = state.currentLabel;
            logExecutiveTracef(
                "post-dispatch label=%s pc=%02o:%04o note=%s newjob=%05o loc=%05o bankset=%05o priority=%05o",
                state.currentLabel.c_str(),
                state.programCounterBank,
                state.programCounterOffset,
                state.executionNote.c_str(),
                memoryImage_->erasableWord(kNewJobAddress),
                memoryImage_->erasableWord(kLocAddress),
                memoryImage_->erasableWord(kBankSetAddress),
                memoryImage_->erasableWord(kPriorityAddress)
            );
        }
        if (state.currentLabel == "CHARIN2") {
            activeDispatchTarget_ = ActiveDispatchTarget{};
            return true;
        }
        if (isExecutiveCompletionBoundaryLabel(state.currentLabel)) {
            logExecutiveTracef(
                "completion boundary=%s pc=%02o:%04o",
                state.currentLabel.c_str(),
                state.programCounterBank,
                state.programCounterOffset
            );
            activeDispatchTarget_ = ActiveDispatchTarget{};
            return true;
        }
        if (!reachedTarget && activeDispatchTarget_.active) {
            const bool labelMatched =
                !activeDispatchTarget_.label.empty() && state.currentLabel == activeDispatchTarget_.label;
            const bool addressMatched =
                state.programCounterBank == activeDispatchTarget_.bank &&
                state.programCounterOffset == activeDispatchTarget_.offset;
            if (labelMatched || addressMatched) {
                reachedTarget = true;
                postTargetInstructions = 0;
                logExecutiveTracef(
                    "dispatch target active pc=%02o:%04o label=%s",
                    state.programCounterBank,
                    state.programCounterOffset,
                    state.currentLabel.c_str()
                );
            }
        } else if (reachedTarget) {
            postTargetInstructions += 1;
            if (postTargetInstructions >= kPostDispatchTargetBudget) {
                logExecutiveTracef(
                    "post-dispatch budget exhausted target=%02o:%04o label=%s",
                    activeDispatchTarget_.bank,
                    activeDispatchTarget_.offset,
                    activeDispatchTarget_.label.c_str()
                );
                activeDispatchTarget_ = ActiveDispatchTarget{};
                return true;
            }
        }
        if (state.programCounterBank == 02 && state.programCounterOffset == 01101) {
            if (!dispatchCapturedNovacRequest()) {
                return false;
            }
            if (!dispatchPendingExecutiveRequest()) {
                return false;
            }
            reachedTarget = false;
            postTargetInstructions = 0;
        }
    }
    activeDispatchTarget_ = ActiveDispatchTarget{};
    return true;
}

bool NativeApolloCore::continueFinalTransitionToNaturalTransfer(int maxInstructions) {
    uint32_t lastUnsupportedCount = cpu_->state().unsupportedOpcodeCount;
    std::string lastTraceLabel;
    for (int instruction = 0; instruction < maxInstructions; ++instruction) {
        cpu_->stepInstruction(*memoryImage_);
        const auto& state = cpu_->state();
        if (state.unsupportedOpcodeCount != lastUnsupportedCount) {
            lastUnsupportedCount = state.unsupportedOpcodeCount;
            logExecutiveTracef(
                "unsupported final-transition pc=%02o:%04o label=%s note=%s",
                state.programCounterBank,
                state.programCounterOffset,
                state.currentLabel.c_str(),
                state.executionNote.c_str()
            );
        }
        if (isExecutiveTraceLabel(state.currentLabel) && state.currentLabel != lastTraceLabel) {
            lastTraceLabel = state.currentLabel;
            logExecutiveTracef(
                "final-transition label=%s pc=%02o:%04o note=%s newjob=%05o loc=%05o bankset=%05o priority=%05o",
                state.currentLabel.c_str(),
                state.programCounterBank,
                state.programCounterOffset,
                state.executionNote.c_str(),
                memoryImage_->erasableWord(kNewJobAddress),
                memoryImage_->erasableWord(kLocAddress),
                memoryImage_->erasableWord(kBankSetAddress),
                memoryImage_->erasableWord(kPriorityAddress)
            );
        }
        if (state.programCounterBank == 02 && state.programCounterOffset == 01101) {
            if (!dispatchCapturedNovacRequest()) {
                return false;
            }
            continue;
        }
        if (state.currentLabel == "CHARIN2") {
            return true;
        }
        if (!pendingExecutiveRequest_.active) {
            return true;
        }
        if (isNaturalSupdxchzTransferState(*cpu_)) {
            const bool atSupdxchzPlusOne = state.programCounterOffset == kSupdxchzPlusOneOffset;
            if (!armPendingExecutiveRequestAtCurrentTransferState(atSupdxchzPlusOne)) {
                return false;
            }
            return continueAfterExecutiveDispatch(512);
        }
    }
    return false;
}

bool NativeApolloCore::runInstructionRoutedApolloInput(
    const std::string& entryLabel,
    int maxInstructions
) {
    pendingExecutiveRequest_ = PendingExecutiveRequest{};
    activeDispatchTarget_ = ActiveDispatchTarget{};
    bool sawResume = false;
    bool sawFinalPreTransferTransition = false;
    uint32_t lastUnsupportedCount = cpu_->state().unsupportedOpcodeCount;
    std::string lastTraceLabel;
    logExecutiveTracef("routed input start entry=%s maxInstructions=%d", entryLabel.c_str(), maxInstructions);
    primeApolloKeyruptLeadInState();
    if (!jumpToLabelWithSwitchedBank(entryLabel, 04)) {
        logExecutiveTracef("routed input jump failed entry=%s", entryLabel.c_str());
        return false;
    }

    for (int instruction = 0; instruction < maxInstructions; ++instruction) {
        cpu_->stepInstruction(*memoryImage_);

        const auto& state = cpu_->state();
        if (instruction < 24) {
            logExecutiveTracef(
                "routed step=%d pc=%02o:%04o label=%s note=%s q=%05o a=%05o l=%05o",
                instruction,
                state.programCounterBank,
                state.programCounterOffset,
                state.currentLabel.c_str(),
                state.executionNote.c_str(),
                state.qRegister,
                state.accumulator,
                state.lRegister
            );
        }
        if (state.unsupportedOpcodeCount != lastUnsupportedCount) {
            lastUnsupportedCount = state.unsupportedOpcodeCount;
            logExecutiveTracef(
                "unsupported routed pc=%02o:%04o label=%s note=%s",
                state.programCounterBank,
                state.programCounterOffset,
                state.currentLabel.c_str(),
                state.executionNote.c_str()
            );
        }
        if (isExecutiveTraceLabel(state.currentLabel) && state.currentLabel != lastTraceLabel) {
            lastTraceLabel = state.currentLabel;
            logExecutiveTracef(
                "routed label=%s pc=%02o:%04o note=%s newjob=%05o loc=%05o bankset=%05o priority=%05o",
                state.currentLabel.c_str(),
                state.programCounterBank,
                state.programCounterOffset,
                state.executionNote.c_str(),
                memoryImage_->erasableWord(kNewJobAddress),
                memoryImage_->erasableWord(kLocAddress),
                memoryImage_->erasableWord(kBankSetAddress),
                memoryImage_->erasableWord(kPriorityAddress)
            );
        }
        if (isFinalPreTransferTransitionLabel(state.currentLabel)) {
            sawFinalPreTransferTransition = true;
        }
        if (state.programCounterBank == 02 && state.programCounterOffset == 01101) {
            if (!dispatchCapturedNovacRequest()) {
                return false;
            }
            continue;
        }

        if (state.executionNote == "RESUME") {
            sawResume = true;
            logExecutiveTracef(
                "resume pc=%02o:%04o newjob=%05o loc=%05o bankset=%05o priority=%05o",
                state.programCounterBank,
                state.programCounterOffset,
                memoryImage_->erasableWord(kNewJobAddress),
                memoryImage_->erasableWord(kLocAddress),
                memoryImage_->erasableWord(kBankSetAddress),
                memoryImage_->erasableWord(kPriorityAddress)
            );
        } else if (sawResume && pendingExecutiveRequest_.active) {
            if (isNaturalSupdxchzTransferState(*cpu_)) {
                const bool atSupdxchzPlusOne = state.programCounterOffset == kSupdxchzPlusOneOffset;
                if (!armPendingExecutiveRequestAtCurrentTransferState(atSupdxchzPlusOne)) {
                    return false;
                }
                return continueAfterExecutiveDispatch(512);
            }
        }

        if (!pendingExecutiveRequest_.active) {
            if (state.currentLabel == "CHARIN2") {
                logExecutiveTrace("routed input completed at CHARIN2 without pending request");
                return true;
            }
            continue;
        }

        if (state.currentLabel == "CHARIN2") {
            logExecutiveTrace("routed input completed at CHARIN2 with pending request already cleared");
            return true;
        }
    }

    if (pendingExecutiveRequest_.active) {
        if (sawFinalPreTransferTransition) {
            if (continueFinalTransitionToNaturalTransfer(256)) {
                return true;
            }
        }
        logExecutiveTracef(
            "routed-step exhaustion pending target=%02o:%04o label=%s newjob=%05o loc=%05o bankset=%05o priority=%05o",
            pendingExecutiveRequest_.targetBank,
            pendingExecutiveRequest_.targetOffset,
            pendingExecutiveRequest_.targetLabel.c_str(),
            memoryImage_->erasableWord(kNewJobAddress),
            memoryImage_->erasableWord(kLocAddress),
            memoryImage_->erasableWord(kBankSetAddress),
            memoryImage_->erasableWord(kPriorityAddress)
        );
        if (!dispatchPendingExecutiveRequest()) {
            return false;
        }
        return continueAfterExecutiveDispatch(512);
    }
    logExecutiveTrace("routed input ended without pending request");
    return true;
}

bool NativeApolloCore::routeApolloDskyInput(const std::string& key) {
    if (key == "PRO") {
        logExecutiveTrace("route key=PRO");
        if (!jumpToLabelWithSwitchedBank("PROCKEY", 040)) {
            logExecutiveTrace("route PRO jump failed");
            return false;
        }
        for (int instruction = 0; instruction < 48; ++instruction) {
            cpu_->stepInstruction(*memoryImage_);
            if (cpu_->state().currentLabel == "VBPROC") {
                break;
            }
        }
        return true;
    }

    const uint16_t keycode = dskyKeycodeForKey(key);
    if (keycode == 0) {
        logExecutiveTracef("route key=%s rejected unknown keycode", key.c_str());
        return false;
    }
    logExecutiveTracef(
        "route key=%s keycode=%u entryPoints=%s",
        key.c_str(),
        keycode,
        hasApolloDskyEntryPoints() ? "yes" : "no"
    );
    memoryImage_->writeErasableWord(kMpacAddress, keycode);
    memoryImage_->writeErasableWord(kNewLocAddress, 0);
    memoryImage_->writeErasableWord(kNewLocAddress + 1, 0);
    return runInstructionRoutedApolloInput("KEYRUPT1", 1024);
}

}  // namespace apollo
