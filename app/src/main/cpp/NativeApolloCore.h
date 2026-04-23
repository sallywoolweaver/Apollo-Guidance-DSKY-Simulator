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
    struct ActiveDispatchTarget {
        bool active = false;
        uint16_t bank = 0;
        uint16_t offset = 0;
        std::string label;
    };

    struct PendingExecutiveRequest {
        bool active = false;
        uint16_t targetAddress = 0;
        uint16_t bankWord = 0;
        uint16_t targetEbank = 0;
        uint16_t targetSuperbank = 0;
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
    bool armPendingExecutiveRequestAtCurrentTransferState(bool atSupdxchzPlusOne);
    bool continueAfterExecutiveDispatch(int maxInstructions);
    bool continueFinalTransitionToNaturalTransfer(int maxInstructions);
    static int normalizeFixedAddressForBank(int bank, uint16_t address12);
    static uint16_t fixedAddressForBankOffset(int bank, int offset);
    static uint16_t decode2CadrEbank(uint16_t bankWord);
    static uint16_t decode2CadrSuperbank(uint16_t bankWord);
    static uint16_t decode2CadrFixedBank(uint16_t targetAddress, uint16_t bankWord);
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
    ActiveDispatchTarget activeDispatchTarget_;
};

}  // namespace apollo
