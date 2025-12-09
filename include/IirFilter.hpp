#pragma once
#include "Filter.hpp"

namespace md {
template <typename T, size_t NumB, size_t NumA>
class IirFilter : public Filter<T, NumB + NumA> {
   private:
    std::array<T, NumB> m_inBuff;
    std::array<T, NumA> m_outBuff;

    T processSample(T input) {
        for (size_t i = NumB - 1; i > 0; i--) {
            m_inBuff[i] = m_inBuff[i - 1];
        }
        m_inBuff[0] = input;

        T feedforward = static_cast<T>(0.0);
        for (size_t i = 0; i < NumB; i++) {
            feedforward += this->m_factors[i] * m_inBuff[i];
        }

        T feedback = static_cast<T>(0.0);
        for (size_t i = 0; i < NumA; i++) {
            feedback += this->m_factors[NumB + i] * m_outBuff[i];
        }

        T output = feedforward - feedback;

        for (size_t i = NumA - 1; i > 0; i--) {
            m_outBuff[i] = m_outBuff[i - 1];
        }
        if (NumA > 0) {
            m_outBuff[0] = output;
        }

        return output;
    }

   public:
    void setCoefficients(const std::array<T, NumB>& bFactors, const std::array<T, NumA>& aFactors) {
        for (size_t i = 0; i < NumB; i++) {
            this->m_factors[i] = bFactors[i];
        }

        for (size_t i = 0; i < NumA; i++) {
            this->m_factors[NumB + i] = aFactors[i];
        }
    }

    void process(T* signal, size_t length) override {
        if (signal == nullptr || length == 0) {
            throw std::invalid_argument("Bad array!");
        }
        for (size_t i = 0; i < length; i++) {
            signal[i] = processSample(signal[i]);
        }
    }

    void reset() override {
        m_inBuff.fill(static_cast<T>(0.0));
        m_outBuff.fill(static_cast<T>(0.0));
        this->m_factors.fill(static_cast<T>(0.0));
    }

    IirFilter(const std::string& name) : Filter<T, NumB + NumA>(name) { reset(); }
};
}  // namespace md