#pragma once
#include <cmath>

#include "Filter.hpp"

namespace md {
/**
 * \brief Finite Impulse Response (FIR) filter implementation.
 *
 * Implements a digital FIR filter using a circular buffer for efficient
 * processing. FIR filters have finite-duration impulse response and are
 * always stable. The filter output depends only on current and past inputs,
 * not on past outputs. Supports low-pass, high-pass, and band-pass designs.
 *
 * \tparam T Data type (must be floating-point).
 * \tparam Size Filter order (number of coefficients).
 */
template <typename T, size_t Size>
class FirFilter : public Filter<T, Size> {
   private:
    /// \brief Circular buffer for input samples
    std::array<T, Size> m_buffer;
    /// \brief Current buffer head position
    size_t m_head = 0;

    /**
     * \brief Processes a single sample through the FIR filter.
     *
     * Implements the FIR convolution algorithm.
     * Stores the input in a circular buffer and computes the weighted sum
     * of the current and past Size-1 samples using the filter coefficients.
     *
     * \param input Input sample value.
     *
     * \return Filtered output sample.
     */
    T processSample(T input) override {
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
    /**
     * \brief Configures the filter as a low-pass filter.
     *
     * Designs FIR filter coefficients for a low-pass filter using the
     * windowed-sinc method. The cutoff frequency must be normalized to
     * the Nyquist frequency (0.0, 0.5). Frequencies below
     * the cutoff are passed, frequencies above are attenuated. The filter
     * coefficients are automatically normalized to unity gain at DC.
     *
     * \param freq Normalized cutoff frequency in range (0.0, 0.5).
     *             Example: 0.1 means 0.1 * (sample_rate / 2).
     *
     * \throws std::invalid_argument if freq <= 0.0 or freq >= 0.5.
     */
    void setupLowPass(T freq) {
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

    /**
     * \brief Configures the filter as a high-pass filter.
     *
     * Designs FIR filter coefficients for a high-pass filter using spectral
     * inversion of a low-pass filter. Frequencies above the cutoff are passed,
     * frequencies below are attenuated. First creates a low-pass filter, then
     * inverts it by negating all coefficients and adding 1 to the center tap.
     *
     * \param freq Normalized cutoff frequency in range (0.0, 0.5).
     *
     * \throws std::invalid_argument if freq <= 0.0 or freq >= 0.5 (via setupLowPass).
     */
    void setupHighPass(T freq) {
        setupLowPass(freq);

        for (size_t i = 0; i < Size; i++) {
            this->m_factors[i] = -this->m_factors[i];
        }

        size_t center = (Size - 1) / 2;
        this->m_factors[center] += 1.0;
    }

    /**
     * \brief Configures the filter as a band-pass filter.
     *
     * Designs FIR filter coefficients for a band-pass filter by subtracting
     * a low-pass filter from another low-pass filter with higher cutoff.
     * Only frequencies between freqLow and freqHigh are passed through,
     * frequencies outside this range are attenuated.
     *
     * \param freqLow Normalized lower cutoff frequency (0.0-0.5).
     * \param freqHigh Normalized upper cutoff frequency (0.0-0.5).
     *
     * \throws std::invalid_argument if freqLow >= freqHigh.
     * \throws std::invalid_argument if frequencies are out of valid range (via setupLowPass).
     */
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

    /**
     * \brief Resets the filter to its initial state.
     *
     * Clears the internal circular buffer and resets the buffer head
     * position to zero. This should be called when starting to process
     * a new independent signal to avoid contamination from previous data.
     * Filter coefficients are not affected.
     */
    void reset() override {
        m_buffer.fill(0.0);
        m_head = 0;
    }

    /**
     * \brief Creates a new FIR filter with cleared state.
     *
     * Initializes the filter with zeroed buffer and coefficients.
     * Filter must be configured with a setup method before use.
     */
    FirFilter() { reset(); }

    /**
     * \brief Creates a copy of an existing FIR filter.
     *
     * Copies the buffer, head position, and coefficients from the source filter.
     * The copied filter will have the same state and configuration.
     *
     * \param other The source filter to copy from.
     */
    FirFilter(const FirFilter<T, Size>& other) {
        m_buffer = other.m_buffer;
        m_head = other.m_head;
        this->m_factors = other.m_factors;
    }

    /// @brief Equality comparison operator
    /// @param other Filter to compare with
    /// @return true if filters are equal
    bool operator==(const FirFilter<T, Size>& other) const {
        return m_buffer == other.m_buffer && m_head == other.m_head && this->m_factors == other.m_factors;
    }

    /// @brief Inequality comparison operator
    /// @param other Filter to compare with
    /// @return true if filters are not equal
    bool operator!=(const FirFilter<T, Size>& other) const { return !(*this == other); }
};
}  // namespace md