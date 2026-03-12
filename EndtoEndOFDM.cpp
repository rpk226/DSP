#include<iostream>
#include "fft.h"

using namespace std;

int main()
{
    int N = 8; // Number of subcarriers
    int M = 4; // Modulation order (e.g., QPSK)
    
    // Generate random data symbols
    int dataSymbols[N];
    for(int i = 0; i < N; i++) {
        dataSymbols[i] = rand() % M; // Random symbol from 0 to M-1
    }
    for(int i = 0; i < N; i++) {
        cout << "Data Symbol " << i << ": " << dataSymbols[i] << endl;
    }
    

   // Perform IFFT to generate OFDM symbol
   complex<double> ofdmSymbol[N];
   for(int k = 0; k < N; k++) {
        ofdmSymbol[k] = 0;
        for(int n = 0; n < N; n++) {
            ofdmSymbol[k] += static_cast<double>(dataSymbols[n]) *
                 exp(complex<double>(0, -2.0 * M_PI * k * n / N));

        }
        ofdmSymbol[k] /= sqrt(N); // Normalize the OFDM symbol
    }
     

    // Output the generated OFDM symbol


    // Add cyclic prefix (CP) to the OFDM symbol
    int cpLength = 4; // Length of cyclic prefix
    complex<double> ofdmSymbolWithCP[N + cpLength];
    for(int i = 0; i < N; i++) {
        ofdmSymbolWithCP[i + cpLength] = ofdmSymbol[i];
    }
    for(int i = 0; i < cpLength; i++) {
        ofdmSymbolWithCP[i] = ofdmSymbol[N - cpLength + i];
    }


    // DAC and upconversion to RF for transmission are omitted for simplicity

    // At the receiver: downconversion, ADC, are ommitted for simplicity

    //consider L channel taps  
    int L = 3; // Number of channel taps
    complex<double> channel[L];
    // h[n]= \detlta(n) + 0.5\delta(n-1) + 0.25\delta(n-2)
    channel[0] = 1;
    channel[1] = 0.5;
    channel[2] = 0.25;

    // Recevied signal after passing through the channel (convolution with the channel impulse response)
    // This follows the assumption that the lenght of received signal> number of channel taps
    complex<double> receivedSignal[N + cpLength+L-1];
    for(int i = 0; i < N + cpLength ; i++) {
        receivedSignal[i] = 0;
        for(int j = 0; j < L; j++) 
            if (i - j >= 0) {
                receivedSignal[i] += ofdmSymbolWithCP[i - j] * channel[j];
            }
        }
    

    /*cout << "Generated OFDM Symbol with CP:" << endl;
    for(int i = 0; i < N+cpLength-1; i++) {
        cout << i<<" :" << receivedSignal[i] << endl;
    }*/
    // Removing CP  
    complex<double> receivedSignalWithoutCP[N];

    for (int i = 0; i < N; i++) {
        receivedSignalWithoutCP[i] = receivedSignal[i + cpLength];
    }


    
    //Perform FFT to recover the transmitted symbols
    complex<double> recoveredSymbols[N];
    for(int n = 0; n < N; n++) {
        recoveredSymbols[n] = 0;
        for(int k = 0; k < N; k++) {
            recoveredSymbols[n] += receivedSignalWithoutCP[k] *
                exp(complex<double>(0, 2.0 * M_PI * k * n / N));
        }
        recoveredSymbols[n] /= sqrt(N); // Normalize the recovered symbols
    }
    
    // FFT of the channel response to equalize the channel effect
    complex<double> channelFrequencyResponse[N];
    for(int k = 0; k < N; k++) {
        channelFrequencyResponse[k] = 0;
        for(int n = 0; n < L; n++) {
            channelFrequencyResponse[k] += channel[n] *
                exp(complex<double>(0, 2.0 * M_PI * k * n / N));
        }
    }
    
    // Equalize the received symbols by dividing by the channel frequency response
    for(int n = 0; n < N; n++) {
        if (abs(channelFrequencyResponse[n]) > 1e-6) { // Avoid division by zero
            recoveredSymbols[n] /= channelFrequencyResponse[n];
        } else {
            recoveredSymbols[n] = 0; // If the channel response is too weak, set the recovered symbol to zero
        }
    }

    cout << "Generated OFDM Symbol with CP:" << endl;
    for(int i = 0; i < N; i++) {
        cout << i<<" :" << recoveredSymbols[i] << endl;
    }
    // demoduation to retrieve the original data symbols

     return 0;

}