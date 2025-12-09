#pragma once
#include <cmath>

#include "Filter.hpp"

namespace md {
template <typename T, size_t Size>
class FirFilter : public Filter<T, Size> {
   private:
    std::array<T, Size> m_buffer;
    size_t m_head = 0;

    T processSample(T input) {
        m_buffer[m_head] = input;
        T output = 0.0;
        size_t bufferIdx = m_head;
        for (size_t i = 0; i < Size; i++) {
            output += m_buffer[bufferIdx] * this->m_factors[i];
            if (bufferIdx == 0) {
                bufferIdx = Size - 1;
            } else {
                bufferIdx--;
            }
        }
        m_head++;
        if (m_head >= Size) {
            m_head = 0;
        }
        return output;
    }

   public:
    void setupLowPass(T freq) {  // do sprawdzenia działanie
        if (freq <= 0 || freq >= 0.5) {
            throw std::invalid_argument("Invalid frequency parameters! Use normalized frequency (0.0-0.5).");
        }

        T center = (static_cast<T>(Size) - 1.0) / 2.0;
        T sum = 0.0;

        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T h = 0.0;
            if (i == center) {
                h = 2.0 * freq;
            } else {
                T x = 2.0 * M_PI * freq * (n - center);
                h = std::sin(x) / (M_PI * (n - center));
            }
            this->m_factors[i] = h;
            sum += h;
        }

        for (size_t i = 0; i < Size; i++) {
            this->m_factors[i] /= sum;
        }
    }

    void setupHighPass(T freq) {
        setupLowPass(freq);

        for (size_t i = 0; i < Size; i++) {
            this->m_factors[i] = -this->m_factors[i];
        }

        size_t center = (Size - 1) / 2;
        this->m_factors[center] += 1.0;
    }

    void setupBandPass(T freqLow, T freqHigh) {
        if (freqLow >= freqHigh) {
            throw std::invalid_argument("Low frequency must be smaller than High frequency!");
        }

        setupLowPass(freqHigh);

        std::array<T, Size> highFactors = this->m_factors;

        setupLowPass(freqLow);

        for (size_t i = 0; i < Size; i++) {
            this->m_factors[i] = highFactors[i] - this->m_factors[i];
        }
    }

    void process(T* signal, size_t length) override {
        if (signal == nullptr || length <= 0) {
            throw std::invalid_argument("Bad array!");
        }
        for (size_t i = 0; i < length; i++) {
            signal[i] = processSample(signal[i]);
        }
    }

    void reset() override {
        m_buffer.fill(0.0);
        m_head = 0;
    }

    FirFilter(const std::string& name) : Filter<T, Size>(name) { reset(); }
};
}  // namespace md