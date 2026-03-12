// main.cpp
#include "math_operations.h"
#include "fft.h"
#include <iostream>
#include <vector>
#include <complex>
using namespace std;

int main()
{
    int a = 10, b = 5;
    vector<complex<double>> dataSymbols = {1.0, 2.0, 3.0, 4.0};
    int N = dataSymbols.size();

  
    cout << "Addition: " << add(a, b) << endl;
    cout << "Subtraction: " << subtract(a, b) << endl;

    vector<complex<double>> ofdmSymbolVec = FFT(dataSymbols);

    cout << "OFDM Symbol:" << endl;
    for(int i = 0; i < N; i++) {
        cout << i << ": " << ofdmSymbolVec[i] <<":"<< dataSymbols[i]<< endl;
    }   
        
    return 0;
}
