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
#include <string>

namespace apollo {

namespace {
constexpr uint16_t kMpacAddress = 0154;
constexpr uint16_t kNewLocAddress = 065;
constexpr uint16_t kAruptAddress = 010;
constexpr uint16_t kLruptAddress = 011;
constexpr uint16_t kBruptAddress = 017;

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
    pendingExecutiveRequestLabel_.clear();
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
    pendingExecutiveRequestLabel_.clear();
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
    const int requestBank = static_cast<int>(cpu_->switchedFixedBank());
    const uint16_t requestAddress = cpu_->state().qRegister & 07777;
    const int requestOffset = normalizeFixedAddressForBank(requestBank, requestAddress);
    pendingExecutiveRequestLabel_ = memoryImage_->ropeLabel(requestBank, requestOffset);
    return !pendingExecutiveRequestLabel_.empty();
}

bool NativeApolloCore::dispatchPendingExecutiveRequest() {
    if (pendingExecutiveRequestLabel_ == "CHARIN_2CADR") {
        pendingExecutiveRequestLabel_.clear();
        return jumpToLabelWithSwitchedBank("CHARIN", 040);
    }
    return false;
}

bool NativeApolloCore::runInstructionRoutedApolloInput(
    const std::string& entryLabel,
    int maxInstructions
) {
    pendingExecutiveRequestLabel_.clear();
    primeApolloKeyruptLeadInState();
    if (!jumpToLabelWithSwitchedBank(entryLabel, 04)) {
        return false;
    }

    for (int instruction = 0; instruction < maxInstructions; ++instruction) {
        cpu_->stepInstruction(*memoryImage_);

        const auto& state = cpu_->state();
        if (state.programCounterBank == 02 && state.programCounterOffset == 01101) {
            if (!dispatchCapturedNovacRequest()) {
                return false;
            }
            continue;
        }

        if (pendingExecutiveRequestLabel_.empty()) {
            if (state.currentLabel == "CHARIN2") {
                return true;
            }
            continue;
        }

        if (state.currentLabel == "CHARIN2") {
            return true;
        }
    }

    if (!pendingExecutiveRequestLabel_.empty()) {
        if (!dispatchPendingExecutiveRequest()) {
            return false;
        }
    }
    return true;
}

bool NativeApolloCore::routeApolloDskyInput(const std::string& key) {
    if (key == "PRO") {
        if (!jumpToLabelWithSwitchedBank("PROCKEY", 040)) {
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
        return false;
    }
    memoryImage_->writeErasableWord(kMpacAddress, keycode);
    memoryImage_->writeErasableWord(kNewLocAddress, 0);
    memoryImage_->writeErasableWord(kNewLocAddress + 1, 0);
    return runInstructionRoutedApolloInput("KEYRUPT1", 384);
}

}  // namespace apollo
