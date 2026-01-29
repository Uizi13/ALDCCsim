#include "util.hpp"

#include <cmath>

#include "element.hpp"

#include <cstdint>
#include <vector>

#include "constants.hpp"

uint8_t get_number_of_unknowns(const std::vector<Element>& elements, const uint8_t max_node) {
    uint8_t v_sources = 0;
    for (const auto& element : elements) {
        if (element.type == 'E')
            ++v_sources;
    }
    return v_sources + max_node;
}
void init_vector(std::vector<double>& vector, const uint8_t n) {
    for (uint8_t i = 0; i < n; ++i)
        vector.push_back(0);
}
void insert_ground_node_equation(std::vector<std::vector<double>>& equations, const uint8_t unknowns) {
    std::vector<double> eq;
    init_vector(eq, unknowns+1);
    eq[0] = 1;
    equations.push_back(eq);
}
uint16_t node_key(const uint8_t node_0, const uint8_t node_1) {
    return (static_cast<uint16_t>(node_0) << 8) | node_1;
}
bool current_source_only_node(const std::vector<Element>& elements, const uint8_t max_node) {
    for (uint8_t i = 1; i <= max_node; ++i) {
        bool found = false;
        for (const Element elem : elements) {
            if (elem.nodes[0] == i || elem.nodes[1] == i) {
                if (elem.type == v_source || elem.type == resistor)
                    found = true;
            }
        }
        if (!found)
            return true;
    }
    return false;
}
bool parallel_voltage_sources(const std::vector<Element>& elements) {
    for (const Element& elem : elements) {
        for (const Element& other_elem : elements) {
            if (&elem == &other_elem)
                continue;
            if (elem.type == v_source && other_elem.type == v_source)
                if ((elem.nodes[0] == other_elem.nodes[0] && elem.nodes[1] == other_elem.nodes[1])
                    || (elem.nodes[0] == other_elem.nodes[1] && elem.nodes[1] == other_elem.nodes[0]))
                    return true;
        }
    }
    return false;
}
double avg(double a, double b) {
    return (a+b)/2;
}
double round_to_prec(double value) {
    const double r = std::round(constants::prec_factor() * value) / constants::prec_factor();
    return r == 0? 0 : r;
}