#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "element.hpp"

/**
 * @struct Context
 * @brief A structure holding all runtime data and state for the circuit analysis.
 *
 * This includes the circuit elements, the maximum node index, the MNA system's
 * equations, and state variables for the bisection algorithm.
 */
struct Context {
    uint8_t max_node = 0;
    uint8_t number_of_unknowns = 0;
    std::vector<Element> elements;
    std::vector<double> potentials;

    Element* variable_elem = nullptr;
    GivenValue given_value;
    double output_value = 0;

    std::unordered_map<uint16_t, uint8_t> v_source_currents_map;
    std::vector<std::vector<double>> equations;
};
