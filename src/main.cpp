// Digital Signal Processing Library - Example Usage
// This example demonstrates the use of FIR and IIR filters, signal processing, and windowing functions

#include <iostream>
#include <vector>

#include "FirFilter.hpp"
#include "IirFilter.hpp"
#include "Signal.hpp"
#include "Window.hpp"

int main() {
    // Create a FIR (Finite Impulse Response) filter with 15 coefficients
    md::FirFilter<double, 15> fir;
    // Configure as a low-pass filter with normalized cutoff frequency of 0.3
    fir.setupLowPass(0.3);

    // Create a Hamming window with 15 points
    md::Window<double, 15> hamming;
    // Setup the Hamming window coefficients
    hamming.setupHamming();

    // Create an IIR (Infinite Impulse Response) filter with 3 numerator and 2 denominator coefficients
    md::IirFilter<double, 3, 2> iir;
    // Define numerator coefficients (feedforward)
    std::array<double, 3> b = {0.02008337, 0.04016673, 0.02008337};
    // Define denominator coefficients (feedback)
    std::array<double, 2> a = {-1.56101808, 0.64135154};
    // Set the IIR filter coefficients
    iir.setCoefficients(b, a);

    // Create a signal with 501 samples
    md::Signal<double, 501> signal;
    // Load signal data from file
    signal.signalFromFile("test_data/test_sin.txt");

    // Create copies of the signal for different filter processing
    md::Signal signal1(signal);  // Copy for FIR filtering
    md::Signal signal2(signal);  // Copy for IIR filtering

    // Display signal statistics
    std::cout << std::endl;
    std::cout << "Energy: " << signal.energy() << std::endl;  // Calculate total energy
    std::cout << "Power: " << signal.power() << std::endl;    // Calculate average power
    std::cout << "RMS: " << signal.rms() << std::endl;        // Calculate RMS value

    // Process signals through different filters
    fir.process(signal1.data(), signal1.size());  // Apply FIR filter
    iir.process(signal2);                         // Apply IIR filter
    std::cout << std::endl;

    std::cout << "FIR output signal: " << std::endl;
    // Print FIR filtered signal
    for (size_t i = 0; i < signal1.size(); i++) {
        std::cout << signal1[i] << ", ";
    }

    // Save FIR filtered signal to file
    signal1.signalToFile("test_data/out.txt");
    std::cout << std::endl;

    std::cout << "IIR output signal: " << std::endl;
    // Print IIR filtered signal
    for (size_t i = 0; i < signal2.size(); i++) {
        std::cout << signal2[i] << ", ";
    }

    // Demonstrate polymorphism - user selects filter type at runtime
    md::Filter<double, 10>* filtr;
    int option;

    // Interactive filter selection
    while (1) {
        std::cout << std::endl << "Pick type of filter(1=fir 2=iir): ";
        std::cin >> option;
        std::cin.clear();
        std::cin.ignore(10000, '\n');

        if (option == 1) {
            // Create FIR filter instance
            filtr = new md::FirFilter<double, 10>;
            break;
        } else if (option == 2) {
            // Create IIR filter instance
            filtr = new md::IirFilter<double, 3, 7>;
            break;
        }
    }

    // Set filter coefficients/factors (can be generated in MATLAB for better performance)
    filtr->setFactors({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    // Process the original signal using the selected filter
    filtr->process(signal);

    // Output the processed signal
    std::cout << std::endl;
    std::cout << "Picked filter output signal: " << std::endl;
    for (size_t i = 0; i < signal.size(); i++) {
        std::cout << signal[i] << ", ";
    }

    // Clean up dynamically allocated memory
    delete filtr;
    return 0;
}