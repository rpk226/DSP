#include "fft.h"
#include <cmath>

using namespace std;

vector<complex<double>> FFT(int dataSymbols[], int N)
{
    vector<complex<double>> ofdmSymbol(N);

    for(int k = 0; k < N; k++) {
        ofdmSymbol[k] = 0;
        for(int n = 0; n < N; n++) {
            ofdmSymbol[k] += static_cast<double>(dataSymbols[n]) *
                exp(complex<double>(0, -2.0 * M_PI * k * n / N));
        }
        ofdmSymbol[k] /= sqrt(N);
    }

    return ofdmSymbol;
}