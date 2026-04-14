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

int normalizeRopeOffset(int bank, int offset) {
    if (bank == 02 && offset >= 04000) {
        return offset - 04000;
    }
    if (bank == 03 && offset >= 06000) {
        return offset - 06000;
    }
    if (bank >= 04 && offset >= 02000) {
        return offset - 02000;
    }
    return offset;
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
                const auto offset = normalizeRopeOffset(bank, std::stoi(parts[1], nullptr, 8));
                const auto word = static_cast<uint16_t>(std::stoi(parts[2], nullptr, 8));
                const size_t index = static_cast<size_t>((bank * 1024) + (offset & 01777));
                if (index < rope_.size()) {
                    if (metadata_.loadedRopeWords == 0) {
                        metadata_.firstRopeBank = bank;
                        metadata_.firstRopeOffset = offset & 01777;
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

bool AgcMemoryImage::loadRopeBinary(const std::vector<uint8_t>& binaryRope) {
    initialize();
    for (size_t i = 0; i < binaryRope.size() / 2 && i < rope_.size(); ++i) {
        uint16_t word = (static_cast<uint16_t>(binaryRope[i * 2]) << 8) | binaryRope[i * 2 + 1];
        rope_[i] = word;
        metadata_.loadedRopeWords++;
    }
    metadata_.containsApolloDerivedWords = true;
    metadata_.sourceKind = "apollo-binary-rope";
    metadata_.loaded = true;
    return true;
}

bool AgcMemoryImage::loadRopeMetadataFromBytes(const std::vector<uint8_t>& metadataBytes) {
    if (metadataBytes.empty()) {
        return true;
    }

    std::string text(metadataBytes.begin(), metadataBytes.end());
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
        if (key == "source_name" || key == "image_type") {
            metadata_.sourceTag = value;
        } else if (key == "source_kind") {
            metadata_.sourceKind = value;
        } else if (key == "first_rope_bank") {
            metadata_.firstRopeBank = std::stoi(value, nullptr, 8);
        } else if (key == "first_rope_offset") {
            metadata_.firstRopeOffset = std::stoi(value, nullptr, 8);
        }
    }
    return true;
}

bool AgcMemoryImage::loadRopeLabelsFromBytes(const std::vector<uint8_t>& labelBytes) {
    if (labelBytes.empty()) {
        return true;
    }

    std::string text(labelBytes.begin(), labelBytes.end());
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
        if (key != "rope_word") {
            continue;
        }
        const auto parts = split(value, ':');
        if (parts.size() < 4) {
            continue;
        }
        const auto bank = std::stoi(parts[0], nullptr, 8);
        const auto offset = normalizeRopeOffset(bank, std::stoi(parts[1], nullptr, 8));
        const size_t index = static_cast<size_t>((bank * 1024) + (offset & 01777));
        if (index < ropeLabels_.size()) {
            ropeLabels_[index] = parts[3];
        }
    }
    return true;
}

bool AgcMemoryImage::loadErasableBinary(const std::vector<uint8_t>& binaryErasable) {
    for (size_t i = 0; i < binaryErasable.size() / 2 && i < erasableInitial_.size(); ++i) {
        uint16_t word = (static_cast<uint16_t>(binaryErasable[i * 2]) << 8) | binaryErasable[i * 2 + 1];
        erasableInitial_[i] = word;
        metadata_.loadedErasableWords++;
    }
    resetErasable();
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
    const size_t index = static_cast<size_t>((bank * 1024) + (offset & 01777));
    if (index >= rope_.size()) {
        return 0;
    }
    return rope_[index];
}

std::string AgcMemoryImage::ropeLabel(int bank, int offset) const {
    const size_t index = static_cast<size_t>((bank * 1024) + (offset & 01777));
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

uint16_t AgcMemoryImage::erasableBankWord(int bank, int offset) const {
    const size_t index = static_cast<size_t>(((bank & 07) * 0400) + (offset & 0377));
    if (index >= erasable_.size()) {
        return 0;
    }
    return erasable_[index];
}

void AgcMemoryImage::writeErasableBankWord(int bank, int offset, uint16_t word) {
    const size_t index = static_cast<size_t>(((bank & 07) * 0400) + (offset & 0377));
    if (index >= erasable_.size()) {
        return;
    }
    erasable_[index] = word & 077777;
}

}  // namespace apollo
