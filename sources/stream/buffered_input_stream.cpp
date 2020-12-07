#include "stream/buffered_input_stream.hpp"
#include <iostream>
#include <assert.h>

io::BufferedInputStream::BufferedInputStream(std::uint32_t read_size,
                                             std::uint32_t buffer_size)
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

    while (_reader.read(_buffer));

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
    int ret_state = fseek(_file, 0, SEEK_SET);
    assert(ret_state == 0);
    _ptr = std::unique_ptr<char>(new char[_read_size]);
    _eof_reached = reset();
}

bool io::BufferedInputStream::BufferReader::read(util::StringBuffer& str)
{
    if (_cursor > _cur_read - 1) {
        if (!_eof_reached) {
            _eof = reset();
        }
    }
    // look for an occurrence of \n in the buffer content
    char* end_of_line = strstr(_ptr.get() + _cursor, "\n");

    // copy all the content
    if (end_of_line == nullptr) {
        str.add(_ptr.get() + _cursor, _read_size - _cursor + 1);
        _cursor = _read_size;
    }
    else {
        str.add(_ptr.get() + _cursor, end_of_line - _ptr.get() - _cursor);
        _cursor += (end_of_line - _ptr.get()) - _cursor + 1;
    }

    // end of file reached. cannot read anymore.
    if (_eof) {
        return false;
    }

    // if eol or the eof are not found, then the read function needs to be called
    // again, it will therefore return true.
    return (end_of_line == nullptr);
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

