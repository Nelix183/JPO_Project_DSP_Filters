#pragma once
#include "SignalProcessor.hpp"

namespace md {
template <typename T, size_t Size>
class Filter : public SignalProcessor<T, Size> {
   public:
    Filter(const std::string& name) : SignalProcessor<T, Size>(name) {}

    virtual void reset() = 0;

    virtual ~Filter() = default;
};
}  // namespace md