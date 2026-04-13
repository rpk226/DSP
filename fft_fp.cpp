#include "FixPointFunction.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

using namespace std;

namespace {

bool isPowerOfTwo(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}
std::vector<std::complex<double>> DFT_FP(std::complex<double> dataSymbols[], int N, bool inverse, int q)
{ 
    double sign = inverse ? 1.0 : -1.0;

    int16_t outputfpreal[N]; // Fixed-point representation of the real part of the output
    int16_t outputfpimg[N]; // Fixed-point representation of the imaginary part of

    vector<complex<double>> output(N);  
    vector<complex<double>> outputFP(N); // Output of the FFT in fixed-point representation
    double invSqrtN_double = 1.0 / sqrt((double)N);
    int16_t invSqrtN = double2int(invSqrtN_double, q);


    for (int k = 0; k < N; k++) {
        
        output[k] = 0; // Initialize the output for the k-th frequency bin
        outputfpreal[k] = 0; // Initialize the fixed-point real part for the k-th frequency bin
        outputfpimg[k] = 0; // Initialize the fixed-point imaginary part for the k-th frequency bin
        for (int n = 0; n < N; n++) {
        
            double cosval=cos(sign*2*M_PI*k*n/N); // Floating-point cosine value
            double sinval=sin(sign*2*M_PI*k*n/N); // Floating-point sine value

            // Convert to fixed-point representation with q=16
            int16_t dataFPReal = double2int(real(dataSymbols[n]), q);
            int16_t dataFPImag = double2int(imag(dataSymbols[n]), q);

            int16_t cosFP = double2int(cosval, q);
            int16_t sinFP = double2int(sinval, q);


            int16_t prod_real, prod_imag;

            prod_real = add(multiply(dataFPReal, cosFP, q),
                    -multiply(dataFPImag, sinFP, q), q);

            prod_imag = add(multiply(dataFPReal, sinFP, q),
                    multiply(dataFPImag, cosFP, q), q);

        
            output[k] += dataSymbols[n] * exp(complex<double>(0, sign* 2.0 * M_PI * k* n / N));
            outputfpreal[k] = add(outputfpreal[k], prod_real, q);
            outputfpimg[k] = add(outputfpimg[k], prod_imag, q);
        }
        int16_t norm_real = multiply((int16_t)outputfpreal[k], invSqrtN, q);
        int16_t norm_imag = multiply((int16_t)outputfpimg[k], invSqrtN, q);

        outputFP[k] = std::complex<double>(
            int2double(norm_real, q),
            int2double(norm_imag, q)
        );
    }
    return outputFP;
}
}

std::vector<std::complex<double>> FFT_FP(
    std::complex<double> dataSymbols[], int N,
    bool inverse, int q
)
{
    if (!isPowerOfTwo(N)) {
        return DFT_FP(dataSymbols, N, inverse,q);
    }

    vector<complex<double>> output(dataSymbols, dataSymbols + N);
    double invSqrtN_double = 1.0 / sqrt((double)N);
    int16_t invSqrtN = double2int(invSqrtN_double, q);
    

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
    int16_t outputfpreal[N]; // Fixed-point representation of the real part of the output
    int16_t outputfpimg[N]; // Fixed-point representation of the imaginary part of the output 
    vector<complex<double>> outputFP(N); // Output After Fixed-point FFT Conversion

    for (int i = 0; i < N; i++) {
        outputfpreal[i] = double2int(real(output[i]), q);
        outputfpimg[i] = double2int(imag(output[i]), q);
    }

    double sign = inverse ? 1.0 : -1.0;

    for (int len = 2; len <= N; len <<= 1) {
        double angle = sign * 2.0 * M_PI / len;
        double cosval=cos(angle);
        double sinval=sin(angle);
        int16_t cosFP = double2int(cosval, q);
        int16_t sinFP = double2int(sinval, q);

        //complex<double> wLen = exp(complex<double>(0, angle));


        for (int start = 0; start < N; start += len) {
          
            int16_t wRealFP = double2int(1.0, q); // Fixed-point representation of the real part of w
            int16_t wImagFP = double2int(0.0, q); // Fixed-point representation of the imaginary part of w

            int halfLen = len / 2;

            for (int offset = 0; offset < halfLen; ++offset) {
                int16_t evenreal = outputfpreal[start + offset];
                int16_t evenimag = outputfpimg[start + offset];

                int16_t oddreal = subtract(multiply(outputfpreal[start + offset + halfLen], wRealFP, q),
                    multiply(outputfpimg[start + offset + halfLen], wImagFP, q), q);

                int16_t oddimag = add(multiply(outputfpreal[start + offset + halfLen], wImagFP, q),
                    multiply(outputfpimg[start + offset + halfLen], wRealFP, q),q);    
   

                outputfpreal[start + offset] = add(evenreal, oddreal, q);
                outputfpimg[start + offset] = add(evenimag, oddimag, q);
                outputfpreal[start + offset + halfLen] = subtract(evenreal, oddreal, q);
                outputfpimg[start + offset + halfLen] = subtract(evenimag, oddimag, q);

                int16_t oldWReal = wRealFP;
                int16_t oldWImag = wImagFP;

                wRealFP = add(multiply(oldWReal, cosFP, q),-multiply(oldWImag, sinFP, q),q);
                wImagFP = add(multiply(oldWReal, sinFP, q),multiply(oldWImag, cosFP, q),q);
        
            }
        }

    }
    
    for (int k = 0; k < N; k++) {
        int16_t norm_real = multiply((int16_t)outputfpreal[k], invSqrtN, q);
        int16_t norm_imag = multiply((int16_t)outputfpimg[k], invSqrtN, q);

        outputFP[k] = std::complex<double>(
            int2double(norm_real, q),
            int2double(norm_imag, q)
        );
    }
   

    return outputFP; // Return the original data symbols for now, replace with outputFP when ready
}




/*vector<complex<double>> computeDFT(
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
    */ 