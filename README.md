This is an implemntation of End to End setup of OFDM 
The function plot the symbol error rate over the various values of SNR 

The OFDM function block is as follows:
At Transmitter
1. Data symbol generation
2. Perform IFFT
3. Add CP

At channel:
1.Has N channel taps 

At Recevier:
1. Channel noise is added
2. Remove CP
3. Perform FFT
4. Hard Decoding and demodulation

![alt text](https://github.com/rpk226/DSP/blob/06a8376c0e91350565eac360340575ea53fa4d1c/Functional%20Block%20of%20OFDM%20TX%20and%20RX.png)
## Components

### `fft.cpp`
- Implements the **Fast Fourier Transform (FFT)** for complex inputs.
- Automatically switches to the **Discrete Fourier Transform (DFT)** when the number of subcarriers `N` is not a power of 2.

### `complexGaussian.cpp`
- Generates **complex Gaussian random variables**.
- Supports configurable **mean** and **variance**.
- Useful for noise modeling in communication system simulations.

### `Plots.ipynb`
- Jupyter Notebook used for visualization.
- Plots **Symbol Error Rate (SER)** versus **Signal-to-Noise Ratio (SNR)**.

### `Ser_vs_noise.csv`
- Stores the simulated values of:
  - **SNR**
  - **SER**
- Used for plotting and performance analysis.

## Build Instructions

Compile the C++ source files using `g++`:

```bash
g++ -o fft fft.cpp
g++ -o complexGaussian complexGaussian.cpp
