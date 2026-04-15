#include "SnapshotBuilder.h"

namespace apollo {

CoreState SnapshotBuilder::build(
    const AgcCpu& cpu,
    const AgcMemoryImage& memoryImage,
    const DskyIo& dsky,
    const CompatibilityScenario& scenario
) {
    CoreState snapshot;
    // Emulator/peripheral-driven fields.
    snapshot.initialized = cpu.state().initialized;
    snapshot.programLoaded = memoryImage.loaded();
    snapshot.program = dsky.program();
    snapshot.verb = dsky.verb();
    snapshot.noun = dsky.noun();
    snapshot.phaseProgram = dsky.phaseProgram();

    // Compatibility-populated display payloads and status text.
    snapshot.register1 = dsky.register1();
    snapshot.register2 = dsky.register2();
    snapshot.register3 = dsky.register3();
    snapshot.phaseLabel = dsky.phaseLabel();
    snapshot.statusLine = dsky.statusLine();
    snapshot.activeAlarmCode = dsky.alarmCode();
    snapshot.activeAlarmTitle = dsky.alarmTitle();
    snapshot.totalAlarms = dsky.totalAlarms();
    snapshot.alarmNeedsAcknowledgement = dsky.alarmNeedsAcknowledgement();
    snapshot.progLight = dsky.progLight();
    snapshot.oprErrLight = dsky.oprErrLight();
    snapshot.compActyLight = dsky.compActyLight();
    snapshot.keyRelLight = dsky.keyRelLight();
    snapshot.currentLabel = cpu.state().currentLabel;
    snapshot.programCounterBank = cpu.state().programCounterBank;
    snapshot.programCounterOffset = cpu.state().programCounterOffset;
    snapshot.executionNote = cpu.state().executionNote;

    // Compatibility fallback flight and outcome state.
    snapshot.altitudeMeters = scenario.altitudeMeters();
    snapshot.verticalVelocityMps = scenario.verticalVelocityMps();
    snapshot.horizontalVelocityMps = scenario.horizontalVelocityMps();
    snapshot.fuelKg = scenario.fuelKg();
    snapshot.throttle = scenario.throttle();
    snapshot.targetVerticalVelocityMps = scenario.targetVerticalVelocityMps();
    snapshot.missionTimeSeconds = scenario.missionTimeSeconds();
    snapshot.loadRatio = scenario.loadRatio();
    snapshot.throttleTrim = scenario.throttleTrim();
    snapshot.missionResult = scenario.missionResult();
    snapshot.missionResultSummary = scenario.missionResultSummary();
    snapshot.lastEvent = scenario.lastEvent();
    return snapshot;
}

}  // namespace apollo
