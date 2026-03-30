#pragma once
#include <string>
#include <sstream>

auto FUNC_LENGTH = [](std::string str) -> int {
    return (int)str.length();
};

auto FUNC_SUBSTRING = [](std::string str, int start, int len) -> std::string {
    if (start < 0) start = 0;
    if (start >= (int)str.length()) return "";
    if (len < 0) len = str.length() - start;
    return str.substr(start, len);
};

auto FUNC_CONCAT = [](std::string a, std::string b) -> std::string {
    return a + b;
};

auto FUNC_TOSTRING = [](int n) -> std::string {
    return std::to_string(n);
};

auto FUNC_TOSTRING_FLOAT = [](double n) -> std::string {
    return std::to_string(n);
};

auto FUNC_TOINT = [](std::string str) -> int {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
};

auto FUNC_TOFLOAT = [](std::string str) -> double {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
};

auto FUNC_STRLOWER = [](std::string str) -> std::string {
    std::string result = str;
    for (char& c : result) {
        if (c >= 'A' && c <= 'Z') c = c + 32;
    }
    return result;
};

auto FUNC_STRUPPER = [](std::string str) -> std::string {
    std::string result = str;
    for (char& c : result) {
        if (c >= 'a' && c <= 'z') c = c - 32;
    }
    return result;
};

auto FUNC_TRIM = [](std::string str) -> std::string {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
};

auto FUNC_REPLACE = [](std::string str, std::string from, std::string to) -> std::string {
    size_t pos = str.find(from);
    if (pos == std::string::npos) return str;
    str.replace(pos, from.length(), to);
    return str;
};

auto FUNC_FIND = [](std::string str, std::string sub) -> int {
    size_t pos = str.find(sub);
    if (pos == std::string::npos) return -1;
    return (int)pos;
};
