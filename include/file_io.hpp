#pragma once
#include <string>

#include "context.hpp"

/**
 * @brief Reads circuit element data from an input file.
 *
 * Parses each line for element type, node 1, node 2, and element value.
 * It also detects the presence of a variable element (missing value) and a
 * desired output value for the bisection method. Updates the `ctx.max_node`.
 * Throws a std::runtime_error on failure to read, too many variable elements,
 * or mismatched solving procedure (variable element vs. given value).
 *
 * @param ctx The Context object to store the parsed data.
 * @param filename The name of the input file.
 * @return bool True if a variable element and desired output were found, indicating bisection is needed.
 */
bool read(Context& ctx, const std::string& filename);

/**
 * @brief Writes the solved parameters (voltage, current, power) for a single element to the output file.
 *
 * The output is formatted into columns with alignment and rounding applied.
 *
 * @param file The output file stream.
 * @param elem The Element object to write.
 */
void write_element(std::ofstream& file, const Element& elem);

/**
 * @brief Writes the final results of the circuit analysis to the output file.
 *
 * Includes the element table (U, I, P) and the node potential table (V).
 * If bisection was performed, it also writes the final value of the variable element.
 * Applies rounding to the specified precision.
 *
 * @param ctx The Context object containing the elements and bisection data.
 * @param filename The name of the output file.
 * @param do_bisection Boolean flag indicating if bisection was performed.
 */
void write(const Context& ctx, const std::string& filename, bool do_bisection);
