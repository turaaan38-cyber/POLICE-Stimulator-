#include "police/core/Log.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
namespace police::core {
namespace { std::mutex log_mutex; }
void log(const LogLevel level, const std::string_view message) {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif
    const char* label = "INFO";
    if (level == LogLevel::Warning) label = "WARN";
    else if (level == LogLevel::Error) label = "ERROR";
    const std::lock_guard<std::mutex> lock(log_mutex);
    std::cerr << '[' << std::put_time(&local, "%H:%M:%S") << "] [" << label << "] " << message << '\n';
}
} // namespace police::core
