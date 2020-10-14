#include "stream/simple_input_stream.hpp"
#include <vector>

io::SimpleInputStream::SimpleInputStream() : _buffer_size(10)
{
}

io::SimpleInputStream::SimpleInputStream(std::uint16_t buffer_size)
    : _buffer_size(buffer_size)
{
}

io::SimpleInputStream::~SimpleInputStream()
{
    fclose(_file);
}

bool io::SimpleInputStream::open(const std::string& file)
{
    _file = fopen(file.c_str(), "r");
    if (_file == nullptr) {
        return false;
    }
    // sets the cursor at the begin of the file.
    return fseek(_file, 0, SEEK_SET);
}

std::string io::SimpleInputStream::readln()
{
    // simple character buffer
    std::vector<char> characters;
    characters.reserve(_buffer_size);

    char buffer;

    while (fread(&buffer, sizeof(char), 1, _file) != NULL) {
        if (buffer == '\n') {
            break;
        }
        characters.push_back(buffer);
    }
    std::string line(characters.begin(), characters.end());
    return line;
}

bool io::SimpleInputStream::seek(std::uint32_t pos)
{
    return fseek(_file, pos, SEEK_SET);
}

bool io::SimpleInputStream::end_of_stream() const
{
    return feof(_file);
}
