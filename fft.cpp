#include "function.h"
#include <complex>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

namespace {

bool isPowerOfTwo(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

vector<complex<double>> computeDFT(
    const complex<double> dataSymbols[], int N, bool inverse
)
{
    vector<complex<double>> output(N);
    double sign = inverse ? 1.0 : -1.0;

    for (int k = 0; k < N; ++k) {
        output[k] = 0;
        for (int n = 0; n < N; ++n) {
            output[k] += dataSymbols[n] *
                exp(complex<double>(0, sign * 2.0 * M_PI * k * n / N));
        }
        output[k] /= sqrt(N);
    }

    return output;
}

} // namespace

std::vector<std::complex<double>> FFT(
    std::complex<double> dataSymbols[], int N,
    bool inverse
)
{
    if (!isPowerOfTwo(N)) {
        return computeDFT(dataSymbols, N, inverse);
    }

    vector<complex<double>> output(dataSymbols, dataSymbols + N);

    int j = 0;
    for (int i = 1; i < N; ++i) {
        int bit = N >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) {
            swap(output[i], output[j]);
        }
    }

    double sign = inverse ? 1.0 : -1.0;
    for (int len = 2; len <= N; len <<= 1) {
        double angle = sign * 2.0 * M_PI / len;
        complex<double> wLen = exp(complex<double>(0, angle));

        for (int start = 0; start < N; start += len) {
            complex<double> w(1.0, 0.0);
            int halfLen = len / 2;

            for (int offset = 0; offset < halfLen; ++offset) {
                complex<double> even = output[start + offset];
                complex<double> odd = output[start + offset + halfLen] * w;

                output[start + offset] = even + odd;
                output[start + offset + halfLen] = even - odd;
                w *= wLen;
            }
        }
    }


    double scale=1/sqrt(N);
    /*if (inverse==false) {
        scale = 1.0; // No scaling for FFT
    }*/


    for (auto& value : output) {
        value *= scale;
    }

    return output;
}
