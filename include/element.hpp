#pragma once
#include <array>
#include <cstdint>

/**
 * @brief Enumeration of supported circuit element types.
 *
 * The values are set to the single-character representation used in the input file.
 */
enum ElementType {
    v_source = 'E',
    c_source = 'J',
    resistor = 'R'
};

/**
 * @struct Element
 * @brief Represents a single circuit component and its solved parameters.
 */
struct Element {
    ElementType type;
    std::array<uint8_t, 2> nodes;
    double value;

    double voltage;
    double current;
    double power;

    /**
     * @brief Checks if the element's defined direction points away from a given node.
     *
     * For sources, this indicates the current is flowing from `nodes[0]` to `nodes[1]`.
     * @param node The node to check against.
     * @return bool True if the element's `nodes[0]` is the given node.
     */
    bool is_directed_outwards(const uint8_t node) const {
        return nodes[0] == node;
    }

    /**
     * @brief Gets the sign of the element's current with respect to a given node in KCL.
     *
     * Current is defined as positive when leaving the node (+1) and negative when entering the node (-1).
     * @param node The node of interest.
     * @return int8_t 1 if current is directed out of the node, -1 if directed into the node.
     */
    int8_t sign_of_current(const uint8_t node) const {
        return static_cast<int8_t>(2 * is_directed_outwards(node) - 1);    // 1 if current leaves, -1 when enters
    }

    /**
     * @brief Finds the node at the other end of the element.
     *
     * @param node The known node.
     * @return uint8_t The index of the connecting node.
     */
    uint8_t other_node(const uint8_t node) const {
        return nodes[0] == node? nodes[1] : nodes[0];
    }
};

/**
 * @brief Enumeration of the measurable values that can be the target for the bisection algorithm.
 *
 * The values are set to the single-character representation used in the input file.
 */
enum class ValueType {
    voltage = 'U',
    current = 'I',
    power = 'P'
};

/**
 * @struct GivenValue
 * @brief Holds information about the desired circuit output for the bisection method.
 */
struct GivenValue {
    Element* elem;
    ValueType v_type;
    double desired_value;
};
