#include <iostream>
#include <vector>

#include "FirFilter.hpp"

int main() {
    md::FirFilter<double, 5> test("test");
    test.setupLowPass(0.1);

    std::vector<double> impulse(15, 0.0);
    impulse[0] = 1.0;

    test.process(impulse.data(), impulse.size());

    for (auto x : impulse) {
        std::cout << x << " ";
    }

    return 0;
}