#include "stream/buffered_input_stream.hpp"
#include <iostream>
#include <sstream>

io::BufferedInputStream::BufferedInputStream(std::uint16_t read_size,
                                             std::uint16_t buffer_size)
    : _buffer_size(buffer_size), _read_size(read_size)
{
    _buffer = util::StringBuffer(_buffer_size);
    _buffer.reset();
}

io::BufferedInputStream::~BufferedInputStream()
{
    if (_file_open) {
        int ret_status = fclose(_file);
        if (ret_status != 0) {
            std::cerr
                << "Error: couldn't close file in SimpleOutputStream destructor"
                << std::endl;
        }
    }
}

bool io::BufferedInputStream::open(const std::string& file)
{
    _file = fopen(file.c_str(), "r");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;
    _reader = BufferReader(_file, _read_size);
    return true;
}

std::string io::BufferedInputStream::readln()
{
    if (!_file_open) {
        throw std::runtime_error(
            "Tried to perform line read but the file is not opened yet.");
    }
    _buffer.reset();

    // local buffer not to mix with _buffer
    char local_buf;

    while (true) {
        local_buf = _reader.read();
        if (local_buf == '\n' || local_buf == '\0') {
            break;
        }
        _buffer.add(local_buf);
    };

    return _buffer.get();
}

bool io::BufferedInputStream::seek(std::uint32_t pos)
{
    int ret_value = fseek(_file, pos, SEEK_SET);
    _reader.reset();
    return ret_value == 0;
}

bool io::BufferedInputStream::end_of_stream() const
{
    return _reader.eof();
}

//-------------------------------------------------------------------
// Private class BufferReader method definitions
//-------------------------------------------------------------------

io::BufferedInputStream::BufferReader::BufferReader(FILE* file,
                                                    std::uint32_t read_size)
    : _file(file), _read_size(read_size)
{
    assert(file != nullptr);
    assert(fseek(_file, 0, SEEK_SET) == 0);
    _ptr = std::unique_ptr<char>(new char[_read_size]);
    _eof_reached = reset();
}

char io::BufferedInputStream::BufferReader::read()
{
    if (_cursor > _cur_read - 1) {
        if (!_eof_reached) {
            _eof_reached = reset();
        }
        else {
            _eof = true;
            return '\0';
        }
    }
    return _ptr.get()[_cursor++];
}

bool io::BufferedInputStream::BufferReader::reset()
{
    _cursor = 0;
    _cur_read = fread(_ptr.get(), sizeof(char), _read_size, _file);
    return (_cur_read < _read_size);
}

bool io::BufferedInputStream::BufferReader::eof() const
{
    return _eof;
}
