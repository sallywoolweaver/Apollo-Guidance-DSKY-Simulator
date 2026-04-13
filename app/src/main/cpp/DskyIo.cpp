#include "DskyIo.h"

#include <cctype>
#include <sstream>

#include "AgcCpu.h"
#include "AgcMemoryImage.h"

namespace apollo {

void DskyIo::initialize() {
    *this = DskyIo();
}

void DskyIo::resetForProgram(const std::string& initialProgram) {
    initialize();
    program_ = initialProgram;
    phaseProgram_ = initialProgram;
    statusLine_ = "P" + initialProgram + " initialized";
}

DskyEvent DskyIo::pressKey(const std::string& key) {
    if (key.size() == 1 && std::isdigit(key[0])) {
        if (entryField_ == EntryField::NONE) {
            setOpError("Select VERB or NOUN first");
            return {};
        }
        if (entryBuffer_.size() >= 2) {
            setOpError("Field complete");
            return {};
        }
        entryBuffer_.push_back(key[0]);
        compActyLight_ = true;
        return {};
    }

    if (key == "VERB") {
        entryField_ = EntryField::VERB;
        entryBuffer_.clear();
        statusLine_ = "Verb entry";
        return {};
    }
    if (key == "NOUN") {
        entryField_ = EntryField::NOUN;
        entryBuffer_.clear();
        statusLine_ = "Noun entry";
        return {};
    }
    if (key == "CLR") {
        if (!entryBuffer_.empty()) {
            entryBuffer_.pop_back();
        }
        statusLine_ = "Clear";
        return {};
    }
    if (key == "KEY REL") {
        keyRelLight_ = false;
        oprErrLight_ = false;
        statusLine_ = "Key release";
        return {DskyEventType::KEY_RELEASE};
    }
    if (key == "RSET") {
        progLight_ = false;
        oprErrLight_ = false;
        keyRelLight_ = false;
        alarmNeedsAcknowledgement_ = false;
        activeAlarmCode_.clear();
        activeAlarmTitle_.clear();
        statusLine_ = "Reset";
        return {DskyEventType::RESET};
    }
    if (key == "PRO") {
        statusLine_ = "Proceed";
        return {DskyEventType::ACKNOWLEDGE_ALARM};
    }
    if (key == "ENTR") {
        if (entryField_ == EntryField::VERB) {
            if (entryBuffer_.size() != 2) {
                setOpError("Verb requires 2 digits");
                return {};
            }
            verb_ = entryBuffer_;
            entryBuffer_.clear();
            entryField_ = EntryField::NONE;
            statusLine_ = "Verb loaded";
            return {};
        }
        if (entryField_ == EntryField::NOUN) {
            if (entryBuffer_.size() != 2) {
                setOpError("Noun requires 2 digits");
                return {};
            }
            noun_ = entryBuffer_;
            entryBuffer_.clear();
            entryField_ = EntryField::NONE;
            statusLine_ = "Noun loaded";
            return {};
        }
        return {DskyEventType::EXECUTE_PENDING_COMMAND};
    }

    return {};
}

DskyEvent DskyIo::executePendingCommand(AgcCpu& cpu) {
    if (verb_ == "16" && noun_ == "68") {
        setDisplayMode(DisplayMode::LANDING_MONITOR);
        setStatus("V16 N68 monitor");
        return {};
    }
    if (verb_ == "06" && (noun_ == "63" || noun_ == "64" || noun_ == "60")) {
        setDisplayMode(DisplayMode::PHASE_SUMMARY);
        setStatus("Phase summary");
        return {};
    }
    if (verb_ == "05" && noun_ == "09") {
        setDisplayMode(DisplayMode::LAST_ALARM);
        setStatus("Last alarm display");
        return {};
    }
    if (verb_ == "37" && noun_ == "63") {
        cpu.requestMajorMode(63);
        setProgram("63");
        setPhase("Braking", "63");
        setStatus("P63 requested");
        return {DskyEventType::SCENARIO_RESET_REQUESTED};
    }

    setOpError("Unsupported command");
    return {};
}

void DskyIo::syncProgramFromCpu(const AgcCpu& cpu) {
    const auto majorMode = std::to_string(cpu.state().currentMajorMode);
    program_ = majorMode;
    phaseProgram_ = majorMode;
}

void DskyIo::syncExecutionFromCpu(const AgcCpu& cpu, const AgcMemoryImage&) {
    const bool executingApolloWord = cpu.state().runState == CpuRunState::EXECUTING && cpu.state().lastFetchedWord != 0;
    compActyLight_ = executingApolloWord;
    if (alarmNeedsAcknowledgement_ || oprErrLight_) {
        return;
    }
    if (executingApolloWord) {
        if (!cpu.state().currentLabel.empty()) {
            statusLine_ = "EXEC " + cpu.state().currentLabel;
        } else {
            std::ostringstream builder;
            builder << "EXEC " << std::oct << cpu.state().programCounterBank << ":" << cpu.state().programCounterOffset;
            statusLine_ = builder.str();
        }
    } else {
        statusLine_ = "CPU idle";
    }
}

void DskyIo::setProgram(const std::string& program) {
    program_ = program;
}

void DskyIo::setRegisters(const std::string& r1, const std::string& r2, const std::string& r3) {
    register1_ = r1;
    register2_ = r2;
    register3_ = r3;
}

void DskyIo::setPhase(const std::string& phaseLabel, const std::string& phaseProgram) {
    phaseLabel_ = phaseLabel;
    phaseProgram_ = phaseProgram;
}

void DskyIo::setStatus(const std::string& status) {
    statusLine_ = status;
}

void DskyIo::setDisplayMode(DisplayMode displayMode) {
    displayMode_ = displayMode;
}

void DskyIo::setAlarm(
    const std::string& code,
    const std::string& title,
    bool needsAcknowledgement,
    int totalAlarms
) {
    activeAlarmCode_ = code;
    activeAlarmTitle_ = title;
    alarmNeedsAcknowledgement_ = needsAcknowledgement;
    totalAlarms_ = totalAlarms;
    progLight_ = needsAcknowledgement;
}

void DskyIo::clearAlarm() {
    activeAlarmCode_.clear();
    activeAlarmTitle_.clear();
    alarmNeedsAcknowledgement_ = false;
    progLight_ = false;
}

void DskyIo::setOpError(const std::string& reason) {
    oprErrLight_ = true;
    keyRelLight_ = true;
    statusLine_ = reason;
}

void DskyIo::setCompatibilityActivityLight(bool active) {
    compActyLight_ = active;
}

}  // namespace apollo
