#ifndef FixPointFunction
#define FixPointFunction
#include <cstdint>
#include <vector>
#include <complex>



std::int16_t add (std::int16_t a, std::int16_t b, int q=15);
std::int16_t subtract (std::int16_t a, std::int16_t b, int q=15);
std::int16_t multiply (std::int16_t a, std::int16_t b, int q=15);
std::int16_t divide (std::int16_t a, std::int16_t b, int q=15);
std::int16_t double2int(double floatingValue, int q=15);
double int2double(std::int16_t fixedPointValue, int q=15);

std::vector<std::complex<double>> FFT_FP(std::complex<double> dataSymbols[], int N, bool inverse,int q=15);

#endif