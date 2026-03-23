#ifndef function
#define function

#include <vector>
#include <complex>

std::vector<std::complex<double>> FFT(
    std::complex<double> dataSymbols[], int N,
    bool inverse
);

std::complex<double> complexGaussian(double mean, double variance);

#endif