#include <iostream>
#include <vector>

#include "FirFilter.hpp"
#include "IirFilter.hpp"
#include "Window.hpp"

int main() {
    md::FirFilter<double, 15> test("test");
    test.setupBandPass(0.1, 0.2);

    std::vector<double> impulse(15, 0.0);
    impulse[0] = 1.0;

    test.process(impulse.data(), impulse.size());

    for (auto x : impulse) {
        std::cout << x << ", ";
    }
    std::cout << std::endl;
    md::Window<double, 15> hamming("hamming");
    hamming.setupHamming();

    hamming.process(impulse.data(), impulse.size());

    for (auto x : impulse) {
        std::cout << x << ", ";
    }

    std::cout << std::endl;
    md::IirFilter<double, 3, 2> test1("test1");
    std::array<double, 3> b = {0.02008337, 0.04016673, 0.02008337};
    std::array<double, 2> a = {-1.56101808, 0.64135154};

    test1.setCoefficients(b, a);

    std::vector<double> impulse1(15, 0.0);
    impulse1[0] = 1.0;

    test1.process(impulse1.data(), impulse1.size());

    for (auto x : impulse1) {
        std::cout << x << ", ";
    }

    return 0;
}