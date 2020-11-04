#include "stream/mmap_input_stream.hpp"


io::MMapInputStream::MMapInputStream(std::uint16_t mapping_size)
    : _mapping_size(mapping_size)
{}

io::MMapInputStream::~MMapInputStream()
{}

bool io::MMapInputStream::open(std::string const& file)
{
    _file_name = file;
    try
    {
        _mapping_handler = MappingHandler(_file_name, _mapping_size);
        return true;
    } catch (const bip::interprocess_exception& exception )
    {
        /*
        throw std::runtime_error(
            exception.what());
        */
        return false;
    } catch (...)
    {
        return false;
    }
}

std::string io::MMapInputStream::readln()
{
    std::string text = std::string();
    if (_mapping_handler.read_until_char('\n'))
    {
        text = _mapping_handler.get_content();
        _mapping_handler.reset_content();
    }
    //TODO
    //std::string text = _mapping_handler.get_content();
    //_mapping_handler.reset_content();
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
    //_mapped_region = bip::mapped_region(_mapped_file,
    //                                    _mode,
    //                                    _actual_offset,
    //                                    _mapping_size);
    //_address = static_cast<char*>(_mapped_region.get_address());
    //_mapped_size = _mapped_region.get_size();
    reset(); // TODO
    _page_size = _mapped_region.get_page_size();
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
        _mapped_region = bip::mapped_region(_mapped_file,
                                            _mode,
                                            _actual_offset,
                                            _mapping_size);
        _address = static_cast<char*>(_mapped_region.get_address());
        _mapped_size = _mapped_region.get_size();
        //reset_content();
        _need_remap = false;
        if (offset + _mapping_size < _file_size -1)
            _eof = false;
        else
            _eof = true;
        return true;
    }
    else
    {
        _eof = true;
        return false;
    }
}

void io::MMapInputStream::MappingHandler::reset_vars()
{
    _actual_offset = 0;
    _cursor = 0;
    _need_remap = false;
    _eof = false;
    reset_content();
}

void io::MMapInputStream::MappingHandler::reset()
{
    reset_vars();
    _mapped_region = bip::mapped_region(_mapped_file,
                                        _mode,
                                        _actual_offset,
                                        _mapping_size);
    _address = static_cast<char*>(_mapped_region.get_address());
    _mapped_size = _mapped_region.get_size();
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
    bool found = false;
    bool is_success = true;
    char* char_ptr = nullptr;

    int t_flag = 0; // TODO : remove the flag
    int t_size = 0; // TODO : remove
    t_size = _mapped_size; // TODO : remove after debugging

    do
    {
        if (!first_pass)
            char_ptr = static_cast<char*>(
                std::memchr(_address, c, _mapping_size));
        else
        {
            //char_ptr = static_cast<char*>(
            //    memchr(_address,c,_mapping_size - backup_cursor));
            char_ptr = static_cast<char*>(
                std::memchr(_address + backup_cursor,
                            c, _mapping_size - backup_cursor));
            first_pass = false;
        }

        if (char_ptr != nullptr)
            found = true;
        else
        {
            if ( next_mapping() )
                loop_ctr++;
            else
            {
                t_flag = 1; // TODO : remove the flag
                is_success = false;
                break;
            }
        }
    } while (not first_pass and not found);

    int end_cursor = 0;
    if (char_ptr == _address)
        return false;
    else if (char_ptr != nullptr)
        end_cursor = char_ptr - _address;
    else
    {
        end_cursor = _file_size - 1 - _actual_offset;
    }

    if (end_cursor - 1 == 0) // in case we encounter a "\n{1,}" situation
            return false; // TODO : get opinion about this

    // In case found the character outside the _mapped_region (page alignment)
    if ( _actual_offset + end_cursor > _file_size - 1)
    {
        end_cursor = _file_size - 1 - _actual_offset;
        is_success = false;
    }

    // end_cursor - 1 to avoid counting "\n" as a "past character"
    const uintmax_t past_chars = (_actual_offset + end_cursor - 1)
                                 - (backup_offset + backup_cursor);
    char content[past_chars+1];
    remap(backup_offset);
    for (int i = 0; i < loop_ctr; i++)
    {
        if ( i != 0 && i != loop_ctr - 1)
        {
            memcpy(&content[i * _mapping_size], _address, _mapping_size);
            next_mapping();
        }
        else if ( i == 0 )
        {
            memcpy(&content[i * _mapping_size],
                   _address + backup_cursor,
                   _mapping_size - backup_cursor);
            next_mapping();
        }
        else
        {
            memcpy(&content[i*_mapping_size],
                   _address,
                   end_cursor);
        }
    }
    if (is_success)
        _cursor = end_cursor + 1;

    if (_cursor > _mapping_size - 1 )
        _need_remap = true;

    _content = std::string(content);
    return is_success;
}

/** TODO
const char* io::MMapInputStream::MappingHandler::get_content()*/
std::string io::MMapInputStream::MappingHandler::get_content()
{
    return _content;
}

void io::MMapInputStream::MappingHandler::reset_content()
{
    _content = std::string();
    //_content[_mapping_size+1] = {'\0'};
}


bool io::MMapInputStream::MappingHandler::eof() const
{
    return _eof;
}
