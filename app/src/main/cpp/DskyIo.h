#pragma once

#include <string>

#include "AgcTypes.h"

namespace apollo {

class AgcCpu;
class AgcMemoryImage;

enum class DskyEventType {
    NONE,
    EXECUTE_PENDING_COMMAND,
    SCENARIO_RESET_REQUESTED,
    ACKNOWLEDGE_ALARM,
    RESET,
    KEY_RELEASE,
};

struct DskyEvent {
    DskyEventType type = DskyEventType::NONE;
};

class DskyIo {
  public:
    void initialize();
    void resetForProgram(const std::string& initialProgram);
    DskyEvent pressKey(const std::string& key, AgcCpu& cpu);
    DskyEvent executePendingCommand(AgcCpu& cpu);
    void releaseMomentaryInputs(AgcCpu& cpu);
    void syncProgramFromCpu(const AgcCpu& cpu);
    void syncExecutionFromCpu(const AgcCpu& cpu, const AgcMemoryImage& memoryImage);

    void setProgram(const std::string& program);
    void setLandingTelemetry(
        double altitudeMeters,
        double verticalVelocityMps,
        double horizontalVelocityMps,
        double fuelKg
    );
    void setRegisters(const std::string& r1, const std::string& r2, const std::string& r3);
    void setPhase(const std::string& phaseLabel, const std::string& phaseProgram);
    void setStatus(const std::string& status);
    void setDisplayMode(DisplayMode displayMode);
    void setAlarm(
        const std::string& code,
        const std::string& title,
        bool needsAcknowledgement,
        int totalAlarms
    );
    void clearAlarm();
    void setOpError(const std::string& reason);
    void setCompatibilityActivityLight(bool active);

    std::string program() const { return program_; }
    std::string verb() const { return verb_; }
    std::string noun() const { return noun_; }
    std::string register1() const { return register1_; }
    std::string register2() const { return register2_; }
    std::string register3() const { return register3_; }
    std::string phaseLabel() const { return phaseLabel_; }
    std::string phaseProgram() const { return phaseProgram_; }
    std::string statusLine() const { return statusLine_; }
    std::string alarmCode() const { return activeAlarmCode_; }
    std::string alarmTitle() const { return activeAlarmTitle_; }
    bool alarmNeedsAcknowledgement() const { return alarmNeedsAcknowledgement_; }
    int totalAlarms() const { return totalAlarms_; }
    bool progLight() const { return progLight_; }
    bool oprErrLight() const { return oprErrLight_; }
    bool compActyLight() const { return compActyLight_; }
    bool keyRelLight() const { return keyRelLight_; }
    DisplayMode displayMode() const { return displayMode_; }
    bool hasApolloDisplayOutput() const { return hasApolloDisplayOutput_; }

  private:
    void refreshRegisters();
    void syncApolloDisplayFromCpu(const AgcCpu& cpu);
    static std::string signedWhole(double value);
    static std::string signedTenth(double value);

    std::string program_ = "63";
    std::string verb_ = "16";
    std::string noun_ = "68";
    std::string register1_ = "+02400";
    std::string register2_ = "-00340";
    std::string register3_ = "+00835";
    std::string phaseLabel_ = "Braking";
    std::string phaseProgram_ = "63";
    std::string statusLine_ = "Native core idle";
    std::string activeAlarmCode_;
    std::string activeAlarmTitle_;
    int totalAlarms_ = 0;
    bool alarmNeedsAcknowledgement_ = false;
    bool progLight_ = false;
    bool oprErrLight_ = false;
    bool compActyLight_ = false;
    bool keyRelLight_ = false;
    bool hasApolloDisplayOutput_ = false;
    bool pendingProceedRelease_ = false;
    EntryField entryField_ = EntryField::NONE;
    std::string entryBuffer_;
    DisplayMode displayMode_ = DisplayMode::LANDING_MONITOR;
    double altitudeMeters_ = 2400.0;
    double verticalVelocityMps_ = -34.0;
    double horizontalVelocityMps_ = 24.0;
    double fuelKg_ = 835.0;
};

}  // namespace apollo
