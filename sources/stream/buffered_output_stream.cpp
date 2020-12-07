#include "stream/buffered_output_stream.hpp"
#include <iostream>
#include <assert.h>

io::BufferedOutputStream::~BufferedOutputStream()
{
    if (_file_open) {
        std::cout << "Warning: BufferedOutputStream destroyed without closing "
                     "the file manually"
                  << std::endl;
        int ret_status = fclose(_file);
        if (ret_status != 0) {
            std::cerr
                << "Error: couldn't close file in BufferedOutputStream destructor"
                << std::endl;
        }
    }
}

io::BufferedOutputStream::BufferedOutputStream(std::uint16_t buffer_size)
    : _buffer_size(buffer_size)
{
}

bool io::BufferedOutputStream::create(const std::string& path)
{
    _file = fopen(path.c_str(), "w");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;

    _writer = BufferWriter(_file, _buffer_size);
    return fseek(_file, 0, SEEK_SET) == 0;
}

bool io::BufferedOutputStream::writeln(std::string str)
{
    if (!_file_open) {
        return false;
    }
    auto str_iterator = str.begin();
    while (str_iterator != str.end()) {
        if (!_writer.write(*str_iterator)) {
            return false;
        }
        str_iterator++;
    }
    if (str.back() != '\n') {
        const char new_line = '\n';
        if (!_writer.write(new_line)) {
            return false;
        }
    }
    return _writer.flush();
}

bool io::BufferedOutputStream::close()
{
    if (!_file_open) {
        std::cout << "Warning: File is already closed." << std::endl;
        return true;
    }
    int ret_status = fclose(_file);
    if (ret_status == 0) {
        _file_open = false;
    }
    return ret_status == 0;
}

// ---------------------------------------------------------
// BUFFER WRITER METHOD DEFINITIONS
// ---------------------------------------------------------

io::BufferedOutputStream::BufferWriter::BufferWriter()
    : _file(nullptr), _write_size(10)
{

}

io::BufferedOutputStream::BufferWriter::BufferWriter(FILE* file,
                                                     std::uint16_t write_size)
    : _file(file), _write_size(write_size)
{
    assert(file != nullptr);
    int ret_state = fseek(_file, 0, SEEK_SET);
    assert(ret_state == 0);
    _ptr = std::unique_ptr<char>(new char[_write_size]);
}

bool io::BufferedOutputStream::BufferWriter::write(char c)
{
    if (_cursor >= _write_size) {
        if (!flush())
            return false;
    }
    _ptr.get()[_cursor++] = c;
    return true;
}

bool io::BufferedOutputStream::BufferWriter::flush()
{
    int wrote_characters = fwrite(_ptr.get(), sizeof(char), _cursor, _file);
    bool success = wrote_characters == _cursor;
    _cursor = 0;
    return success;
}
