#include "stream/simple_input_stream.hpp"
#include <iostream>
#include <vector>

io::SimpleInputStream::SimpleInputStream(std::uint32_t buffer_size)
    : _buffer_size(buffer_size)
{
    _buffer = util::StringBuffer(_buffer_size);
}

io::SimpleInputStream::~SimpleInputStream()
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

bool io::SimpleInputStream::open(const std::string& file)
{
    _file = fopen(file.c_str(), "r");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;
    // sets the cursor at the begin of the file.
    return fseek(_file, 0, SEEK_SET) == 0;
}

std::string io::SimpleInputStream::readln()
{
    if (!_file_open) {
        throw std::runtime_error(
            "Tried to perform line read but the file is not opened yet.");
    }
    // simple character buffer
    _buffer.reset();

    char local_buf;

    while (fread(&local_buf, sizeof(char), 1, _file) == 1) {
        if (local_buf == '\n') {
            break;
        }
        _buffer.add(local_buf);
    }
    return _buffer.get();
}

bool io::SimpleInputStream::seek(std::uint32_t pos)
{
    if (!_file_open) {
        return false;
    }
    return fseek(_file, pos, SEEK_SET);
}

bool io::SimpleInputStream::end_of_stream() const
{
    if (!_file_open) {
        return false;
    }
    return feof(_file);
}
