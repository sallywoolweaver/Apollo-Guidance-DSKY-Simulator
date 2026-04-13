#pragma once

#include <string>
#include <vector>

namespace apollo {

struct ScenarioBootstrapData {
    bool loaded = false;
    std::string requestedInitialProgram = "63";
    double initialAltitudeMeters = 2400.0;
    double initialVerticalVelocityMps = -34.0;
    double initialHorizontalVelocityMps = 24.0;
    double initialFuelKg = 835.0;
    double initialThrottle = 0.62;
};

class ScenarioBootstrap {
  public:
    void initialize();
    bool loadFromBytes(const std::vector<uint8_t>& bootstrapBytes);
    const ScenarioBootstrapData& data() const { return data_; }

  private:
    ScenarioBootstrapData data_;
};

}  // namespace apollo
