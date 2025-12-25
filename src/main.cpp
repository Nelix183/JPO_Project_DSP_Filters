#include <iostream>
#include <vector>

#include "FirFilter.hpp"
#include "IirFilter.hpp"
#include "Signal.hpp"
#include "Window.hpp"

int main() {
    md::FirFilter<double, 15> fir;
    fir.setupLowPass(0.3);

    md::Window<double, 15> hamming;
    hamming.setupHamming();

    md::IirFilter<double, 3, 2> iir;
    std::array<double, 3> b = {0.02008337, 0.04016673, 0.02008337};
    std::array<double, 2> a = {-1.56101808, 0.64135154};

    iir.setCoefficients(b, a);

    md::Signal<double, 501> signal;
    signal.signalFromFile("test_data/test_sin.txt");
    md::Signal signal1(signal);
    md::Signal signal2(signal);
    std::cout << std::endl;
    std::cout << signal.energy() << std::endl;
    std::cout << signal.power() << std::endl;
    std::cout << signal.rms() << std::endl;
    fir.process(signal1.data(), signal1.size());
    iir.process(signal2);
    std::cout << std::endl;
    for (size_t i = 0; i < signal1.size(); i++) {
        std::cout << signal1.data()[i] << ", ";
    }
    signal1.signalToFile("test_data/out.txt");
    std::cout << std::endl;
    for (size_t i = 0; i < signal2.size(); i++) {
        std::cout << signal2.data()[i] << ", ";
    }

    md::Filter<double, 10>* filtr;
    int option;
    while (1) {
        std::cout << std::endl << "(1=fir 2=iir): ";
        std::cin >> option;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        if (option == 1) {
            filtr = new md::FirFilter<double, 10>;
            break;
        } else if (option == 2) {
            filtr = new md::IirFilter<double, 3, 7>;
            break;
        }
    }

    filtr->setFactors({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    filtr->process(signal);

    std::cout << std::endl;
    for (size_t i = 0; i < signal.size(); i++) {
        std::cout << signal.data()[i] << ", ";
    }

    delete filtr;
    return 0;
}