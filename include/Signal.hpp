#pragma once
#include <array>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string_view>

namespace md {
/**
 * \brief Represents a digital signal.
 *
 * This class provides a container for signal samples with operations
 * for signal processing, file I/O, and energy calculations. The signal
 * is implemented as a fixed-size array wrapper with additional DSP
 * functionality. All samples are stored contiguously in memory.
 *
 * \tparam T Data type (must be floating point: float, double, or long double).
 * \tparam Size Number of samples in the signal (must be > 0).
 */
template <typename T, size_t Size>
class Signal {
   private:
    /// @brief Signal samples array
    std::array<T, Size> m_samples;

   public:
    /**
     * \brief Creates a new Signal with all samples initialized to zero.
     *
     * Initializes all Size samples to zero. This is useful for creating
     * an empty signal that will be filled later or used as an accumulator.
     */
    Signal() { m_samples.fill(static_cast<T>(0)); }
    /**
     * \brief Creates a new Signal from existing samples.
     *
     * Initializes the signal with a copy of the provided samples array.
     * The array must have exactly Size elements.
     *
     * \param samples Array of initial sample values to copy.
     */
    Signal(const std::array<T, Size>& samples) : m_samples(samples) {}
    /**
     * \brief Creates a copy of an existing Signal.
     *
     * Performs a deep copy of all samples from the source signal.
     *
     * \param other The signal to copy from.
     */
    Signal(const Signal<T, Size>& other) : m_samples(other.m_samples) {}

    /**
     * \brief Reads signal samples from a file.
     *
     * Reads exactly Size samples from a file containing whitespace-separated
     * numeric values (spaces, tabs, or newlines). If the file cannot be opened
     * or contains insufficient data, an exception is thrown. Existing signal
     * data is completely overwritten.
     *
     * \param path Path to the input file containing signal samples.
     *
     * \throws std::invalid_argument if the file cannot be opened.
     * \throws std::runtime_error if the file contains insufficient data or invalid format.
     */
    void signalFromFile(std::string_view path) {
        std::fstream file(path.data(), std::ios::in);
        if (!file.is_open()) {
            throw std::invalid_argument("File can not be open!");
        }
        for (size_t i = 0; i < Size; i++) {
            if (!(file >> m_samples[i])) {
                file.close();
                throw std::runtime_error("Failed to read data");
            }
        }
        file.close();
    }

    /**
     * \brief Writes signal samples to a file.
     *
     * Writes all signal samples to a file, one value per line. If the file
     * already exists, it will be overwritten. If the file cannot be created
     * or opened for writing, an exception is thrown.
     *
     * \param path Path to the output file.
     *
     * \throws std::invalid_argument if the file cannot be opened for writing.
     */
    void signalToFile(std::string_view path) {
        std::fstream file(path.data(), std::ios::out);
        if (!file.is_open()) {
            throw std::invalid_argument("File can not be open!");
        }

        for (size_t i = 0; i < Size; i++) {
            file << m_samples[i] << "\n";
        }
        file.close();
    }

    /**
     * \brief Calculates the total energy of the signal.
     *
     * Computes the signal energy as the sum of squared samples.
     *
     * \return The total energy of the signal.
     */
    T energy() const {
        T result = 0;
        for (size_t i = 0; i < Size; i++) {
            result += m_samples[i] * m_samples[i];
        }
        return result;
    }

    /**
     * \brief Calculates the average power of the signal.
     *
     * Computes the average power as energy divided by the number of samples.
     *
     * \return The average power of the signal.
     */
    T power() const { return this->energy() / Size; }

    /**
     * \brief Calculates the RMS (Root Mean Square) value.
     *
     * Computes RMS as the square root of the average power.
     *
     * \return The RMS value of the signal.
     */
    T rms() const { return std::sqrt(this->power()); }

    /// @brief Gets signal size
    /// @return Number of samples
    constexpr size_t size() const { return Size; }

    /// @brief Gets pointer to samples data
    /// @return Pointer to samples array
    T* data() { return m_samples.data(); }
    /// @brief Gets const pointer to samples data
    /// @return Const pointer to samples array
    const T* data() const { return m_samples.data(); }

    /// @brief Gets iterator to beginning
    /// @return Begin iterator
    typename std::array<T, Size>::iterator begin() { return m_samples.begin(); }
    /// @brief Gets iterator to end
    /// @return End iterator
    typename std::array<T, Size>::iterator end() { return m_samples.end(); }

    /**
     * \brief Accesses signal sample at given index.
     *
     * Provides read/write access to the signal sample at the specified index.
     * Performs bounds checking and throws an exception if the index is invalid.
     *
     * \param index The index of the sample to access (must be 0 <= index < Size).
     *
     * \return Reference to the sample at the specified index.
     *
     * \throws std::out_of_range if index >= Size.
     */
    T& operator[](size_t index) {
        if (index >= Size) {
            throw std::out_of_range("Index out of bounds!");
        }
        return m_samples[index];
    }

    /**
     * \brief Assigns values from another signal.
     *
     * Copies all samples from the source signal to this signal.
     * Includes self-assignment protection to avoid unnecessary copying.
     *
     * \param other The source signal to copy from.
     *
     * \return Reference to this signal after assignment.
     */
    Signal& operator=(const Signal<T, Size>& other) {
        if (this != &other) {
            m_samples = other.m_samples;
        }
        return *this;
    }

    /**
     * \brief Compares two signals for equality.
     *
     * Two signals are considered equal if all corresponding samples
     * have identical values.
     *
     * \param other Signal to compare with.
     *
     * \return true if all samples are equal, false otherwise.
     */
    bool operator==(const Signal<T, Size>& other) const { return m_samples == other.m_samples; }

    /// @brief Inequality comparison operator
    /// @param other Signal to compare with
    /// @return true if signals are not equal
    bool operator!=(const Signal<T, Size>& other) const { return !(*this == other); }

    /**
     * \brief Adds two signals element-wise.
     *
     * Creates and returns a new signal containing the sum.
     *
     * \param other Signal to add.
     *
     * \return A new signal containing the element-wise sum.
     */
    Signal operator+(const Signal<T, Size>& other) const {
        Signal result;
        for (size_t i = 0; i < Size; i++) {
            result.m_samples[i] = m_samples[i] + other.m_samples[i];
        }
        return result;
    }

    /**
     * \brief Adds another signal to this signal in-place.
     *
     * Performs element-wise addition: this[i] += other[i].
     * Modifies this signal directly.
     *
     * \param other Signal to add.
     *
     * \return Reference to this signal after addition.
     */
    Signal& operator+=(const Signal<T, Size>& other) {
        for (size_t i = 0; i < Size; i++) {
            m_samples[i] += other.m_samples[i];
        }
        return *this;
    }

    /**
     * \brief Subtracts two signals element-wise.
     *
     * Performs element-wise subtraction: result[i] = this[i] - other[i].
     * Creates and returns a new signal containing the difference.
     *
     * \param other Signal to subtract.
     *
     * \return A new signal containing the element-wise difference.
     */
    Signal operator-(const Signal<T, Size>& other) const {
        Signal result;
        for (size_t i = 0; i < Size; i++) {
            result.m_samples[i] = m_samples[i] - other.m_samples[i];
        }
        return result;
    }

    /// @brief Subtraction assignment operator
    /// @param other Signal to subtract
    /// @return Reference to this signal
    Signal& operator-=(const Signal<T, Size>& other) {
        for (size_t i = 0; i < Size; i++) {
            m_samples[i] -= other.m_samples[i];
        }
        return *this;
    }
};
}  // namespace md