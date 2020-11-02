#include "stream/mmap_output_stream.hpp"

io::MMapOutputStream::MMapOutputStream(std::uint16_t mapping_size)
    : _buffer_size(mapping_size)
{}

io::MMapOutputStream::~MMapOutputStream()
{}

bool io::MMapOutputStream::create(std::string const& path)
{
    return _mapping_handler.create_file();
}

bool io::MMapOutputStream::writeln(std::string str)
{
    return _mapping_handler.writeln_text(str.c_str());
}

bool io::MMapOutputStream::close()
{
    return _mapping_handler.empty_mapping();
}


//-------------------------------------------------------------------
// Private class MappingHandler method definitions
//-------------------------------------------------------------------

io::MMapOutputStream::MappingHandler::MappingHandler(std::string& file,
                                                    std::uint16_t mapping_size)
    : _file_name(file.c_str()), _mapping_size(mapping_size*sizeof(char))
{
    _mapped_file = bip::file_mapping(_file_name, _mode);
    reset();
    _page_size = _mapped_region.get_page_size();
}

bool io::MMapOutputStream::MappingHandler::create_file()
{
    bip::file_mapping::remove(_file_name);
    std::filebuf fbuf;
    fbuf.open(_file_name, std::ios_base::in | std::ios_base::out
              | std::ios_base::trunc);
    if ( fbuf.is_open() )
    {
        fbuf.pubseekoff(0, std::ios_base::beg);
        fbuf.sputc('\0');
        return true;
    }
    else
        return false;
}

bool io::MMapOutputStream::MappingHandler::next_mapping()
{
    return remap(_actual_offset + _mapping_size);
}

bool io::MMapOutputStream::MappingHandler::remap(uintmax_t offset)
{
    _actual_offset = offset;
    _mapped_region = bip::mapped_region(_mapped_file,
                                        _mode,
                                        _actual_offset,
                                        _mapping_size);
    _address = static_cast<char*>(_mapped_region.get_address());
    _mapped_size = _mapped_region.get_size();
    _flush_offset = 0;
    return EXIT_SUCCESS;
}

void io::MMapOutputStream::MappingHandler::reset_vars()
{
    _actual_offset = 0;
    _flush_offset = 0;
}

void io::MMapOutputStream::MappingHandler::reset()
{
    reset_vars();
    _mapped_region = bip::mapped_region(_mapped_file,
                                        _mode,
                                        _actual_offset,
                                        _mapping_size);
    _address = static_cast<char*>(_mapped_region.get_address());
    _mapped_size = _mapped_region.get_size();
}

bool io::MMapOutputStream::MappingHandler::writeln_text(const char *text)
{
    uint16_t length = strlen(text);
    uint16_t first_size = 0; // In case _flush_offset is not zero
    uint16_t last_size = 0; // In case length is not multiple of _mapping_size
    uint16_t complete_loops = 0; // Number of full memcpy to perform.
    if ( _mapping_size - _flush_offset < length )
    {
        first_size = _mapping_size - _flush_offset; // If 0, remap occurs.
        last_size = (length - first_size) % _mapping_size;
        complete_loops = (length - first_size - last_size) / _mapping_size;
    }
    else
        first_size = length - _mapping_size - (_flush_offset+1);

    { // Increase the size of the file to allow mapping. It has its own scope.
        std::filebuf fbuf;
        fbuf.open(_file_name, std::ios_base::in | std::ios_base::out);
        fbuf.pubseekoff(_actual_offset + length + 1, std::ios_base::beg);
        fbuf.sputc('\n');
    }

    if (first_size > 0)
    {
        memcpy(_address, text, first_size);
        _mapped_region.flush(_flush_offset, first_size);
        _flush_offset += first_size;
    }
    for (int i = 0; i < complete_loops; i++)
    {
        next_mapping();
        memcpy(_address,
               &text[first_size + i*_mapping_size],
               _mapping_size);
        _mapped_region.flush(_flush_offset, _mapping_size);
    }
    if (last_size > 0)
    {
        next_mapping();
        memcpy(_address,
               &text[first_size + complete_loops*_mapping_size],
               last_size);
        _mapped_region.flush(_flush_offset, last_size);
        _flush_offset = last_size;
    }
    _actual_offset += 1;
    return true;
}

bool io::MMapOutputStream::MappingHandler::empty_mapping()
{
    reset_vars();
    _mapped_region = bip::mapped_region();
    _mapped_file = bip::file_mapping();
    return true;
}