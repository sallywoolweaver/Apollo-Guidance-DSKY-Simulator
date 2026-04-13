#include "AlarmExecutive.h"

#include "AgcCpu.h"
#include "DskyIo.h"

namespace apollo {

void AlarmExecutive::initialize() {
    reset();
}

void AlarmExecutive::reset() {
    lastUnsupportedOpcodeCount_ = 0;
    totalAlarms_ = 0;
}

void AlarmExecutive::update(const AgcCpu& cpu, DskyIo& dsky) {
    const auto unsupportedCount = cpu.state().unsupportedOpcodeCount;
    if (unsupportedCount <= lastUnsupportedOpcodeCount_) {
        return;
    }
    if (dsky.alarmNeedsAcknowledgement()) {
        return;
    }

    const auto delta = unsupportedCount - lastUnsupportedOpcodeCount_;
    lastUnsupportedOpcodeCount_ = unsupportedCount;
    totalAlarms_ += 1;

    const bool landingMonitor = dsky.displayMode() == DisplayMode::LANDING_MONITOR;
    const bool approachProgram = cpu.state().currentMajorMode == 64;
    const bool executiveOverflow = approachProgram && landingMonitor && delta >= 2;

    if (executiveOverflow) {
        dsky.setAlarm("1201", "Executive Overflow (CPU unsupported opcode pressure)", true, totalAlarms_);
        dsky.setStatus("1201 Executive Overflow");
        return;
    }

    dsky.setAlarm("1202", "Executive Overflow (CPU unsupported opcode pressure)", true, totalAlarms_);
    dsky.setStatus("1202 Executive Overflow");
}

}  // namespace apollo
