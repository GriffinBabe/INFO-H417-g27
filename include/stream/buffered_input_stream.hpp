#pragma once
#include "stream/input_stream.hpp"

namespace io {
class BufferedInputStream : public InputStream {
public:
    BufferedInputStream() = default;
    BufferedInputStream(std::uint16_t read_size = 10,
                        std::uint16_t buffer_size = 10);

    virtual ~BufferedInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:
    /**
     * Character buffer wrapper.
     */
    class BufferReader {
    public:
        BufferReader() = default;

        BufferReader(FILE* file, std::uint32_t read_size = 10);

        /**
         * Reads a single character, reloads the buffer though reset() if
         * necessary.
         * @return the read character.
         */
        char read();

        /**
         * Reloads the buffer.
         * Usually called after a seek call or when we have read all the
         * characters in the buffer.
         *
         * @return bool, if we have reached the EOF return true
         */
        bool reset();

        [[ nodiscard ]] bool eof() const;

    private:
        /**
         * Wrapped buffer.
         */
        std::unique_ptr<char> _ptr;

        /**
         * The number of character to read at a time.
         */
        std::uint32_t _read_size = 10;

        /**
         * Normally should be the same size as _read_size, until
         * we reach the end of file.
         */
        std::uint32_t _cur_read = 10;

        /**
         * Current read position in the buffer
         */
        std::uint32_t _cursor = 0;

        /**
         * File descriptor.
         */
        FILE* _file = nullptr;

        /**
         * If we have loaded all the characters until the end of file.
         */
        bool _eof_reached = false;

        /**
         * There is no character left to read.
         */
        bool _eof = false;
    };

    /**
     * Our encapsulated reader object.
     */
    BufferReader _reader;

    /**
     * Size of the buffer vector in the readln() function
     */
    std::uint16_t _buffer_size = 10;

    /**
     * Used by BufferReader.
     */
    std::uint32_t _read_size = 10;

    /**
     * File descriptor
     */
    FILE* _file = nullptr;

    /**
     * Tracks if the file is open or not.
     */
    bool _file_open = false;
};
} // namespace io