#include <chrono>
#include <string_view>
#include "post.hpp"
#include "parser.hpp"
#include <string>
#include <sstream>
#pragma once
namespace utils {
   inline long generateDayId(void) {
    auto now = std::chrono::system_clock::now();
    time_t formatted = std::chrono::system_clock::to_time_t(now);
    
    struct tm * parts = std::localtime(&formatted);

    return (parts->tm_year + 1900) * 1000 + (parts->tm_mon + 1) * 100 + parts->tm_mday;
}

inline bool isEmpty(std::string_view str) {
        return str.empty() || str.find_first_not_of(" \t\r\n") == std::string::npos;
}

}