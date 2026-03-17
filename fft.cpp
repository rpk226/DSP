#include "function.h"
#include <complex>
#include <vector>
#include <cmath>

using namespace std;

std::vector<std::complex<double>> FFT(
    std::complex<double> dataSymbols[], int N,
    bool inverse
)
{

    vector<complex<double>> ofdmSymbol(N);

    double sign = inverse ? 1.0 : -1.0;

    for (int k = 0; k < N; k++) {
        ofdmSymbol[k] = 0;
        for (int n = 0; n < N; n++) {
            ofdmSymbol[k] += dataSymbols[n] *
                exp(complex<double>(0, sign * 2.0 * M_PI * k * n / N));
        }
        ofdmSymbol[k] /= sqrt(N);
    }

    return ofdmSymbol;
}