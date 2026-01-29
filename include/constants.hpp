#pragma once
#include "util.hpp"

/**
 * @namespace constants
 * @brief Global constants and configuration variables for the circuit solver.
 */
namespace constants {
    inline uint32_t PRECISION = 4;
    inline double prec_factor() { return power(10, PRECISION); }
    constexpr double ZERO_TRESHOLD = 1e-12;
    constexpr double EPSILON = 2e-10;
    constexpr uint32_t BISECTION_PREC = 50;
}