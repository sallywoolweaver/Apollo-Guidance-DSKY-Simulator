#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AgcTypes.h"

namespace apollo {

class AgcMemoryImage {
  public:
    void initialize();
    bool loadRopeFromBytes(const std::vector<uint8_t>& imageBytes);
    bool loadErasableFromBytes(const std::vector<uint8_t>& erasableBytes);
    void resetErasable();
    uint16_t ropeWord(int bank, int offset) const;
    std::string ropeLabel(int bank, int offset) const;
    uint16_t erasableWord(int address) const;
    void writeErasableWord(int address, uint16_t word);

    const AgcProgramImage& metadata() const { return metadata_; }
    bool loaded() const { return metadata_.loaded; }

  private:
    AgcProgramImage metadata_;
    std::vector<uint16_t> rope_;
    std::vector<std::string> ropeLabels_;
    std::vector<uint16_t> erasableInitial_;
    std::vector<uint16_t> erasable_;
};

}  // namespace apollo
