#pragma once

#include "AgcTypes.h"

namespace apollo {

class AgcMemoryImage;

class AgcCpu {
  public:
    void initialize();
    void loadProgramContext(const AgcProgramImage& image, const std::string& requestedProgram);
    void resetForScenario(const AgcProgramImage& image, const std::string& requestedProgram);
    void requestMajorMode(int majorMode);
    void step(double deltaSeconds, AgcMemoryImage& memoryImage);

    const AgcCpuState& state() const { return state_; }

  private:
    void advanceProgramCounter();
    void executeFetchedWord(uint16_t word, AgcMemoryImage& memoryImage);

    AgcCpuState state_;
    bool branchTaken_ = false;
};

}  // namespace apollo
