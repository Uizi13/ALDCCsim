#include "util.hpp"
#include "solver.hpp"
#include "file_io.hpp"
#include "bisect.hpp"
#include "context.hpp"
#include "cli.hpp"
#include "constants.hpp"

#include <fstream>


int main(int argc, char* argv[]) {
    auto [ifilename, ofilename, prec] = cli::parse(argc, argv);
    constants::PRECISION = prec;

    Context ctx;

    bool do_bisection = read(ctx, ifilename);
    ctx.number_of_unknowns = get_number_of_unknowns(ctx.elements, ctx.max_node);

    if (current_source_only_node(ctx.elements, ctx.max_node))
        throw std::runtime_error("Indeterminate voltage at one of the nodes (only current sources connected)");
    if (parallel_voltage_sources(ctx.elements))
        throw std::runtime_error("Indeterminate current in one of the branches (parallel voltage sources)");

    run(ctx, do_bisection);
    if (do_bisection) {
        bisect(ctx);
    }
    write(ctx, ofilename, do_bisection);
    return 0;
}
