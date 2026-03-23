#include <random>
#include <complex>

using namespace std;

std::complex<double> complexGaussian(double mean, double variance)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    double stddev = sqrt(variance / 2.0); // split between real & imag

    std::normal_distribution<double> dist(mean, stddev);

    return std::complex<double>(dist(gen), dist(gen));
}