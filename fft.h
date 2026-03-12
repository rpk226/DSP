#ifndef FFT_H
#define FFT_H

#include <vector>
#include <complex>

std::vector<std::complex<double>> FFT(
    int dataSymbols[], int N,
    bool inverse = false
);


#endif