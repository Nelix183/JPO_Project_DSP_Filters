#pragma once
#include "SignalProcessor.hpp"
namespace md {
template <typename T, size_t Size>
class Window : public SignalProcessor<T, Size> {
   public:
    void process(T* signal, size_t length) {
        if (signal == nullptr || length != Size) {
            throw std::invalid_argument("Bad array!");
        }
        for (size_t i = 0; i < length; i++) {
            signal[i] *= this->m_factors[i];
        }
    }

    void setupRectangular() { this->m_factors.fill(static_cast<T>(1.0)); }

    void setupHamming() {
        if (Size <= 1) return;
        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T N = static_cast<T>(Size - 1);
            T val = 0.54 - 0.46 * std::cos((2.0 * M_PI * n) / N);
            this->m_factors[i] = static_cast<T>(val);
        }
    }

    void setupHann() {
        if (Size <= 1) return;
        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T N = static_cast<T>(Size - 1);
            T val = 0.5 * (1.0 - std::cos((2.0 * M_PI * n) / N));
            this->m_factors[i] = static_cast<T>(val);
        }
    }

    void setupBlackman() {
        if (Size <= 1) return;
        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T N = static_cast<T>(Size - 1);
            T term1 = std::cos((2.0 * M_PI * n) / N);
            T term2 = std::cos((4.0 * M_PI * n) / N);
            T val = 0.42 - 0.5 * term1 + 0.08 * term2;
            this->m_factors[i] = static_cast<T>(val);
        }
    }

    Window(const std::string& name) : SignalProcessor<T, Size>(name) { setupRectangular(); }
};
}  // namespace md