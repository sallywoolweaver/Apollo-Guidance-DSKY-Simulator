#pragma once

#include "AgcCpu.h"
#include "AgcMemoryImage.h"
#include "CompatibilityScenario.h"
#include "DskyIo.h"

namespace apollo {

class SnapshotBuilder {
  public:
    static CoreState build(
        const AgcCpu& cpu,
        const AgcMemoryImage& memoryImage,
        const DskyIo& dsky,
        const CompatibilityScenario& scenario
    );
};

}  // namespace apollo
