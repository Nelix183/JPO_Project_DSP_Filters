#pragma once
#include "SignalProcessor.hpp"

namespace md {
/**
 * \brief Abstract base class for digital filters.
 *
 * Provides common functionality for digital signal filtering including
 * FIR and IIR filters. Filters process signals by modifying each sample
 * according to stored coefficients and internal state. The class maintains
 * filter state across multiple process() calls, enabling continuous filtering.
 *
 * \tparam T Data type (must be floating-point).
 * \tparam Size Filter order (number of coefficients).
 */
template <typename T, size_t Size>
class Filter : public SignalProcessor<T, Size> {
   public:
    /**
     * \brief Resets the filter to its initial state.
     *
     * Clears all internal buffers and state variables. This should be called
     * when starting to process a new independent signal to avoid contamination
     * from previous data. Implementation is filter-specific.
     */
    virtual void reset() = 0;

    /// @brief Pure virtual destructor
    virtual ~Filter() = default;

    /**
     * \brief Processes a signal container in-place.
     *
     * Applies the filter to each sample in a container that provides
     * iterators (e.g., std::vector, std::array, Signal). The container's
     * elements are modified in-place. The filter maintains its internal
     * state across samples.
     *
     * \tparam Container Type of container (must support range-based for loop).
     * \param signal Signal container to process (modified in-place).
     */
    template <typename Container>
    void process(Container& signal) {
        for (auto& sample : signal) {
            sample = processSample(sample);
        }
    }

    /**
     * \brief Processes a signal array in-place.
     *
     * Applies the filter to each sample of the input signal array,
     * modifying the signal in-place. The filter maintains its internal
     * state across calls, enabling continuous processing of signal.
     *
     * \param signal Pointer to the signal array to process.
     * \param length Number of samples in the signal array.
     *
     * \throws std::invalid_argument if signal is nullptr or length is 0.
     */
    void process(T* signal, size_t length) override {
        if (signal == nullptr || length == 0) {
            throw std::invalid_argument("Bad array!");
        }
        for (size_t i = 0; i < length; i++) {
            signal[i] = processSample(signal[i]);
        }
    }

   protected:
    /// @brief Default constructor
    Filter() = default;

   private:
    /**
     * \brief Processes a single sample through the filter.
     *
     * Implements the core filtering algorithm for one sample.
     * Updates internal filter state and returns the filtered output.
     * Implementation is filter-specific (FIR or IIR).
     *
     * \param input Input sample value.
     *
     * \return Filtered output sample.
     */
    virtual T processSample(T input) = 0;
};
}  // namespace md