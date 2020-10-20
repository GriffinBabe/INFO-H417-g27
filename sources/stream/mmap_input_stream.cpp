#include "stream/mmap_input_stream.hpp"


io::MMapInputStream::MMapInputStream(std::uint16_t read_size,
                                     std::uint16_t buffer_size)
    : _buffer_size(buffer_size), _read_size(read_size)
{}

io::MMapInputStream::~MMapInputStream()
{
    // TODO
}

bool io::MMapInputStream::open(std::string const& file)
{
    const char* file_name = (const char*) &file;
    _mapped_file = bip::file_mapping( file_name, bip::read_only);
    _mapped_region = bip::mapped_region(_mapped_file,
                                        bip::read_only,
                                        0,
                                        _buffer_size);
    // TODO
}

std::string readln()
{
    // TODO
}

bool seek(std::uint32_t pos)
{
    // TODO
}

bool io::MMapInputStream::end_of_stream()
{
    // TODO
}
