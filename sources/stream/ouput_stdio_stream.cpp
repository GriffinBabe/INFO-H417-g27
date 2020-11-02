#include "stream/ouput_stdio_stream.hpp"

io::StdioOutputStream::~StdioOutputStream()
{
    if (_file_open) {
        std::cout << "Warning: StdioOutputStream destroyed without closing "
                     "the file manually"
                  << std::endl;
        int ret_status = fclose(_file);
        if (ret_status != 0) {
            std::cerr
                << "Error: couldn't close file in StdioOutputStream destructor"
                << std::endl;
        }
    }
}
bool io::StdioOutputStream::create(const std::string& path)
{
    _file = fopen(path.c_str(), "w");
    if (_file == nullptr) {
        return false;
    }
    _file_open = true;

    return fseek(_file, 0, SEEK_SET) == 0;
}

bool io::StdioOutputStream::writeln(std::string instr)
{
    if (!_file_open) {
        return false;
    }
    fputs(instr.c_str(),_file);
}

bool io::StdioOutputStream::close()
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