#pragma once

#include <array>

#include "AgcTypes.h"

namespace apollo {

class AgcMemoryImage;

class AgcCpu {
  public:
    void initialize();
    void loadProgramContext(const AgcProgramImage& image, const std::string& requestedProgram);
    void resetForScenario(const AgcProgramImage& image, const std::string& requestedProgram);
    void requestMajorMode(int majorMode);
    void setInputChannel(uint16_t channel, uint16_t word);
    void setInputChannelBits(uint16_t channel, uint16_t mask, bool asserted);
    void step(double deltaSeconds, AgcMemoryImage& memoryImage);

    const AgcCpuState& state() const { return state_; }
    uint16_t channelValue(uint16_t channel) const;
    uint16_t dskyRelayWord(uint16_t row) const;

  private:
    uint16_t applyIndexToInstruction(uint16_t word);
    uint16_t readOperand12(uint16_t address12, const AgcMemoryImage& memoryImage) const;
    uint16_t readOperand10(uint16_t address10, const AgcMemoryImage& memoryImage) const;
    void writeOperand10(uint16_t address10, uint16_t word, AgcMemoryImage& memoryImage);
    uint16_t readChannel(uint16_t channel) const;
    void writeChannel(uint16_t channel, uint16_t word);
    void setProgramCounterFromAddress(uint16_t address12);
    void syncBankRegisters();
    uint16_t fixedBankForAddress(uint16_t address12) const;
    uint16_t readFixedWord(uint16_t address12, const AgcMemoryImage& memoryImage) const;
    void advanceProgramCounter();
    void executeFetchedWord(uint16_t word, AgcMemoryImage& memoryImage);

    AgcCpuState state_;
    bool branchTaken_ = false;
    std::array<uint16_t, 0200> ioChannels_{};
    std::array<uint16_t, 020> dskyRelayWords_{};
};

}  // namespace apollo
