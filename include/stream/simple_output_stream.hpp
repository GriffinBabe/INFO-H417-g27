#pragma once
#include "output_stream.hpp"

namespace io {
class SimpleOutputStream : public OutputStream {
public:
    SimpleOutputStream() = default;

    virtual ~SimpleOutputStream();

    SimpleOutputStream(std::uint16_t buffer_size);

    virtual bool create(std::string const& path) override;

    virtual bool writeln(std::string str) override;

    virtual bool close() override;

private:
    FILE* _file = nullptr;
    std::uint16_t _buffer_size = 10;
    bool _file_open = false;
};
} // namespace io
