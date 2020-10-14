#include "stream/simple_input_stream.hpp"
#include <vector>

io::SimpleInputStream::~SimpleInputStream()
{
    fclose(_file);
}

bool io::SimpleInputStream::open(const std::string& file)
{
    _file = fopen(file.c_str(), "r");
    return _file != nullptr;
}

std::shared_ptr<char[]> io::SimpleInputStream::readln()
{
    std::vector<char> characters;
    char buffer;

    while (fgets(&buffer, 1, _file) != NULL) {
        if (buffer == '\n') {
            break;
        }
        characters.push_back(buffer);
    }
    // TODO return buffer
}
