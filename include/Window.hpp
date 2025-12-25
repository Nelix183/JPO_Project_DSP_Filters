#pragma once
#include <cmath>

#include "SignalProcessor.hpp"
namespace md {
/**
 * \brief Window functions for signal processing.
 *
 * Provides various window functions used in spectral analysis and filter design.
 * Supports rectangular, Hamming, Hann, and Blackman windows.
 *
 * \tparam T Data type (must be floating-point).
 * \tparam Size Window size (number of samples).
 */
template <typename T, size_t Size>
class Window : public SignalProcessor<T, Size> {
   public:
    /**
     * \brief Applies window function to a signal.
     *
     * Multiplies each sample of the signal by the corresponding window
     * coefficient (element-wise multiplication). The signal is modified
     * in-place. The signal length must exactly match the window size.
     *
     * \param signal Pointer to the signal array to process.
     * \param length Number of samples in the signal (must equal Size).
     *
     * \throws std::invalid_argument if signal is nullptr or length != Size.
     */
    void process(T* signal, size_t length) override {
        if (signal == nullptr || length != Size) {
            throw std::invalid_argument("Bad array!");
        }
        for (size_t i = 0; i < length; i++) {
            signal[i] *= this->m_factors[i];
        }
    }

    /**
     * \brief Configures a rectangular (boxcar) window.
     *
     * Sets all window coefficients to 1.0, effectively applying no windowing.
     * The rectangular window has the narrowest main lobe but highest side lobes,
     * resulting in maximum spectral leakage.
     */
    void setupRectangular() { this->m_factors.fill(static_cast<T>(1.0)); }

    /**
     * \brief Configures a Hamming window.
     *
     * Computes Hamming window coefficients.
     * The Hamming window is commonly used in spectral analysis to reduce
     * spectral leakage with moderate side lobe suppression.
     *
     * \note If Size <= 1, the function returns without changes.
     */
    void setupHamming() {
        if (Size <= 1) return;
        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T N = static_cast<T>(Size - 1);
            T val = 0.54 - 0.46 * std::cos((2.0 * M_PI * n) / N);
            this->m_factors[i] = static_cast<T>(val);
        }
    }

    /**
     * \brief Configures a Hann (Hanning) window.
     *
     * Computes Hann window coefficients.
     * The Hann window provides good frequency resolution and moderate
     * side lobe suppression, commonly used in FFT analysis.
     *
     * \note If Size <= 1, the function returns without changes.
     */
    void setupHann() {
        if (Size <= 1) return;
        for (size_t i = 0; i < Size; i++) {
            T n = static_cast<T>(i);
            T N = static_cast<T>(Size - 1);
            T val = 0.5 * (1.0 - std::cos((2.0 * M_PI * n) / N));
            this->m_factors[i] = static_cast<T>(val);
        }
    }

    /**
     * \brief Configures a Blackman window.
     *
     * Computes Blackman window coefficients.
     * The Blackman window provides excellent side lobe suppression
     * (better than Hamming/Hann) at the cost of wider main lobe.
     *
     * \note If Size <= 1, the function returns without changes.
     */
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

    /**
     * \brief Creates a new Window with rectangular coefficients.
     *
     * Initializes the window with rectangular (all ones) coefficients.
     * Can be reconfigured using setup methods.
     */
    Window() { setupRectangular(); }

    /**
     * \brief Creates a copy of an existing Window.
     *
     * Copies the window coefficients from the source window.
     *
     * \param other The source window to copy from.
     */
    Window(const Window<T, Size>& other) { this->m_factors = other.m_factors; }

    /// @brief Equality comparison operator
    /// @param other Window to compare with
    /// @return true if windows are equal
    bool operator==(const Window<T, Size>& other) const { return this->m_factors == other.m_factors; }

    /// @brief Inequality comparison operator
    /// @param other Window to compare with
    /// @return true if windows are not equal
    bool operator!=(const Window<T, Size>& other) const { return !(*this == other); }
};
}  // namespace md