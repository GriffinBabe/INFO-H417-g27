#include "stream/mmap_output_stream.hpp"

io::MMapOutputStream::MMapOutputStream(std::uint16_t buffer_size)
    : _buffer_size(buffer_size)
{}

io::MMapOutputStream::~MMapOutputStream()
{
    // TODO
}

bool io::MMapOutputStream::create(std::string const& path)
{
    // TODO
}

bool io::MMapOutputStream::writeln(std::string str)
{
    // TODO
}

bool io::MMapOutputStream::close()
{
    // TODO
}

