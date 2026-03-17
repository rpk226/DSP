#ifndef function
#define function

#include <vector>
#include <complex>

std::vector<std::complex<double>> FFT(
    std::complex<double> dataSymbols[], int N,
    bool inverse
);

#endif