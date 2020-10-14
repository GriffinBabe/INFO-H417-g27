#pragma once
#include "input_stream.hpp"

namespace io {
class SimpleInputStream : public InputStream {
public:
    SimpleInputStream();
    ~SimpleInputStream();

    SimpleInputStream(std::uint16_t buffer_size);

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:
    /**
     * File descriptor.
     */
    FILE* _file = nullptr;

    /**
     * The base size allocation for the buffer in the read() function.
     */
    std::uint16_t _buffer_size;

    /**
     * Tracks if the file is open or not.
     */
    bool _file_open = false;
};
} // namespace io
