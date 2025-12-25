#pragma once
#include <array>
#include <stdexcept>

namespace md {

/**
 * \brief Abstract base class for all signal processors.
 *
 * Provides a common interface for signal processing operations including
 * filters and windows. All processors store processing factors/coefficients
 * and implement a process() method. Template parameters are validated at
 * compile time to ensure correct usage (Size > 0 and T is floating-point).
 *
 * \tparam T Data type (must be a floating-point type: float, double, or long double).
 * \tparam Size Number of processing factors/coefficients (must be > 0).
 */
template <typename T, size_t Size>
class SignalProcessor {
   protected:
    /// @brief coefficients/factors array
    std::array<T, Size> m_factors;

    /**
     * \brief Creates a new SignalProcessor with zeroed factors.
     *
     * Initializes all processing factors to zero. Performs compile-time
     * validation to ensure Size > 0 and T is a floating-point type.
     * This constructor is protected and can only be called by derived classes.
     */
    SignalProcessor() {
        static_assert(Size > 0, "Size must be positive!");
        static_assert(std::is_floating_point<T>::value, "Template type T must be a floating point type!");
        m_factors.fill(0.0);
    }

    /**
     * \brief Creates a copy of an existing SignalProcessor.
     *
     * Copies all processing factors from the source processor.
     * This constructor is protected and can only be called by derived classes.
     *
     * \param other The source processor to copy from.
     */
    SignalProcessor(const md::SignalProcessor<T, Size>& other) : m_factors(other.m_factors) {}

   public:
    /// @brief Processes signal
    /// @param signal Signal array pointer
    /// @param length Signal length
    virtual void process(T* signal, size_t length) = 0;

    /**
     * \brief Sets the processing factors/coefficients.
     *
     * Replaces the current processing factors with new values.
     * Factors can be generated in matlab. It works in polimorfizm.
     *
     * \warning No validation
     * is performed on the input values. It is the caller's responsibility to
     * ensure the factors are valid for the intended processing operation
     * (e.g., finite values, proper normalization for filters).
     *
     * \param factors Array of Size processing factors to set.
     */
    void setFactors(const std::array<T, Size>& factors) { m_factors = factors; }

    /**
     * \brief Gets a copy of the current processing factors.
     *
     * Returns a copy of the internal factors array. Modifications to the
     * returned array will not affect the processor's internal state.
     *
     * \return Copy of the factors array.
     */
    std::array<T, Size> getFactors() const { return m_factors; }

    /// @brief Pure virtual destructor
    virtual ~SignalProcessor() = default;
};
}  // namespace md