#pragma once
#include "stream/input_stream.hpp"
#include "stream/string_buffer.hpp"

namespace io {
class BufferedInputStream : public InputStream {
public:
    BufferedInputStream() = default;
    BufferedInputStream(std::uint32_t read_size = 10,
                        std::uint32_t buffer_size = 10);

    virtual ~BufferedInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:
    /**
     * Character read buffer wrapper.
     */
    class BufferReader {
    public:
        BufferReader() = default;

        BufferReader(FILE* file, std::uint32_t read_size = 10);

        /**
         * Reads the content of the BufferReader and copies it in the given
         * StringBuffer. If the end of line is reached or the end of stream is reached, returns false.
         * @param str the StringBuffer to copy data into.
         * @return false is the eol or eof is reached. true otherwise.
         */
        bool read(util::StringBuffer& str);

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
     * The string buffer used in the readln() function.
     */
    util::StringBuffer _buffer;

    /**
     * Our encapsulated reader object.
     */
    BufferReader _reader;

    /**
     * Size of the buffer vector in the readln() function
     */
    std::uint32_t _buffer_size = 10;

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
