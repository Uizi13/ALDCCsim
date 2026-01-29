#pragma once
#include "context.hpp"

/**
 * @brief Updates the output value for the bisection algorithm.
 *
 * Based on the `ValueType` specified in `ctx.given_value`, this sets
 * `ctx.output_value` to the voltage, current, or power of the monitored element.
 * This function is called after a full circuit run to check the result.
 *
 * @param ctx The Context object containing all circuit data.
 */
void update_bisection_output(Context& ctx);

/**
 * @brief Determines the sign of the derivative of the output value with respect to the variable element value.
 *
 * It checks how the `ctx.output_value` changes when the `ctx.variable_elem->value` is perturbed by a small
 * value (`constants::EPSILON`). This determines the monotonic relationship used in the bisection.
 *
 * @param ctx The Context object containing all circuit data.
 * @return int8_t 1 if the derivative is positive, -1 if negative.
 */
int8_t find_derivative_sign(Context& ctx);

/**
 * @brief Finds an initial search bound for the bisection method.
 *
 * The function iteratively doubles an initial value until the sign of the difference
 * between the current output and the desired output flips, indicating the solution is
 * within the bounded range.
 *
 * @param ctx The Context object containing all circuit data.
 * @param derivative_sign The sign of the output value's derivative (1 or -1).
 * @return double The calculated upper bound for the variable element's value.
 */
double find_bound(Context& ctx, int8_t derivative_sign);

/**
 * @brief Implements the bisection method to find the variable element's value that yields the desired output value.
 *
 * It iteratively narrows the search interval (`range[0]` and `range[1]`) based on the sign of the
 * difference between the current output and the desired output, taking into account the
 * derivative sign. The iteration runs for 'constants::BISECTION_PREC' steps.
 *
 * @param ctx The Context object containing all circuit data.
 */
void bisect(Context& ctx);
