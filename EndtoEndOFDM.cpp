#include <iostream>
#include <vector>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include "function.h"

using namespace std;

double runSimulation(double noisePower)
{
    int N = 1024; // Number of subcarriers
    int M = 4; // Modulation order (e.g., QPSK)
    // Generate random data symbols
    complex<double> dataSymbols[N];
    for(int i = 0; i < N; i++) {
        dataSymbols[i] = rand() % M; // Random symbol from 0 to M-1
    }

    //Modulation of data symbols (e.g., QAM)
    complex<double> modulatedSymbols[N];

    for(int i = 0; i < N; i++) {
        switch((int)dataSymbols[i].real()) {
            case 0: modulatedSymbols[i] = complex<double>(1, 1); break;
            case 1: modulatedSymbols[i] = complex<double>(-1, 1); break;
            case 2: modulatedSymbols[i] = complex<double>(-1, -1); break;
            case 3: modulatedSymbols[i] = complex<double>(1, -1); break;
        }
        modulatedSymbols[i] /= sqrt(2.0); // normalization
    }

    /*for(int i = 0; i < N; i++) {
        cout << "Data /modulated Symbol " << i << ": " << dataSymbols[i] <<":" << modulatedSymbols[i] << endl;
    }*/

    vector<complex<double>> ofdmSymbol = FFT(modulatedSymbols, N,true);//IFFT

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

    for(int i = 0; i < N; i++) {
        channel[i]= channel[i]* complexGaussian(0, 1); // Add noise to the channel taps
    }

    // Recevied signal after passing through the channel (convolution with the channel impulse response)
    // This follows the assumption that the lenght of received signal> number of channel taps
    complex<double> receivedSignal[N + cpLength+L-1];
    
    for(int i = 0; i < N + cpLength ; i++) {
        receivedSignal[i] = 0;
        for(int j = 0; j < L; j++) 
            if (i - j >= 0) {
                receivedSignal[i] += ofdmSymbolWithCP[i - j] * channel[j];
            }
        receivedSignal[i] += complexGaussian(0, noisePower); // Add noise to the received signal
        }
    
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

    //detection and demodulation of the recovered symbols
    vector<int> detectedSymbols(N);

    for(int i = 0; i < N; i++) {

        int b1 = real(recoveredSymbols[i]) > 0 ? 0 : 1;
        int b2 = imag(recoveredSymbols[i]) > 0 ? 0 : 1;

        if (b1 == 0 && b2 == 0) detectedSymbols[i] = 0; // 00
        else if (b1 == 1 && b2 == 0) detectedSymbols[i] = 1; // 01
        else if (b1 == 1 && b2 == 1) detectedSymbols[i] = 2; // 11
        else if (b1 == 0 && b2 == 1) detectedSymbols[i] = 3; // 10
    }
    /*cout << "Generated OFDM Symbol with CP:" << endl;
    for(int i = 0; i < N; i++) {
        cout << i<<" :" << recoveredSymbols[i] <<":" << detectedSymbols[i] << endl;
    }

    */


    // Compare the detected symbols with the original data symbols to calculate the symbol error rate (SER)
    int errorCount = 0;
    for(int i = 0; i < N; i++) {
        if (detectedSymbols[i] != (int)dataSymbols[i].real()) {
            errorCount++;
        }
    }
    double symbolErrorRate = (double)errorCount / N;
    return symbolErrorRate;
}

int main()
{
    srand((unsigned int)time(NULL));

    vector<double> snrDbValues = {1.0, 2.0, 3.0, 5.0};
    int trialsPerSNR = 100;
    double signalPower = 1.0; // QPSK symbols are normalized to unit average power

    cout << "snr_db,average_ser" << endl;
    cout << fixed << setprecision(6);

    for (double snrDb : snrDbValues) {
        double snrLinear = pow(10.0, snrDb / 10.0);
        double noisePower = signalPower / snrLinear;
        double totalSER = 0.0;

        for (int trial = 0; trial < trialsPerSNR; ++trial) {
            totalSER += runSimulation(noisePower);
        }

        cout << snrDb << "," << (totalSER / trialsPerSNR) << endl;
    }

    return 0;

}
