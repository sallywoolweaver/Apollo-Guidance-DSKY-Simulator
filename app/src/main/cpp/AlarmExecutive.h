#pragma once

#include <cstdint>

namespace apollo {

class AgcCpu;
class DskyIo;

class AlarmExecutive {
  public:
    void initialize();
    void reset();
    void update(const AgcCpu& cpu, DskyIo& dsky);

  private:
    uint32_t lastUnsupportedOpcodeCount_ = 0;
    int totalAlarms_ = 0;
};

}  // namespace apollo
