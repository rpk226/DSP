#include <iostream>
#include <vector>
#include <complex>
#include <cstdlib>
#include <ctime>
#include "function.h"

using namespace std;

int main()
{
    int N = 8; // Number of subcarriers
    int M = 4; // Modulation order (e.g., QPSK)
    srand((unsigned int)time(NULL));
    // Generate random data symbols
    complex<double> dataSymbols[N];
    for(int i = 0; i < N; i++) {
        dataSymbols[i] = rand() % M; // Random symbol from 0 to M-1
    }
    for(int i = 0; i < N; i++) {
        cout << "Data Symbol " << i << ": " << dataSymbols[i] << endl;
    }
    // Modulation of data symbols (e.g., QPSK)
    /*complex<double> modulatedSymbols[N];
    for(int i = 0; i < N; i++) {
      modulatedSymbols[i] = polar(1.0, 2.0 * M_PI * dataSymbols[i] / M); // Map symbols to constellation points
    }
    */

    vector<complex<double>> ofdmSymbol = FFT(dataSymbols, N,true);//IFFT

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
    complex<double> channel[N]={};
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
    vector<complex<double>> recoveredSymbols = FFT(receivedSignalWithoutCP, N,false);//FFT

    // FFT of the channel response to equalize the channel effect
    vector<complex<double>> channelFrequencyResponse = FFT(channel, N,false);//FFT
    for(int n = 0; n < N; n++) {
        channelFrequencyResponse[n] *= sqrt(N); // Scale the channel frequency response by sqrt(N) to match the scaling of the FFT
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