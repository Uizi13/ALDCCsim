#pragma once
#include "context.hpp"

/**
 * @brief Runs the main circuit analysis procedure.
 *
 * Finds all necessary nodal and source equations,
 * solves the resulting system of linear equations and decodes the results into
 * node potentials and source currents. If bisection is enabled, it updates the output value
 * for the bisection algorithm.
 *
 * @param ctx The Context object containing all circuit data.
 * @param do_bisection Boolean flag indicating whether to perform bisection updates.
 */
void run(Context& ctx, bool do_bisection);

/**
 * @brief Constructs the system of linear equations for the circuit using Modified Nodal Analysis (MNA).
 *
 * This function creates equations based on:
 * 1. Ground node (Node 1) potential is zero.
 * 2. Voltage sources (introducing a new unknown for every voltage source current).
 * 3. Kirchhoff's Current Law (KCL) for every non-ground node.
 *
 * @param ctx The Context object containing all circuit data.
 */
void find_equations(Context& ctx);

/**
 * @brief Solves the system of linear equations stored in the context using Gaussian elimination with partial pivoting.
 *
 * The equations are represented as an augmented matrix stored in `ctx.equations`.
 * After solving, the matrix is in reduced row echelon form.
 * Throws a std::runtime_error if contradictory equations are found.
 *
 * @param ctx The Context object containing all circuit data.
 */
void solve(Context& ctx);

/**
 * @brief Decodes the results from the reduced row echelon form of the equations.
 *
 * Extracts the solved node potentials and voltage source currents from the matrix.
 * It also checks for and handles rows representing identity and throws an error if
 * the circuit is contradictory or not fully determinate.
 *
 * @param ctx The Context object containing all circuit data.
 * @return std::pair<std::vector<double>, std::vector<double>> A pair of vectors:
 * the first contains node potentials, the second contains voltage source currents.
 */
std::pair<std::vector<double>, std::vector<double>> decode_and_normalize(Context& ctx);

/**
 * @brief Calculates and updates the voltage, current, and power for every circuit element.
 *
 * Uses the solved node potentials and voltage source currents to determine
 * the final operating parameters for all resistors, current sources, and voltage sources.
 *
 * @param ctx The Context object containing all circuit data.
 * @param potentials A vector of solved node potentials.
 * @param currents A vector of solved voltage source currents.
 */
void finalize(Context& ctx, const std::vector<double>& potentials, const std::vector<double>& currents);