#pragma once
#include <cstdint>
#include <string>
/**
 * @namespace cli
 * @brief Contains functions and structures for command-line argument parsing.
 */
namespace cli {

    /**
     * @brief Defines the valid command-line switches for the program.
     */
    enum class Switch {
        none = 0,
        in = 'i',
        out = 'o',
        prec = 'p'
    };

    /**
     * @brief Holds the parsed command-line input values.
     */
    struct Input {
        std::string ifname = "";
        std::string ofname = "";
        uint8_t prec = 4;
    };

    /**
     * @brief Parses the command line arguments to extract input file, output file, and precision.
     *
     * It checks for the '-i', '-o', and '-p' switches and reads the subsequent argument.
     * It also prints usage instructions if no arguments are provided.
     * Throws a std::runtime_error if a switch is expected but not found, or if
     * the required input/output filenames are missing.
     *
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @return Input A struct containing the parsed input filename, output filename, and precision.
     */
    Input parse(int argc, char* argv[]);
}