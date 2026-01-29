#include "bisect.hpp"
#include "element.hpp"
#include "constants.hpp"
#include "solver.hpp"
#include "util.hpp"

void update_bisection_output(Context& ctx) {
    switch (ctx.given_value.v_type) {
        case ValueType::voltage:
            ctx.output_value = ctx.given_value.elem->voltage;
            break;
        case ValueType::current:
            ctx.output_value = ctx.given_value.elem->current;
            break;
        case ValueType::power:
            ctx.output_value = ctx.given_value.elem->power;
            break;
    }
}

int8_t find_derivative_sign(Context& ctx) {
    const double val_at_zero = ctx.output_value;
    ctx.variable_elem->value = constants::EPSILON;
    run(ctx, true);
    const double val_at_epsilon = ctx.output_value;
    int8_t derivative_sign = val_at_epsilon - val_at_zero > 0? 1 : -1;
    return derivative_sign;
}

double find_bound(Context& ctx, int8_t derivative_sign) {
    int8_t desired_value_direction = (ctx.given_value.desired_value - ctx.output_value > 0)? 1 : -1;
    double upper_bound = derivative_sign * desired_value_direction * constants::EPSILON;
    while ((ctx.output_value - ctx.given_value.desired_value) * desired_value_direction < 0) {
        ctx.variable_elem->value = upper_bound;
        run(ctx, true);
        upper_bound *= 2;
    }
    return upper_bound;
}

void bisect(Context& ctx) {
    int8_t derivative_sign = find_derivative_sign(ctx);
    const double bound = find_bound(ctx, derivative_sign);
    double range[2] = {bound/4, bound/2};
    double val = avg(range[0], range[1]);
    double delta_y;
    for (int i = 0; i < constants::BISECTION_PREC; ++i) {
        ctx.variable_elem->value = val;
        run(ctx, true);
        delta_y = ctx.given_value.desired_value - ctx.output_value;
        if (delta_y * derivative_sign * bound > 0)
            range[0] = val;
        else
            range[1] = val;
        val = avg(range[0], range[1]);
    }
}
