#include "stream/simple_output_stream.hpp"
#include <iostream>

io::SimpleOutputStream::~SimpleOutputStream()
{
    if (_file_open) {
        std::cout << "Warning: SimpleOutputStream destroyed without closing "
                     "the file manually"
                  << std::endl;
        int ret_status = fclose(_file);
        if (ret_status != 0) {
            std::cerr
                << "Error: couldn't close file in SimpleOutputStream destructor"
                << std::endl;
        }
    }
}

io::SimpleOutputStream::SimpleOutputStream(std::uint32_t buffer_size)
    : _buffer_size(buffer_size)
{
}

bool io::SimpleOutputStream::create(const std::string& path)
{
    _file = fopen(path.c_str(), "w");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;
    return fseek(_file, 0, SEEK_SET) == 0;
}

bool io::SimpleOutputStream::writeln(std::string str)
{
    if (!_file_open) {
        return false;
    }
    // iterates over the string and writes character by character
    auto str_iterator = str.begin();
    while (str_iterator != str.end()) {
        if (fwrite(&*str_iterator, sizeof(char), 1, _file) != 1) {
            return false;
        }
        str_iterator++;
    }
    if (str.back() != '\n') {
        // terminates with a new line character
        const char new_line = '\n';
        if (fwrite(&new_line, sizeof(char), 1, _file) != 1)
            return false;
    }
    return true;
}

bool io::SimpleOutputStream::close()
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
