#pragma once
#include <array>
#include <stdexcept>
#include <string>

namespace md {

template <typename T, size_t Size>
class SignalProcessor {
   protected:
    std::string m_name;
    std::array<T, Size> m_factors;

    SignalProcessor(const std::string& name) {
        static_assert(Size > 0, "Size must be positive!");

        if (name.empty()) {
            throw std::invalid_argument("Name cannot be empy!");
        }
        m_name = name;
        m_factors.fill(0.0);
    }

    SignalProcessor(const md::SignalProcessor<T, Size>& other) : m_name(other.m_name), m_factors(other.m_factors) {}

   public:
    virtual void process(T* signal, size_t length) = 0;

    void setName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("Name cannot be empy!");
        }
        m_name = name;
    }

    void setFactors(const std::array<T, Size>& factors) { m_factors = factors; }

    virtual ~SignalProcessor() = default;
};
}  // namespace md