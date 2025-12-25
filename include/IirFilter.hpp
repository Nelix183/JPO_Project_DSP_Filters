#pragma once
#include "Filter.hpp"

namespace md {
/**
 * \brief Infinite Impulse Response (IIR) filter implementation.
 *
 * Implements a digital IIR filter with separate feedforward and feedback
 * coefficients. IIR filters have infinite-duration impulse response and
 * can achieve sharper frequency responses than FIR filters with fewer
 * coefficients. The filter output depends on both current/past inputs
 * and past outputs, which requires careful coefficient design for stability.
 *
 * \tparam T Data type (must be floating-point).
 * \tparam NumB Number of feedforward (numerator) coefficients.
 * \tparam NumA Number of feedback (denominator) coefficients.
 */
template <typename T, size_t NumB, size_t NumA>
class IirFilter : public Filter<T, NumB + NumA> {
   private:
    /// \brief Input samples buffer
    std::array<T, NumB> m_inBuff;
    /// \brief Output samples buffer
    std::array<T, NumA> m_outBuff;

    /**
     * \brief Processes a single sample through the IIR filter.
     *
     * Implements the IIR difference equation.
     * Updates both input and output history buffers.
     *
     * \param input Input sample value.
     *
     * \return Filtered output sample.
     */
    T processSample(T input) override {
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
    /**
     * \brief Sets the IIR filter coefficients.
     *
     * Configures the filter with feedforward (numerator) and feedback
     * (denominator) coefficients.
     *
     * \param bFactors Array of NumB feedforward coefficients [b0, b1, ..., b(NumB-1)].
     * \param aFactors Array of NumA feedback coefficients [a1, a2, ..., a(NumA)].
     *
     * \note The a0 coefficient is assumed to be 1 and is not included in aFactors.
     * \note Ensure coefficients result in a stable filter (poles inside unit circle).
     */
    void setCoefficients(const std::array<T, NumB>& bFactors, const std::array<T, NumA>& aFactors) {
        for (size_t i = 0; i < NumB; i++) {
            this->m_factors[i] = bFactors[i];
        }

        for (size_t i = 0; i < NumA; i++) {
            this->m_factors[NumB + i] = aFactors[i];
        }
    }

    /**
     * \brief Resets the filter to its initial state.
     *
     * Clears both input and output history buffers and resets all
     * coefficients to zero. This should be called when starting to process
     * a new independent signal. After reset, coefficients must be set again
     * using setCoefficients() before filtering.
     */
    void reset() override {
        m_inBuff.fill(static_cast<T>(0.0));
        m_outBuff.fill(static_cast<T>(0.0));
        this->m_factors.fill(static_cast<T>(0.0));
    }

    /**
     * \brief Creates a new IIR filter with cleared state.
     *
     * Initializes the filter with zeroed buffers and coefficients.
     * Filter must be configured with setCoefficients() before use.
     */
    IirFilter() { reset(); }

    /**
     * \brief Creates a copy of an existing IIR filter.
     *
     * Copies the input buffer, output buffer, and coefficients from the
     * source filter. The copied filter will have the same state and configuration.
     *
     * \param other The source filter to copy from.
     */
    IirFilter(const IirFilter<T, NumB, NumA>& other) {
        m_inBuff = other.m_inBuff;
        m_outBuff = other.m_outBuff;
        this->m_factors = other.m_factors;
    }

    /// @brief Equality comparison operator
    /// @param other Filter to compare with
    /// @return true if filters are equal
    bool operator==(const IirFilter<T, NumB, NumA>& other) const {
        return m_inBuff == other.m_inBuff && m_outBuff == other.m_outBuff && this->m_factors == other.m_factors;
    }

    /// @brief Inequality comparison operator
    /// @param other Filter to compare with
    /// @return true if filters are not equal
    bool operator!=(const IirFilter<T, NumB, NumA>& other) const { return !(*this == other); }
};
}  // namespace md