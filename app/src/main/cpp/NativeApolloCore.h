#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "AgcTypes.h"

namespace apollo {

class AgcCpu;
class AgcMemoryImage;
class DskyIo;
class CompatibilityScenario;
class ScenarioBootstrap;
class AlarmExecutive;

class NativeApolloCore {
  public:
    NativeApolloCore();
    ~NativeApolloCore();

    void initCore();
    bool loadProgramImage(const std::vector<uint8_t>& image);
    void resetScenario();
    void pressKey(const std::string& key);
    void stepSimulation(double deltaSeconds);
    CoreState getSnapshot() const;

  private:
    mutable std::mutex mutex_;
    AgcCpu* cpu_;
    AgcMemoryImage* memoryImage_;
    ScenarioBootstrap* scenarioBootstrap_;
    DskyIo* dskyIo_;
    AlarmExecutive* alarmExecutive_;
    CompatibilityScenario* compatibilityScenario_;
};

}  // namespace apollo
