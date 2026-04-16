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
    struct PendingExecutiveRequest {
        bool active = false;
        uint16_t targetAddress = 0;
        uint16_t bankWord = 0;
        uint16_t targetBank = 0;
        uint16_t targetOffset = 0;
        std::string requestSiteLabel;
        std::string targetLabel;
    };

    bool runInstructionRoutedApolloInput(
        const std::string& entryLabel,
        int maxInstructions
    );
    bool jumpToLabel(const std::string& label);
    bool jumpToLabelWithSwitchedBank(const std::string& label, uint16_t switchedBank);
    bool dispatchCapturedNovacRequest();
    bool dispatchPendingExecutiveRequest();
    bool continueAfterExecutiveDispatch(int maxInstructions);
    static int normalizeFixedAddressForBank(int bank, uint16_t address12);
    static uint16_t fixedAddressForBankOffset(int bank, int offset);
    void primeApolloKeyruptLeadInState();
    bool hasApolloDskyEntryPoints() const;
    bool routeApolloDskyInput(const std::string& key);
    mutable std::mutex mutex_;
    AgcCpu* cpu_;
    AgcMemoryImage* memoryImage_;
    ScenarioBootstrap* scenarioBootstrap_;
    DskyIo* dskyIo_;
    AlarmExecutive* alarmExecutive_;
    CompatibilityScenario* compatibilityScenario_;
    PendingExecutiveRequest pendingExecutiveRequest_;
};

}  // namespace apollo
