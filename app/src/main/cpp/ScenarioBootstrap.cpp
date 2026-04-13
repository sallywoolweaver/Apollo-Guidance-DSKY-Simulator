#include "ScenarioBootstrap.h"

#include <sstream>

namespace apollo {

namespace {
std::string trim(const std::string& text) {
    const auto start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}
}

void ScenarioBootstrap::initialize() {
    data_ = ScenarioBootstrapData();
}

bool ScenarioBootstrap::loadFromBytes(const std::vector<uint8_t>& bootstrapBytes) {
    initialize();
    std::string text(bootstrapBytes.begin(), bootstrapBytes.end());
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line.starts_with("#")) {
            continue;
        }
        const auto delimiter = line.find('=');
        if (delimiter == std::string::npos) {
            continue;
        }
        const auto key = trim(line.substr(0, delimiter));
        const auto value = trim(line.substr(delimiter + 1));
        if (key == "requested_initial_program") {
            data_.requestedInitialProgram = value;
        } else if (key == "initial_altitude_m") {
            data_.initialAltitudeMeters = std::stod(value);
        } else if (key == "initial_vertical_velocity_mps") {
            data_.initialVerticalVelocityMps = std::stod(value);
        } else if (key == "initial_horizontal_velocity_mps") {
            data_.initialHorizontalVelocityMps = std::stod(value);
        } else if (key == "initial_fuel_kg") {
            data_.initialFuelKg = std::stod(value);
        } else if (key == "initial_throttle") {
            data_.initialThrottle = std::stod(value);
        }
    }
    data_.loaded = true;
    return true;
}

}  // namespace apollo
