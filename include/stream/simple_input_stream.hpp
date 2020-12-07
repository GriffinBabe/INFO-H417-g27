#pragma once
#include "stream/input_stream.hpp"
#include "stream/string_buffer.hpp"

namespace io {
class SimpleInputStream : public InputStream {
public:
    SimpleInputStream() = default;

    SimpleInputStream(std::uint32_t buffer_size);

    virtual ~SimpleInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:

    /**
     * The string buffer used in the readln() function.
     */
    util::StringBuffer _buffer;

    /**
     * File descriptor.
     */
    FILE* _file = nullptr;

    /**
     * The base size allocation for the buffer in the read() function.
     */
    std::uint32_t _buffer_size = 10;

    /**
     * Tracks if the file is open or not.
     */
    bool _file_open = false;
};
} // namespace io
