#include "CompatibilityScenario.h"

#include <algorithm>

namespace apollo {

namespace {
constexpr double kLunarGravity = 1.62;
constexpr double kMaxUpwardAcceleration = 5.35;
constexpr double kFullThrottleFuelFlow = 5.4;

double clamp(double value, double minimum, double maximum) {
    return std::max(minimum, std::min(value, maximum));
}
}

void CompatibilityScenario::resetFromBootstrap(const ScenarioBootstrapData& bootstrap, const AgcProgramImage& image, AgcCpu& cpu, DskyIo& dsky) {
    initialRequestedProgram_ = bootstrap.requestedInitialProgram;
    initialFirstRopeBank_ = image.firstRopeBank;
    initialFirstRopeOffset_ = image.firstRopeOffset;
    initialAltitudeMeters_ = bootstrap.initialAltitudeMeters;
    initialVerticalVelocityMps_ = bootstrap.initialVerticalVelocityMps;
    initialHorizontalVelocityMps_ = bootstrap.initialHorizontalVelocityMps;
    initialFuelKg_ = bootstrap.initialFuelKg;
    initialThrottle_ = bootstrap.initialThrottle;
    altitudeMeters_ = initialAltitudeMeters_;
    verticalVelocityMps_ = initialVerticalVelocityMps_;
    horizontalVelocityMps_ = initialHorizontalVelocityMps_;
    fuelKg_ = initialFuelKg_;
    throttle_ = initialThrottle_;
    targetVerticalVelocityMps_ = -30.0;
    missionTimeSeconds_ = 0.0;
    loadRatio_ = 0.0;
    throttleTrim_ = 0.0;
    missionResult_.clear();
    missionResultSummary_.clear();
    lastEvent_ = "P63 initialized";
    totalAlarms_ = 0;
    AgcProgramImage cpuImage = image;
    cpuImage.loaded = true;
    cpu.resetForScenario(cpuImage, initialRequestedProgram_);
    dsky.resetForProgram(std::to_string(cpu.state().currentMajorMode));
    dsky.syncProgramFromCpu(cpu);
    dsky.setDisplayMode(DisplayMode::LANDING_MONITOR);
    dsky.setPhase("Braking", std::to_string(cpu.state().currentMajorMode));
    dsky.setStatus("Program image loaded");
    dsky.clearAlarm();
    dsky.setLandingTelemetry(altitudeMeters_, verticalVelocityMps_, horizontalVelocityMps_, fuelKg_);
    updateGuidanceTargets(dsky);
}

void CompatibilityScenario::handleDskyEvent(const DskyEvent& event, AgcCpu& cpu, DskyIo& dsky) {
    switch (event.type) {
        case DskyEventType::EXECUTE_PENDING_COMMAND:
            break;
        case DskyEventType::SCENARIO_RESET_REQUESTED: {
            AgcProgramImage image;
            image.loaded = true;
            image.firstRopeBank = initialFirstRopeBank_;
            image.firstRopeOffset = initialFirstRopeOffset_;
            ScenarioBootstrapData bootstrap;
            bootstrap.loaded = true;
            bootstrap.requestedInitialProgram = initialRequestedProgram_;
            bootstrap.initialAltitudeMeters = initialAltitudeMeters_;
            bootstrap.initialVerticalVelocityMps = initialVerticalVelocityMps_;
            bootstrap.initialHorizontalVelocityMps = initialHorizontalVelocityMps_;
            bootstrap.initialFuelKg = initialFuelKg_;
            bootstrap.initialThrottle = initialThrottle_;
            resetFromBootstrap(bootstrap, image, cpu, dsky);
            break;
        }
        case DskyEventType::ACKNOWLEDGE_ALARM:
            dsky.clearAlarm();
            dsky.setStatus("Alarm acknowledged");
            break;
        case DskyEventType::RESET:
            dsky.clearAlarm();
            break;
        case DskyEventType::KEY_RELEASE:
        case DskyEventType::NONE:
            break;
    }
}

void CompatibilityScenario::handleKey(const std::string& key, DskyIo& dsky) {
    if (key == "TRIM_UP") {
        throttleTrim_ = clamp(throttleTrim_ + 0.08, -1.0, 1.0);
        dsky.setStatus("Throttle trim increased");
    } else if (key == "TRIM_DOWN") {
        throttleTrim_ = clamp(throttleTrim_ - 0.08, -1.0, 1.0);
        dsky.setStatus("Throttle trim decreased");
    }
}

void CompatibilityScenario::step(double deltaSeconds, AgcMemoryImage& memoryImage, AgcCpu& cpu, DskyIo& dsky) {
    if (!missionResult_.empty()) {
        return;
    }

    cpu.step(deltaSeconds, memoryImage);
    updateGuidanceTargets(dsky);
    throttle_ = clamp((kLunarGravity + (targetVerticalVelocityMps_ - verticalVelocityMps_) * 0.12) / kMaxUpwardAcceleration + throttleTrim_ * 0.16, 0.0, 1.0);
    if (fuelKg_ <= 0.0) {
        throttle_ = 0.0;
    }

    const double netAcceleration = throttle_ * kMaxUpwardAcceleration - kLunarGravity;
    verticalVelocityMps_ += netAcceleration * deltaSeconds;
    altitudeMeters_ = std::max(0.0, altitudeMeters_ + verticalVelocityMps_ * deltaSeconds);
    fuelKg_ = std::max(0.0, fuelKg_ - kFullThrottleFuelFlow * throttle_ * deltaSeconds);
    const double damping = dsky.phaseProgram() == "63" ? 0.010 : (dsky.phaseProgram() == "64" ? 0.018 : 0.028);
    if (altitudeMeters_ > 0.0) {
        horizontalVelocityMps_ = std::max(0.0, horizontalVelocityMps_ - damping * deltaSeconds * (1.0 + throttle_));
    }
    missionTimeSeconds_ += deltaSeconds;

    updateLoadAndAlarms(cpu, dsky);
    updatePhaseAndOutcome(cpu, dsky);
    dsky.setLandingTelemetry(altitudeMeters_, verticalVelocityMps_, horizontalVelocityMps_, fuelKg_);
}

void CompatibilityScenario::updateGuidanceTargets(const DskyIo& dsky) {
    if (dsky.phaseProgram() == "63") {
        targetVerticalVelocityMps_ = -clamp(12.0 + altitudeMeters_ / 95.0, 16.0, 42.0);
    } else if (dsky.phaseProgram() == "64") {
        targetVerticalVelocityMps_ = -clamp(3.0 + altitudeMeters_ / 110.0, 4.0, 14.0);
    } else {
        targetVerticalVelocityMps_ = -clamp(0.7 + altitudeMeters_ / 180.0, 1.0, 4.2);
    }
}

void CompatibilityScenario::updateLoadAndAlarms(AgcCpu& cpu, DskyIo& dsky) {
    const double phaseLoad = dsky.phaseProgram() == "63" ? 0.72 : (dsky.phaseProgram() == "64" ? 0.81 : 0.56);
    const double displayLoad = dsky.displayMode() == DisplayMode::LANDING_MONITOR ? 0.16 : 0.05;
    const double radarLoad = (altitudeMeters_ >= 450.0 && altitudeMeters_ <= 1800.0 && dsky.phaseProgram() != "66") ? 0.17 : 0.0;
    double scriptedPressure = 0.0;
    if ((missionTimeSeconds_ >= 32.0 && missionTimeSeconds_ <= 37.0) ||
        (missionTimeSeconds_ >= 52.0 && missionTimeSeconds_ <= 57.0) ||
        (missionTimeSeconds_ >= 102.0 && missionTimeSeconds_ <= 108.0) ||
        (missionTimeSeconds_ >= 114.0 && missionTimeSeconds_ <= 120.0)) {
        scriptedPressure = 0.16;
    } else if (missionTimeSeconds_ >= 88.0 && missionTimeSeconds_ <= 93.0) {
        scriptedPressure = 0.22;
    }
    loadRatio_ = phaseLoad + displayLoad + radarLoad + scriptedPressure + std::abs(throttleTrim_) * 0.05;
}

void CompatibilityScenario::updatePhaseAndOutcome(AgcCpu& cpu, DskyIo& dsky) {
    const std::string displayedProgram = dsky.program();
    if (dsky.hasApolloDisplayOutput() && displayedProgram != dsky.phaseProgram()) {
        if (displayedProgram == "64") {
            cpu.requestMajorMode(64);
            dsky.syncProgramFromCpu(cpu);
            dsky.setPhase("Approach", displayedProgram);
            lastEvent_ = "Apollo DSKY entered P64";
            dsky.setStatus(lastEvent_);
            return;
        }
        if (displayedProgram == "66") {
            cpu.requestMajorMode(66);
            dsky.syncProgramFromCpu(cpu);
            dsky.setPhase("Landing", displayedProgram);
            lastEvent_ = "Apollo DSKY entered P66";
            dsky.setStatus(lastEvent_);
            return;
        }
    }

    if (fuelKg_ <= 0.0 && altitudeMeters_ > 0.0) {
        missionResult_ = "Fuel Exhausted";
        missionResultSummary_ = "Usable descent propellant was exhausted before a controlled landing.";
        dsky.setPhase("Failure", "68");
        cpu.requestMajorMode(68);
        dsky.syncProgramFromCpu(cpu);
        lastEvent_ = "Descent propellant depleted";
        dsky.setStatus(lastEvent_);
        return;
    }

    if (altitudeMeters_ <= 0.0) {
        const double verticalImpact = std::abs(verticalVelocityMps_);
        const double horizontalImpact = std::abs(horizontalVelocityMps_);
        if (verticalImpact <= 3.0 && horizontalImpact <= 1.2) {
            missionResult_ = "Safe Landing";
            missionResultSummary_ = "Touchdown rates stayed within the slice's conservative landing envelope.";
            dsky.setPhase("Landed", "68");
        } else if (verticalImpact <= 6.0 && horizontalImpact <= 2.5) {
            missionResult_ = "Hard Landing";
            missionResultSummary_ = "Touchdown was survivable in this simulator, but outside nominal comfort.";
            dsky.setPhase("Landed", "68");
        } else {
            missionResult_ = "Crash";
            missionResultSummary_ = "Impact energy exceeded the simulator's safe touchdown thresholds.";
            dsky.setPhase("Failure", "68");
        }
        cpu.requestMajorMode(68);
        dsky.syncProgramFromCpu(cpu);
        lastEvent_ = "Touchdown";
        dsky.setStatus(lastEvent_);
        return;
    }

    if (dsky.phaseProgram() == "63" && altitudeMeters_ <= 850.0) {
        cpu.requestMajorMode(64);
        dsky.syncProgramFromCpu(cpu);
        dsky.setPhase("Approach", std::to_string(cpu.state().currentMajorMode));
        AgcProgramImage nextPhase;
        nextPhase.loaded = true;
        nextPhase.firstRopeBank = cpu.state().programCounterBank;
        nextPhase.firstRopeOffset = cpu.state().programCounterOffset;
        cpu.resetForScenario(nextPhase, "64");
        lastEvent_ = dsky.alarmNeedsAcknowledgement() ? "P64 entered under overload pressure" : "P64 approach entered";
        dsky.setStatus(lastEvent_);
    } else if (dsky.phaseProgram() == "64" && altitudeMeters_ <= 120.0) {
        cpu.requestMajorMode(66);
        dsky.syncProgramFromCpu(cpu);
        dsky.setPhase("Landing", std::to_string(cpu.state().currentMajorMode));
        AgcProgramImage nextPhase;
        nextPhase.loaded = true;
        nextPhase.firstRopeBank = cpu.state().programCounterBank;
        nextPhase.firstRopeOffset = cpu.state().programCounterOffset;
        cpu.resetForScenario(nextPhase, "66");
        lastEvent_ = "P66 landing phase entered";
        dsky.setStatus(lastEvent_);
    }
}

}  // namespace apollo
