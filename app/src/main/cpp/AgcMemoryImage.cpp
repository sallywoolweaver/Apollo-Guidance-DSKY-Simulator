#include "AgcMemoryImage.h"

#include <algorithm>
#include <sstream>
#include <vector>

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

std::vector<std::string> split(const std::string& text, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(text);
    std::string part;
    while (std::getline(stream, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}
}

void AgcMemoryImage::initialize() {
    metadata_ = AgcProgramImage();
    rope_.assign(36864, 0);
    ropeLabels_.assign(36864, "");
    erasableInitial_.assign(2048, 0);
    erasable_.assign(2048, 0);
}

bool AgcMemoryImage::loadRopeFromBytes(const std::vector<uint8_t>& imageBytes) {
    initialize();
    std::string text(imageBytes.begin(), imageBytes.end());
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
        if (key == "image_type") {
            metadata_.sourceTag = value;
        } else if (key == "source_name") {
            metadata_.sourceTag = value;
        } else if (key == "source_kind") {
            metadata_.sourceKind = value;
        } else if (key == "rope_word") {
            const auto parts = split(value, ':');
            if (parts.size() >= 3) {
                const auto bank = std::stoi(parts[0], nullptr, 8);
                const auto offset = std::stoi(parts[1], nullptr, 8);
                const auto word = static_cast<uint16_t>(std::stoi(parts[2], nullptr, 8));
                const size_t index = static_cast<size_t>((bank * 1024) + (offset & 07777));
                if (index < rope_.size()) {
                    if (metadata_.loadedRopeWords == 0) {
                        metadata_.firstRopeBank = bank;
                        metadata_.firstRopeOffset = offset & 07777;
                    }
                    rope_[index] = word;
                    if (parts.size() >= 4) {
                        ropeLabels_[index] = parts[3];
                    }
                    metadata_.loadedRopeWords += 1;
                    metadata_.containsApolloDerivedWords = true;
                }
            }
        }
    }
    metadata_.loaded = true;
    return true;
}

bool AgcMemoryImage::loadErasableFromBytes(const std::vector<uint8_t>& erasableBytes) {
    std::string text(erasableBytes.begin(), erasableBytes.end());
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
        if (key == "erasable_word") {
            const auto parts = split(value, ':');
            if (parts.size() >= 2) {
                const auto address = std::stoi(parts[0], nullptr, 8) & 03777;
                const auto word = static_cast<uint16_t>(std::stoi(parts[1], nullptr, 8));
                if (static_cast<size_t>(address) < erasableInitial_.size()) {
                    erasableInitial_[static_cast<size_t>(address)] = word;
                    metadata_.loadedErasableWords += 1;
                }
            }
        }
    }
    resetErasable();
    return true;
}

void AgcMemoryImage::resetErasable() {
    erasable_ = erasableInitial_;
}

uint16_t AgcMemoryImage::ropeWord(int bank, int offset) const {
    const size_t index = static_cast<size_t>((bank * 1024) + (offset & 07777));
    if (index >= rope_.size()) {
        return 0;
    }
    return rope_[index];
}

std::string AgcMemoryImage::ropeLabel(int bank, int offset) const {
    const size_t index = static_cast<size_t>((bank * 1024) + (offset & 07777));
    if (index >= ropeLabels_.size()) {
        return "";
    }
    return ropeLabels_[index];
}

uint16_t AgcMemoryImage::erasableWord(int address) const {
    const size_t index = static_cast<size_t>(address & 03777);
    if (index >= erasable_.size()) {
        return 0;
    }
    return erasable_[index];
}

void AgcMemoryImage::writeErasableWord(int address, uint16_t word) {
    const size_t index = static_cast<size_t>(address & 03777);
    if (index >= erasable_.size()) {
        return;
    }
    erasable_[index] = word;
}

}  // namespace apollo
