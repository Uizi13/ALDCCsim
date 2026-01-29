#pragma once
#include <cstdint>
#include <vector>

#include "element.hpp"

/**
 * @brief Calculates the total number of unknowns in the MNA system.
 *
 * The number of unknowns is equal to the maximum node index (for node potentials)
 * plus the number of voltage sources (for voltage source currents).
 *
 * @param elements A constant reference to the vector of circuit elements.
 * @param max_node The highest node index in the circuit.
 * @return uint8_t The total number of unknowns.
 */
uint8_t get_number_of_unknowns(const std::vector<Element>& elements, uint8_t max_node);

/**
 * @brief Initializes a vector of doubles with a specified number of zeros.
 *
 * @param vector The vector to be initialized.
 * @param n The number of zeros to append.
 */
void init_vector(std::vector<double>& vector, uint8_t n);

/**
 * @brief Inserts the equation for the ground node (Node 1) into the system.
 *
 * @param equations The vector of equation rows (augmented matrix).
 * @param unknowns The total number of unknowns in the system.
 */
void insert_ground_node_equation(std::vector<std::vector<double>>& equations, uint8_t unknowns);

/**
 * @brief Creates a unique 16-bit key for a node pair, independent of the order.
 *
 * The key is constructed by shifting the greater node to the upper 8 bits
 * and the smaller node to the lower 8 bits.
 *
 * @param node_0 The first node index.
 * @param node_1 The second node index.
 * @return uint16_t The unique node key.
 */
uint16_t node_key(uint8_t node_0, uint8_t node_1);

/**
 * @brief Checks if there is a node connected only to current sources.
 *
 * This condition makes the node's voltage indeterminate, as only KCL equations
 * (which govern current) apply, not Ohm's law (which relates voltage and current).
 *
 * @param elements A constant reference to the vector of circuit elements.
 * @param max_node The highest node index.
 * @return bool True if such a node exists, false otherwise.
 */
bool current_source_only_node(const std::vector<Element>& elements, uint8_t max_node);

/**
 * @brief Checks for voltage sources connected in parallel.
 *
 * Parallel voltage sources (connected to the same two nodes) lead to an
 * indeterminate current in the branches, making the system unsolvable.
 *
 * @param elements A constant reference to the vector of circuit elements.
 * @return bool True if parallel voltage sources are found, false otherwise.
 */
bool parallel_voltage_sources(const std::vector<Element>& elements);

/**
 * @brief Calculates the average (arithmetic mean) of two double-precision numbers.
 *
 * @param a The first number.
 * @param b The second number.
 * @return double The average of a and b.
 */
double avg(double a, double b);

/**
 * @brief Calculates the base raised to the power of the exponent.
 *
 * This is a `constexpr` implementation of the power function for use
 * in compile-time calculations.
 *
 * @param base The number to be raised to a power.
 * @param exp The integer exponent.
 * @return double The result of base to the power of exp.
 */
constexpr double power(double base, int exp) {
    double result = 1.0;
    while (exp-- > 0) result *= base;
    return result;
}

/**
 * @brief Rounds a double-precision value to the specified number of significant digits.
 *
 * Uses `constants::prec_factor()` to scale the value for rounding.
 * Special handling to ensure negative zero is not returned.
 *
 * @param value The value to be rounded.
 * @return double The rounded value.
 */
double round_to_prec(double value);