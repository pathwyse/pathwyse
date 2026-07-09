#ifndef COLUMN_GENERATION_CPP_CONSOLE_H
#define COLUMN_GENERATION_CPP_CONSOLE_H

#include <iomanip>
#include <iostream>
#include <string>

// Console output helpers, matching PathWyse's own output style
static constexpr int CONSOLE_WIDTH = 44;

inline void printHeader(const std::string& title) {
    std::cout << std::string(CONSOLE_WIDTH, '-') << "\n[ " << title << " ]\n" << std::string(CONSOLE_WIDTH, '-') << "\n";
}

template<typename T>
inline void printKV(const std::string& key, const T& value) {
    std::cout << "  " << std::left << std::setw(24) << key << ": " << value << "\n";
}

#endif
