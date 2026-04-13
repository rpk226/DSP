#include "FixPointFunction.h"


std::int16_t add (std::int16_t a, std::int16_t b, int q) {

    if (a > 0 && b > 0 && (INT16_MAX - a < b)) {
        // Handle positive overflow
        return INT16_MAX;
    } else if (a < 0 && b < 0 && (INT16_MIN - a > b)) {
        // Handle negative overflow
        return INT16_MIN;
    }
    return a + b;
}
std::int16_t subtract (std::int16_t a, std::int16_t b, int q) {
    if (a > 0 && b < 0 && (INT16_MAX - a < -b)) {
        // Handle positive overflow
        return INT16_MAX;
    } else if (a < 0 && b > 0 && (INT16_MIN - a > b)) {
        // Handle negative overflow
        return INT16_MIN;
    }
    return a - b;
}
std::int16_t multiply (std::int16_t a, std::int16_t b, int q) {
    std::int32_t temp = static_cast<std::int32_t>(a) * static_cast<std::int32_t>(b);
    return static_cast<std::int16_t>(temp >> q);
}
std::int16_t divide(std::int16_t a, std::int16_t b, int q) {
    std::int32_t temp = (static_cast<std::int32_t>(a) << q);
    return static_cast<std::int16_t>(temp / b);
}

std::int16_t double2int(double floatingValue, int q) {
    // Clip the value to the representable range
    /*if (floatingValue > 1.0) {
        floatingValue = 1.0;
    } else if (floatingValue < -1.0) {
        floatingValue = -1.0;
    }*/

    return floatingValue*(1<<q);
}

double int2double(std::int16_t fixedPointValue, int q) {
    return static_cast<double>(fixedPointValue) / (1 << q);
}
