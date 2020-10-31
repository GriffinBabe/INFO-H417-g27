#include "stream/mmap_input_stream.hpp"


io::MMapInputStream::MMapInputStream(std::uint16_t mapping_size)
    : _mapping_size(mapping_size)
{}

io::MMapInputStream::~MMapInputStream()
{
    // TODO
}

bool io::MMapInputStream::open(std::string const& file)
{
    _file_name = file;
    _mapping_handler = MappingHandler(_file_name,
                                    _mapping_size);
}

std::string io::MMapInputStream::readln()
{
    _mapping_handler.read_until_char('\n');
    std::string text = _mapping_handler.get_content();
    _mapping_handler.reset_content();
    return text;
}

bool io::MMapInputStream::seek(std::uint32_t pos)
{
    return _mapping_handler.remap(pos);
}

bool io::MMapInputStream::end_of_stream() const
{
    return _mapping_handler.eof();
}


//-------------------------------------------------------------------
// Private class MappingHandler method definitions
//-------------------------------------------------------------------

io::MMapInputStream::MappingHandler::MappingHandler(std::string& file,
                                                    std::uint16_t mapping_size)
    : _file_name(file.c_str()), _mapping_size(mapping_size*sizeof(char))
{
    _file_size = sfs::file_size(_file_name);
    _mapped_file = bip::file_mapping(_file_name, _mode);
    _mapped_region = bip::mapped_region(_mapped_file,
                                        _mode,
                                        0,
                                        _mapping_size);
    _page_size = _mapped_region.get_page_size();
    _address = static_cast<char*>(_mapped_region.get_address());
    _mapped_size = _mapped_region.get_size();
    _content[_mapping_size+1] = {'\0'};
}

bool io::MMapInputStream::MappingHandler::next_mapping()
{
    return remap(_actual_offset + _mapping_size);
}

bool io::MMapInputStream::MappingHandler::remap(uintmax_t offset)
{
    if (offset <= _file_size - 1)
    {
        _cursor = 0;
        _actual_offset = offset;
        _mapped_region = bip::mapped_region(_mapped_region,
                                            _mode,
                                            _actual_offset,
                                            _mapping_size);
        _address = static_cast<char*>(_mapped_region.get_address());
        _mapped_size = _mapped_region.get_size();
        _eof = false;
        return EXIT_SUCCESS;
    }
    else
    {
        _eof = true;
        return EXIT_FAILURE;
    }
}

void io::MMapInputStream::MappingHandler::reset()
{
    _actual_offset = 0;
    _cursor = 0;
    _mapped_region = bip::mapped_region(_mapped_file,
                                        _mode,
                                        _actual_offset,
                                        _mapping_size);
    _address = static_cast<char*>(_mapped_region.get_address());
    _mapped_size = _mapped_region.get_size();
    _eof = false;
}


bool io::MMapInputStream::MappingHandler::read_until_char(char c)
{
    if (_need_remap)
    {
        if ( !next_mapping() )
            return false;
    }
    else if (_eof)
        return false;

    int loop_ctr = 1; // one loop will be executed anyway
    uint16_t backup_cursor = _cursor;
    uintmax_t backup_offset = _actual_offset;
    bool first_pass = true;
    bool is_success = true;
    char* char_ptr = nullptr;

    do
    {
        if (!first_pass)
            char_ptr = static_cast<char*>(
                memchr(_address, c, _mapping_size));
        else
        {
            char_ptr = static_cast<char*>(
                memchr(_address,c,_mapping_size - backup_cursor));
            first_pass = false;
        }

        if (char_ptr != nullptr)
            break;
        else
        {
            if ( next_mapping() )
                loop_ctr++;
            else
            {
                is_success = false;
                break;
            }
        }
    } while (!first_pass);

    const int end_cursor = char_ptr - _address;
    const uintmax_t past_chars = (_actual_offset + end_cursor)
                                 - (backup_offset + backup_cursor);
    _content[past_chars+1] = {'\0'};
    remap(backup_offset);
    for (int i = 0; i < loop_ctr; i++)
    {
        if ( i != 0 && i != loop_ctr - 1)
        {
            memcpy(&_content[i * _mapping_size], _address, _mapping_size);
            next_mapping();
        }
        else if ( i == 1 )
        {
            memcpy(&_content[i * _mapping_size],
                   _address,
                   _mapping_size - backup_cursor);
            next_mapping();
        }
        else
        {
            memcpy(&_content[i*_mapping_size],
                   _address,
                   end_cursor);
        }
    }
    if (is_success)
        _cursor = end_cursor + 1;

    if (_cursor + _actual_offset >= _file_size - 1 )
        _eof = true;

    return is_success;
}

const char* io::MMapInputStream::MappingHandler::get_content()
{
    return _content;
}

void io::MMapInputStream::MappingHandler::reset_content()
{
    _content[_mapping_size+1] = {'\0'};
}

bool io::MMapInputStream::MappingHandler::eof() const
{
    return _eof;
}
