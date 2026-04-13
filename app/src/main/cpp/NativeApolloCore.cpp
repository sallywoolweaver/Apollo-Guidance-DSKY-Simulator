#include "NativeApolloCore.h"

#include "AgcCpu.h"
#include "AgcMemoryImage.h"
#include "AlarmExecutive.h"
#include "CompatibilityScenario.h"
#include "DskyIo.h"
#include "ScenarioBootstrap.h"
#include "SnapshotBuilder.h"

#include <string>

namespace apollo {

namespace {
struct ProgramPackageSections {
    std::vector<uint8_t> ropeBytes;
    std::vector<uint8_t> erasableBytes;
    std::vector<uint8_t> bootstrapBytes;
};

ProgramPackageSections splitProgramPackage(const std::vector<uint8_t>& bytes) {
    const std::string text(bytes.begin(), bytes.end());
    const std::string ropeMarker = "[[ROPE]]";
    const std::string erasableMarker = "[[ERASABLE]]";
    const std::string bootstrapMarker = "[[BOOTSTRAP]]";
    ProgramPackageSections sections;
    const auto ropePos = text.find(ropeMarker);
    const auto erasablePos = text.find(erasableMarker);
    const auto bootstrapPos = text.find(bootstrapMarker);
    if (ropePos == std::string::npos || erasablePos == std::string::npos || bootstrapPos == std::string::npos ||
        erasablePos < ropePos || bootstrapPos < erasablePos) {
        sections.ropeBytes = bytes;
        return sections;
    }
    const auto ropeStart = ropePos + ropeMarker.size();
    const auto ropeContent = text.substr(ropeStart, erasablePos - ropeStart);
    const auto erasableStart = erasablePos + erasableMarker.size();
    const auto erasableContent = text.substr(erasableStart, bootstrapPos - erasableStart);
    const auto bootstrapStart = bootstrapPos + bootstrapMarker.size();
    const auto bootstrapContent = text.substr(bootstrapStart);
    sections.ropeBytes.assign(ropeContent.begin(), ropeContent.end());
    sections.erasableBytes.assign(erasableContent.begin(), erasableContent.end());
    sections.bootstrapBytes.assign(bootstrapContent.begin(), bootstrapContent.end());
    return sections;
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
    const bool ropeLoaded = memoryImage_->loadRopeFromBytes(sections.ropeBytes);
    const bool erasableLoaded = memoryImage_->loadErasableFromBytes(sections.erasableBytes);
    const bool bootstrapLoaded = scenarioBootstrap_->loadFromBytes(sections.bootstrapBytes);
    if (!ropeLoaded || !erasableLoaded || !bootstrapLoaded) {
        return false;
    }
    cpu_->loadProgramContext(memoryImage_->metadata(), scenarioBootstrap_->data().requestedInitialProgram);
    alarmExecutive_->reset();
    compatibilityScenario_->resetFromBootstrap(scenarioBootstrap_->data(), memoryImage_->metadata(), *cpu_, *dskyIo_);
    return true;
}

void NativeApolloCore::resetScenario() {
    std::scoped_lock lock(mutex_);
    if (!memoryImage_->loaded()) {
        return;
    }
    memoryImage_->resetErasable();
    alarmExecutive_->reset();
    compatibilityScenario_->resetFromBootstrap(scenarioBootstrap_->data(), memoryImage_->metadata(), *cpu_, *dskyIo_);
}

void NativeApolloCore::pressKey(const std::string& key) {
    std::scoped_lock lock(mutex_);
    if (key == "TRIM_UP" || key == "TRIM_DOWN") {
        compatibilityScenario_->handleKey(key, *dskyIo_);
        return;
    }
    DskyEvent event = dskyIo_->pressKey(key);
    if (event.type == DskyEventType::EXECUTE_PENDING_COMMAND) {
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
    alarmExecutive_->update(*cpu_, *dskyIo_);
    dskyIo_->syncProgramFromCpu(*cpu_);
    dskyIo_->syncExecutionFromCpu(*cpu_, *memoryImage_);
}

CoreState NativeApolloCore::getSnapshot() const {
    std::scoped_lock lock(mutex_);
    return SnapshotBuilder::build(*cpu_, *memoryImage_, *dskyIo_, *compatibilityScenario_);
}

}  // namespace apollo
