#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>

auto FUNC_SQRT = [](double x) -> double {
    return std::sqrt(x);
};

auto FUNC_ABS = [](int x) -> int {
    return std::abs(x);
};

auto FUNC_MIN = [](int a, int b) -> int {
    return (a < b) ? a : b;
};

auto FUNC_MAX = [](int a, int b) -> int {
    return (a > b) ? a : b;
};

auto FUNC_RAND = []() -> int {
    return std::rand();
};

auto FUNC_ROUND = [](double x) -> int {
    return (int)std::round(x);
};

auto FUNC_FLOOR = [](double x) -> int {
    return (int)std::floor(x);
};

auto FUNC_CEIL = [](double x) -> int {
    return (int)std::ceil(x);
};

auto FUNC_POW = [](double base, double exp) -> double {
    return std::pow(base, exp);
};

auto FUNC_LOG = [](double x) -> double {
    return std::log(x);
};

auto FUNC_LOG10 = [](double x) -> double {
    return std::log10(x);
};

auto FUNC_SIN = [](double x) -> double {
    return std::sin(x);
};

auto FUNC_COS = [](double x) -> double {
    return std::cos(x);
};

auto FUNC_TAN = [](double x) -> double {
    return std::tan(x);
};
