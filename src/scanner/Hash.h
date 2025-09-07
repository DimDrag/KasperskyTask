#ifndef HASH_H
#define HASH_H

#include <string_view>
#include <charconv>
#include <sstream>
#include <cstdint>

struct Hash {
    uint64_t data[2];
    bool operator<(const Hash& other) const {
        if (data[0] < other.data[0])
            return true;
        if (data[0] > other.data[0])
            return false;
        if (data[1] < other.data[1])
            return true;
        return false;
    }
    bool operator==(const Hash& other) const {
        return data[0] == other.data[0] &&
               data[1] == other.data[1];
    }
    static Hash fromString(const std::string &str) {
        std::string_view strv(str.c_str());
        return fromStringView(strv);
    }
    static Hash fromStringView(const std::string_view &strv) {
        std::string_view part1 = strv.substr(0, 16);
        std::string_view part2 = strv.substr(16);
        Hash ret{};
        std::from_chars(part1.data(), part1.data() + part1.size(), ret.data[0], 16);
        std::from_chars(part2.data(), part2.data() + part2.size(), ret.data[1], 16);
        return ret;
    }
    std::string toString() const {
        std::stringstream ss;
        ss << std::hex << data[0] << data[1];
        return ss.str();
    }
};
#endif // HASH_H
