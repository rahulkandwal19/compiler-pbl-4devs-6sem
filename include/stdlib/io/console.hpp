#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

auto FUNC_READ = []() {
    std::string input;
    std::getline(std::cin, input);
    return input;
};

auto FUNC_READLN = []() {
    std::string input;
    std::getline(std::cin, input);
    return input;
};
