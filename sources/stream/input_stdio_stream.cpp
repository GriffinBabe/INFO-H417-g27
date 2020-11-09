#include "stream/input_stdio_stream.hpp"
#include <iostream>
#include <string.h>

io::StdioInputStream::~StdioInputStream()
{
    if (_file_open) {
        int ret_status = fclose(_file);
        if (ret_status != 0) {
            std::cerr
                << "Error: couldn't close file in StdioOutputStream destructor"
                << std::endl;
        }
    }
}

bool io::StdioInputStream::open(const std::string& file)
{
    _file = fopen(file.c_str(), "r");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;
    // sets the cursor at the begin of the file.
    return fseek(_file, 0, SEEK_SET) == 0;
}

bool io::StdioInputStream::seek(std::uint32_t pos)
{
    if (!_file_open) {
        return false;
    }
    return fseek(_file, pos, SEEK_SET);
}

std::string io::StdioInputStream::readln()
{
    if (!_file_open) {
        throw std::runtime_error(
            "Tried to perform line read but the file is not opened yet.");
    }
    while(fgets(str,sizeof(str),_file) == nullptr){
        if(feof(_file)){
            _file_open = false;
            break;
        }// to do : add something more dynamic by extanding the buffer with memcpy (or is it efficient ? must avoid overhead)
    }
    std::string input_str(str);
    return input_str;
}

bool io::StdioInputStream::end_of_stream() const
{
    if (_file_open) {
        return false;
    }
    return feof(_file);
}
