#pragma once
#include "output_stream.hpp"

namespace io {
class BufferedOutputStream : public OutputStream {
public:
    BufferedOutputStream() = default;

    virtual ~BufferedOutputStream();

    BufferedOutputStream(std::uint32_t buffer_size);

    virtual bool create(std::string const& path) override;

    virtual bool writeln(std::string str) override;

    virtual bool close() override;

private:
    /**
     * Character write buffer wrapper
     */
    class BufferWriter {
    public:
        BufferWriter();

        BufferWriter(FILE* file, std::uint32_t write_size = 10);

        /**
         * Registers a new character to the buffer. If the cursor
         * reaches the buffer size, then calls flush()
         * @param c the character to add to the buffer.
         * @return if the character was successfully added.
         */
        bool write(char c);

        /**
         * Writes all the buffer content to the file and resets
         * the cursor to 0
         * @return bool, if the buffer has been successfully written to the
         * file.
         */
        bool flush();

    private:
        /**
         * Wrapped buffer containing characrers to write.
         */
        std::unique_ptr<char> _ptr;

        /**
         * Size of the wrapped buffer.
         */
        std::uint32_t _write_size = 10;

        /**
         * Position of last character in our buffer.
         */
        std::uint32_t _cursor = 0;

        /**
         * File descriptor.
         */
        FILE* _file = nullptr;
    };

    std::uint32_t _buffer_size = 10;

    BufferWriter _writer;

    FILE* _file = nullptr;

    bool _file_open = false;
};
} // namespace io