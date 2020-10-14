#pragma once
#include "output_stream.hpp"

namespace io {
class SimpleOutputStream : public OutputStream {
public:
    SimpleOutputStream();
    ~SimpleOutputStream();
    SimpleOutputStream(std::uint16_t buffer_size);

private:

    FILE* _file = nullptr;
    std::uint16_t _buffer_size;
};
} // namespace io
