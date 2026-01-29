#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>

#include "file_io.hpp"
#include "constants.hpp"

bool read(Context& ctx, const std::string& filename) {
    ctx.elements.reserve(16);
    std::ifstream file(filename);

    std::string line;
    bool empty_value = false;
    bool found_given_value = false;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Element* elem_ptr = &ctx.elements.emplace_back();
        std::stringstream str_stream(line);

        char type;
        uint16_t n1, n2;
        double value;

        if (!(str_stream >> type >> n1 >> n2)) {
            throw std::runtime_error("Failed to read element data");
        }

        uint8_t greater_node = std::max(n1, n2);
        if (greater_node > ctx.max_node)
            ctx.max_node = greater_node;

        elem_ptr->type = static_cast<ElementType>(type);
        elem_ptr->nodes[0] = static_cast<uint8_t>(n1);
        elem_ptr->nodes[1] = static_cast<uint8_t>(n2);

        if (!(str_stream >> value)) {
            str_stream.clear();
            str_stream.ignore();
            if (!empty_value) {
                empty_value = true;
                elem_ptr->value = 1e-10;
                ctx.variable_elem = elem_ptr;
            }
            else {
                throw std::runtime_error("Failed to read element value");
            }
        }
        else {
            elem_ptr->value = value;
        }

        char v_type;
        double desired_value;

        if (str_stream >> v_type >> desired_value) {
            if (!found_given_value) {
                found_given_value = true;
                ctx.given_value.elem = elem_ptr;
                ctx.given_value.v_type = static_cast<ValueType>(v_type);
                ctx.given_value.desired_value = desired_value;
            }
            else {
                throw std::runtime_error("Too many given values");
            }
        }
    }
    if (empty_value != found_given_value)
        throw std::runtime_error("No applicable solving procedure (amounts of given values and variable elements don't match)");
    return empty_value;
}
void write_element(std::ofstream& file, const Element& elem) {
    std::string name = static_cast<char>(elem.type) + std::to_string(elem.nodes[0]) + std::to_string(elem.nodes[1]);
    file << '\n'
        << "   " << name << "  " << " | " << std::right
        << std::setw(13) << round_to_prec(elem.voltage) << " | "
        << std::setw(13) << round_to_prec(elem.current)<< " | "
        << std::setw(13) << round_to_prec(elem.power)<< ' ';
}
void write(const Context& ctx, const std::string& filename, bool do_bisection) {
    std::ofstream file(filename);
    file << std::setprecision(constants::PRECISION);
    if (do_bisection)
        file << "Answer: x = " << round_to_prec(ctx.variable_elem->value) << "\n" << std::endl;

    file
        << " Element |      U[V]     |      I[A]     |      P[W]     " << std::endl
        << "---------+---------------+---------------+---------------";

    for (uint8_t i = 0; i < ctx.elements.size(); ++i) {
        write_element(file, ctx.elements[i]);
    }
    file << '\n' << '\n'
        << " Node |      V[V]     " << std::endl
        << "------+---------------";

    for (uint8_t i = 0; i < ctx.potentials.size(); ++i) {
        file << '\n'
            << "  " << i+1 << "  " << " | " << std::right
            << std::setw(13) << round_to_prec(ctx.potentials[i]) << ' ';
    }
}