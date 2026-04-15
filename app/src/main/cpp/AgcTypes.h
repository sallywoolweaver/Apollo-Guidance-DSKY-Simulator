#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace apollo {

enum class EntryField {
    NONE,
    VERB,
    NOUN,
};

enum class DisplayMode {
    LANDING_MONITOR,
    PHASE_SUMMARY,
    LAST_ALARM,
};

enum class CpuRunState {
    IDLE,
    EXECUTING,
    HALTED,
};

struct CoreState {
    bool initialized = false;
    bool programLoaded = false;
    std::string program = "63";
    std::string verb = "16";
    std::string noun = "68";
    std::string register1 = "+02400";
    std::string register2 = "-00340";
    std::string register3 = "+00835";
    std::string phaseLabel = "Braking";
    std::string phaseProgram = "63";
    std::string statusLine = "Native core idle";
    std::string activeAlarmCode;
    std::string activeAlarmTitle;
    int totalAlarms = 0;
    bool alarmNeedsAcknowledgement = false;
    bool progLight = false;
    bool oprErrLight = false;
    bool compActyLight = true;
    bool keyRelLight = false;
    std::string currentLabel;
    int programCounterBank = 0;
    int programCounterOffset = 0;
    std::string executionNote;
    double altitudeMeters = 2400.0;
    double verticalVelocityMps = -34.0;
    double horizontalVelocityMps = 24.0;
    double fuelKg = 835.0;
    double throttle = 0.62;
    double targetVerticalVelocityMps = -30.0;
    double missionTimeSeconds = 0.0;
    double loadRatio = 0.0;
    double throttleTrim = 0.0;
    std::string missionResult;
    std::string missionResultSummary;
    std::string lastEvent = "Awaiting program image";
};

struct AgcProgramImage {
    bool loaded = false;
    std::string sourceTag = "rope-image";
    std::string sourceKind = "custom-bootstrap";
    int loadedRopeWords = 0;
    int loadedErasableWords = 0;
    bool containsApolloDerivedWords = false;
    int firstRopeBank = 0;
    int firstRopeOffset = 0;
};

struct AgcCpuState {
    bool initialized = false;
    CpuRunState runState = CpuRunState::IDLE;
    uint64_t cycleCounter = 0;
    uint64_t executedInstructions = 0;
    uint32_t restartCounter = 0;
    int currentMajorMode = 63;
    uint16_t accumulator = 0;
    uint16_t lRegister = 0;
    uint16_t qRegister = 0;
    uint16_t ebRegister = 0;
    uint16_t fbRegister = 0;
    uint16_t bbRegister = 0;
    uint16_t indexRegister = 0;
    bool interruptsEnabled = true;
    uint16_t outputChannel7 = 0;
    bool extendFlag = false;
    uint16_t lastFetchedWord = 0;
    uint16_t lastOpcodeClass = 0;
    uint32_t unsupportedOpcodeCount = 0;
    uint32_t supportedOpcodeSkeletonCount = 0;
    int programCounterBank = 0;
    int programCounterOffset = 0;
    std::string currentLabel;
    std::vector<std::string> executedLabels;
    std::string executionNote = "CPU idle";
};

}  // namespace apollo
