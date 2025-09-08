#ifndef SRC_UTILS_CLOCK_H_
#define SRC_UTILS_CLOCK_H_

#include <time.h>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace Kaspersky {

///
/// \brief Класс-утилита для получения времени в виде строки
///
class Clock {
public:
    Clock() = delete;
    static std::string getTimeNow() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_buf, &now_c);
#else
        localtime_r(&now_c, &tm_buf);
#endif
        std::ostringstream timeSS;
        timeSS << std::put_time(&tm_buf, "%H:%M:%S");
        return timeSS.str();
    }
};

} // namespace Kaspersky
#endif // SRC_UTILS_CLOCK_H_
