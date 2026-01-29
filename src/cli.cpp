#include "cli.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
namespace cli {
    Input parse(int argc, char* argv[]) {

        if (argc == 1) {
            std::cout << "To run the program use following command line switches:\n"
                         "-i input file\n"
                         "-o output file\n"
                         "-p (optional) how many significant digits to print for output values (recommended max 8)" << std::endl;
        }

        Input parsed;
        Switch current = Switch::none;

        for (uint8_t i = 1; i < argc; ++i) {

            if (current == Switch::none) {
                if (argv[i][0] == '-') {
                    char flag = argv[i][1];
                    current = static_cast<Switch>(flag);
                    continue;
                }
                throw std::runtime_error("Expecting a switch");
            }
            switch (current) {
                case Switch::in:
                    parsed.ifname = argv[i];
                    break;
                case Switch::out:
                    parsed.ofname = argv[i];
                    break;
                case Switch::prec:
                    parsed.prec = static_cast<uint8_t>(std::stoi(argv[i]));
                    break;
                default:
                    break;
            }
            current = Switch::none;
        }
        if (parsed.ifname == "")
            throw std::runtime_error("Expected input file");
        if (parsed.ofname == "")
            throw std::runtime_error("Expected output file");
        return parsed;
    }
}
