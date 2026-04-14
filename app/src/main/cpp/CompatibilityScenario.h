#pragma once

#include <string>

#include "AgcCpu.h"
#include "AgcMemoryImage.h"
#include "DskyIo.h"
#include "ScenarioBootstrap.h"

namespace apollo {

class CompatibilityScenario {
  public:
    void resetFromBootstrap(const ScenarioBootstrapData& bootstrap, const AgcProgramImage& image, AgcCpu& cpu, DskyIo& dsky);
    void handleDskyEvent(const DskyEvent& event, AgcCpu& cpu, DskyIo& dsky);
    void handleKey(const std::string& key, DskyIo& dsky);
    void step(double deltaSeconds, AgcMemoryImage& memoryImage, AgcCpu& cpu, DskyIo& dsky);

    double altitudeMeters() const { return altitudeMeters_; }
    double verticalVelocityMps() const { return verticalVelocityMps_; }
    double horizontalVelocityMps() const { return horizontalVelocityMps_; }
    double fuelKg() const { return fuelKg_; }
    double throttle() const { return throttle_; }
    double targetVerticalVelocityMps() const { return targetVerticalVelocityMps_; }
    double missionTimeSeconds() const { return missionTimeSeconds_; }
    double loadRatio() const { return loadRatio_; }
    double throttleTrim() const { return throttleTrim_; }
    std::string missionResult() const { return missionResult_; }
    std::string missionResultSummary() const { return missionResultSummary_; }
    std::string lastEvent() const { return lastEvent_; }

  private:
    void updateGuidanceTargets(const DskyIo& dsky);
    void updateLoadAndAlarms(AgcCpu& cpu, DskyIo& dsky);
    void updatePhaseAndOutcome(AgcCpu& cpu, DskyIo& dsky);

    double altitudeMeters_ = 2400.0;
    double verticalVelocityMps_ = -34.0;
    double horizontalVelocityMps_ = 24.0;
    double fuelKg_ = 835.0;
    double throttle_ = 0.62;
    double initialAltitudeMeters_ = 2400.0;
    double initialVerticalVelocityMps_ = -34.0;
    double initialHorizontalVelocityMps_ = 24.0;
    double initialFuelKg_ = 835.0;
    double initialThrottle_ = 0.62;
    int initialFirstRopeBank_ = 0;
    int initialFirstRopeOffset_ = 0;
    std::string initialRequestedProgram_ = "63";
    double targetVerticalVelocityMps_ = -30.0;
    double missionTimeSeconds_ = 0.0;
    double loadRatio_ = 0.0;
    double throttleTrim_ = 0.0;
    std::string missionResult_;
    std::string missionResultSummary_;
    std::string lastEvent_ = "Awaiting program image";
    int totalAlarms_ = 0;
};

}  // namespace apollo
