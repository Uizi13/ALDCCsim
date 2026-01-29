#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <vector>

#include "util.hpp"
#include "solver.hpp"
#include "bisect.hpp"
#include "constants.hpp"

void run(Context& ctx, bool do_bisection) {
    ctx.equations.clear();
    find_equations(ctx);
    solve(ctx);
    auto [pot, curr] = decode_and_normalize(ctx);
    ctx.potentials = pot;
    finalize(ctx, pot, curr);
    if (do_bisection) {
        update_bisection_output(ctx);
    }
}

void find_equations(Context& ctx) {
    insert_ground_node_equation(ctx.equations, ctx.number_of_unknowns);
    uint8_t source_count = 0;
    for (const auto& elem : ctx.elements) {
        if (elem.type != v_source)
            continue;
        ctx.v_source_currents_map[node_key(elem.nodes[0], elem.nodes[1])] = source_count;
        std::vector<double> eq;
        init_vector(eq, ctx.number_of_unknowns+1);
        eq[elem.nodes[0] - 1] = 1;
        eq[elem.nodes[1] - 1] = -1;
        eq[ctx.number_of_unknowns] = elem.value;
        ctx.equations.push_back(eq);
        ++source_count;
    }
    for (uint8_t i = 1; i < ctx.max_node; ++i) {
        std::vector<double> eq;
        init_vector(eq, ctx.number_of_unknowns+1);
        double conductance_sum = 0;
        auto neighbours = ctx.elements | std::views::filter(
            [i](const Element& e){ return e.nodes[0] == i || e.nodes[1] == i; });
        for (auto it = neighbours.begin(); it != neighbours.end(); ++it) {
            switch (it->type) {
                case v_source: {
                    const auto which_current = ctx.v_source_currents_map.at(node_key(it->nodes[0], it->nodes[1]));
                    eq[ctx.max_node + which_current] = it->sign_of_current(i);
                    break;
                }
                case c_source: {
                    eq[ctx.number_of_unknowns] += it->sign_of_current(i) * it->value;
                    break;
                }
                case resistor: {
                    const double conductance = 1 / it->value;
                    eq[it->other_node(i)-1] -= conductance;
                    conductance_sum += conductance;
                    break;
                }
            }
        }
        eq[i-1] = conductance_sum;
        ctx.equations.push_back(eq);
    }
}
void solve(Context& ctx) {
    for (size_t i = 0; i < ctx.equations.size(); ++i) {
        size_t pivot_col = 0;
        while (pivot_col < ctx.equations[i].size() - 1 && std::abs(ctx.equations[i][pivot_col]) < constants::ZERO_TRESHOLD)
            ++pivot_col;

        if (pivot_col == ctx.equations[i].size() - 1) {
            if (std::abs(ctx.equations[i][pivot_col]) > constants::ZERO_TRESHOLD) {
                throw std::runtime_error("Circuit is contradictory");
            }
            continue;
        }

        size_t pivot_row = i;
        double best = std::abs(ctx.equations[i][pivot_col]);
        for (size_t r = i + 1; r < ctx.equations.size(); ++r) {
            double v = std::abs(ctx.equations[r][pivot_col]);
            if (v > best) {
                best = v;
                pivot_row = r;
            }
        }

        if (pivot_row != i)
            std::swap(ctx.equations[i], ctx.equations[pivot_row]);

        auto& base_eq = ctx.equations[i];
        double pivot = base_eq[pivot_col];
        for (auto& x : base_eq)
            x /= pivot;

        for (size_t j = 0; j < ctx.equations.size(); ++j) {
            if (j == i)
                continue;
            auto& other_eq = ctx.equations[j];
            double factor = other_eq[pivot_col];
            for (size_t c = 0; c < other_eq.size(); ++c) {
                other_eq[c] -= factor * base_eq[c];
                if (std::abs(other_eq[c]) < constants::ZERO_TRESHOLD)
                    other_eq[c] = 0;
            }
        }
    }
}

std::pair<std::vector<double>, std::vector<double>> decode_and_normalize(Context& ctx) {
    std::vector<double> potentials;
    std::vector<double> currents;
    init_vector(potentials, ctx.max_node);
    init_vector(currents, ctx.number_of_unknowns - ctx.max_node);
    for (auto eq = ctx.equations.begin(); eq != ctx.equations.end();) {
        bool all_zero = true;
        for (uint8_t i = 0; i < eq->size()-1; ++i) {
            if (std::abs((*eq)[i]) > constants::ZERO_TRESHOLD) {
                all_zero = false;
                break;
            }
        }
        if (all_zero) {
            if (std::abs(eq->back()) > constants::ZERO_TRESHOLD)
                throw std::runtime_error("Circuit is contradictory");
            eq = ctx.equations.erase(eq);
            continue;
        }

        size_t j = 0;
        while (std::abs((*eq)[j]) < constants::ZERO_TRESHOLD) {
            j++;
        }

        const auto divisor = (*eq)[j];
        uint8_t values_found = 0;
        for (uint8_t i = 0; i < eq->size(); ++i) {
            (*eq)[i] = (*eq)[i] / divisor;
            if (std::abs((*eq)[i]) > constants::ZERO_TRESHOLD) {
                if ((*eq)[i] != eq->back())
                    values_found += 1;
            }
            else
                (*eq)[i] = 0;   // eliminate negative 0's
        }

        if (values_found > 1)
            throw std::runtime_error("Circuit is not fully determinate");

        if (j < ctx.max_node)
            potentials[j] = -(*eq)[ctx.number_of_unknowns];
        else
            currents[j-ctx.max_node] = -(*eq)[ctx.number_of_unknowns];
        ++eq;
    }
    return {potentials, currents};
}
void finalize(Context& ctx, const std::vector<double>& potentials, const std::vector<double>& currents) {
    for (auto& elem : ctx.elements) {
        std::string name = static_cast<char>(elem.type) + std::to_string(elem.nodes[0]) + std::to_string(elem.nodes[1]);
        switch (elem.type) {
            case v_source:
                elem.voltage = elem.value;
                elem.current = currents[
                    ctx.v_source_currents_map.at(node_key(elem.nodes[0], elem.nodes[1]))
                    ];
                break;
            case c_source:
                elem.voltage = potentials[elem.nodes[1] - 1] - potentials[elem.nodes[0] - 1];
                elem.current = elem.value;
                break;
            case resistor:
                elem.voltage = potentials[elem.nodes[1] - 1] - potentials[elem.nodes[0] - 1];
                elem.current = elem.voltage / elem.value;
                break;
        }
        elem.power = elem.voltage * elem.current;
    }
}