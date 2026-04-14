#include "DskyIo.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

#include "AgcCpu.h"
#include "AgcMemoryImage.h"

namespace apollo {

namespace {
constexpr uint16_t kChannelDsalmout = 011;
constexpr uint16_t kChannelMnkeyin = 015;
constexpr uint16_t kChannelProceed = 032;
constexpr uint16_t kProceedMask = 020000;

uint16_t dskyKeycodeForKey(const std::string& key) {
    if (key == "VERB") return 17;
    if (key == "RSET") return 18;
    if (key == "KEY REL") return 25;
    if (key == "+") return 26;
    if (key == "-") return 27;
    if (key == "ENTR") return 28;
    if (key == "CLR") return 30;
    if (key == "NOUN") return 31;
    if (key.size() == 1 && std::isdigit(key[0])) {
        const int digit = key[0] - '0';
        return digit == 0 ? 16 : static_cast<uint16_t>(digit);
    }
    return 0;
}

std::string signedWholeValue(double value) {
    int rounded = static_cast<int>(std::round(value));
    rounded = std::max(-9999, std::min(99999, rounded));
    if (rounded >= 0) {
        return "+" + std::to_string(100000 + rounded).substr(1);
    }
    return "-" + std::to_string(10000 + std::abs(rounded)).substr(1);
}

char decodeRelayDigit(uint16_t code) {
    switch (code & 037) {
        case 0:
            return ' ';
        case 3:
            return '1';
        case 15:
            return '4';
        case 19:
            return '7';
        case 21:
            return '0';
        case 25:
            return '2';
        case 27:
            return '3';
        case 28:
            return '6';
        case 29:
            return '8';
        case 30:
            return '5';
        case 31:
            return '9';
        default:
            return '?';
    }
}

std::string decodeTwoDigits(uint16_t relayWord) {
    std::string digits(2, ' ');
    digits[0] = decodeRelayDigit(static_cast<uint16_t>((relayWord >> 5) & 037));
    digits[1] = decodeRelayDigit(static_cast<uint16_t>(relayWord & 037));
    return digits;
}

std::string compactDigits(const std::string& rawDigits) {
    std::string compact;
    for (const char digit : rawDigits) {
        if (digit != ' ' && digit != '?') {
            compact.push_back(digit);
        }
    }
    return compact;
}

std::string decodeSignedRegister(const AgcCpu& cpu, uint16_t plusRow, uint16_t minusRow, uint16_t bridgeRow) {
    std::string digits;
    const uint16_t bridgeWord = cpu.dskyRelayWord(bridgeRow);
    digits.push_back(decodeRelayDigit(bridgeWord & 037));
    digits += decodeTwoDigits(cpu.dskyRelayWord(plusRow));
    digits += decodeTwoDigits(cpu.dskyRelayWord(minusRow));

    char sign = ' ';
    if ((cpu.dskyRelayWord(plusRow) & 0400) != 0) {
        sign = '+';
    } else if ((cpu.dskyRelayWord(minusRow) & 0400) != 0) {
        sign = '-';
    }
    return std::string(1, sign) + digits;
}
}

void DskyIo::initialize() {
    *this = DskyIo();
}

void DskyIo::resetForProgram(const std::string& initialProgram) {
    initialize();
    program_ = initialProgram;
    phaseProgram_ = initialProgram;
    statusLine_ = "P" + initialProgram + " initialized";
    refreshRegisters();
}

DskyEvent DskyIo::pressKey(const std::string& key, AgcCpu& cpu) {
    if (key == "PRO") {
        cpu.setInputChannelBits(kChannelProceed, kProceedMask, false);
        pendingProceedRelease_ = true;
        if (!hasApolloDisplayOutput_) {
            statusLine_ = "Proceed";
            return {DskyEventType::ACKNOWLEDGE_ALARM};
        }
        return {};
    }

    const uint16_t keycode = dskyKeycodeForKey(key);
    if (keycode != 0) {
        cpu.setInputChannel(kChannelMnkeyin, keycode);
    }

    if (hasApolloDisplayOutput_) {
        if (key == "KEY REL") {
            return {DskyEventType::KEY_RELEASE};
        }
        if (key == "RSET") {
            return {DskyEventType::RESET};
        }
        return {};
    }

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
        if (!hasApolloDisplayOutput_) {
            setDisplayMode(DisplayMode::LANDING_MONITOR);
            setStatus("V16 N68 monitor");
        }
        return {};
    }
    if (verb_ == "06" && (noun_ == "63" || noun_ == "64" || noun_ == "60")) {
        if (!hasApolloDisplayOutput_) {
            setDisplayMode(DisplayMode::PHASE_SUMMARY);
            setStatus("Phase summary");
        }
        return {};
    }
    if (verb_ == "05" && noun_ == "09") {
        if (!hasApolloDisplayOutput_) {
            setDisplayMode(DisplayMode::LAST_ALARM);
            setStatus("Last alarm display");
        }
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

void DskyIo::releaseMomentaryInputs(AgcCpu& cpu) {
    if (pendingProceedRelease_) {
        cpu.setInputChannelBits(kChannelProceed, kProceedMask, true);
        pendingProceedRelease_ = false;
    }
}

void DskyIo::syncProgramFromCpu(const AgcCpu& cpu) {
    const auto majorMode = std::to_string(cpu.state().currentMajorMode);
    program_ = majorMode;
    phaseProgram_ = majorMode;
    refreshRegisters();
}

void DskyIo::syncExecutionFromCpu(const AgcCpu& cpu, const AgcMemoryImage&) {
    syncApolloDisplayFromCpu(cpu);

    const bool executingApolloWord = cpu.state().runState == CpuRunState::EXECUTING && cpu.state().lastFetchedWord != 0;
    const bool unsupportedApolloOpcode = cpu.state().unsupportedOpcodeCount > 0;
    const uint16_t dsalmout = cpu.channelValue(kChannelDsalmout);
    const bool channelCompActy = (dsalmout & 000002) != 0;
    const bool channelKeyRel = (dsalmout & 000020) != 0;
    const bool channelOprErr = (dsalmout & 000100) != 0;
    compActyLight_ = channelCompActy || executingApolloWord;
    keyRelLight_ = channelKeyRel;
    oprErrLight_ = channelOprErr || unsupportedApolloOpcode;
    if (alarmNeedsAcknowledgement_) {
        return;
    }
    if (unsupportedApolloOpcode) {
        if (!cpu.state().currentLabel.empty()) {
            statusLine_ = "UNSUPPORTED " + cpu.state().currentLabel;
        } else {
            statusLine_ = "UNSUPPORTED OPCODE";
        }
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

void DskyIo::setLandingTelemetry(
    double altitudeMeters,
    double verticalVelocityMps,
    double horizontalVelocityMps,
    double fuelKg
) {
    altitudeMeters_ = altitudeMeters;
    verticalVelocityMps_ = verticalVelocityMps;
    horizontalVelocityMps_ = horizontalVelocityMps;
    fuelKg_ = fuelKg;
    refreshRegisters();
}

void DskyIo::setRegisters(const std::string& r1, const std::string& r2, const std::string& r3) {
    register1_ = r1;
    register2_ = r2;
    register3_ = r3;
}

void DskyIo::setPhase(const std::string& phaseLabel, const std::string& phaseProgram) {
    phaseLabel_ = phaseLabel;
    phaseProgram_ = phaseProgram;
    refreshRegisters();
}

void DskyIo::setStatus(const std::string& status) {
    statusLine_ = status;
}

void DskyIo::setDisplayMode(DisplayMode displayMode) {
    displayMode_ = displayMode;
    refreshRegisters();
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
    refreshRegisters();
}

void DskyIo::clearAlarm() {
    activeAlarmCode_.clear();
    activeAlarmTitle_.clear();
    alarmNeedsAcknowledgement_ = false;
    progLight_ = false;
    refreshRegisters();
}

void DskyIo::setOpError(const std::string& reason) {
    oprErrLight_ = true;
    keyRelLight_ = true;
    statusLine_ = reason;
}

void DskyIo::setCompatibilityActivityLight(bool active) {
    compActyLight_ = active;
}

void DskyIo::syncApolloDisplayFromCpu(const AgcCpu& cpu) {
    const std::string apolloProgram = compactDigits(decodeTwoDigits(cpu.dskyRelayWord(11)));
    const std::string apolloVerb = compactDigits(decodeTwoDigits(cpu.dskyRelayWord(10)));
    const std::string apolloNoun = compactDigits(decodeTwoDigits(cpu.dskyRelayWord(9)));

    const std::string apolloRegister1 = decodeSignedRegister(cpu, 7, 6, 8);
    const std::string apolloRegister2 = decodeSignedRegister(cpu, 5, 4, 3);
    const std::string apolloRegister3 = decodeSignedRegister(cpu, 2, 1, 3);

    const bool hasApolloDigits =
        cpu.dskyRelayWord(11) != 0 || cpu.dskyRelayWord(10) != 0 || cpu.dskyRelayWord(9) != 0 ||
        cpu.dskyRelayWord(8) != 0 || cpu.dskyRelayWord(7) != 0 || cpu.dskyRelayWord(6) != 0 ||
        cpu.dskyRelayWord(5) != 0 || cpu.dskyRelayWord(4) != 0 || cpu.dskyRelayWord(3) != 0 ||
        cpu.dskyRelayWord(2) != 0 || cpu.dskyRelayWord(1) != 0;
    hasApolloDisplayOutput_ = hasApolloDigits;
    if (!hasApolloDigits) {
        return;
    }

    if (!apolloProgram.empty()) {
        program_ = apolloProgram;
    }
    if (!apolloVerb.empty()) {
        verb_ = apolloVerb;
    }
    if (!apolloNoun.empty()) {
        noun_ = apolloNoun;
    }

    if (apolloVerb == "05" && apolloNoun == "09") {
        displayMode_ = DisplayMode::LAST_ALARM;
    } else if (apolloVerb == "06") {
        displayMode_ = DisplayMode::PHASE_SUMMARY;
    } else if (apolloVerb == "16" && apolloNoun == "68") {
        displayMode_ = DisplayMode::LANDING_MONITOR;
    }

    register1_ = apolloRegister1;
    register2_ = apolloRegister2;
    register3_ = apolloRegister3;
}

void DskyIo::refreshRegisters() {
    if (alarmNeedsAcknowledgement_ && !activeAlarmCode_.empty()) {
        setRegisters(
            "+12" + phaseProgram_,
            "+0" + activeAlarmCode_,
            "+" + std::to_string(10000 + totalAlarms_).substr(1)
        );
        return;
    }

    if (displayMode_ == DisplayMode::PHASE_SUMMARY) {
        setRegisters(
            "+0" + phaseProgram_ + "00",
            signedWhole(altitudeMeters_),
            signedWhole(std::abs(horizontalVelocityMps_))
        );
        return;
    }

    if (displayMode_ == DisplayMode::LAST_ALARM) {
        setRegisters(
            "+" + std::to_string(10000 + totalAlarms_).substr(1),
            activeAlarmCode_.empty() ? "+00000" : "+0" + activeAlarmCode_,
            "+0" + phaseProgram_ + "00"
        );
        return;
    }

    setRegisters(
        signedWhole(altitudeMeters_),
        signedTenth(verticalVelocityMps_),
        signedWhole(fuelKg_)
    );
}

std::string DskyIo::signedWhole(double value) {
    return signedWholeValue(value);
}

std::string DskyIo::signedTenth(double value) {
    return signedWholeValue(value * 10.0);
}

}  // namespace apollo
