#include "stream/mmap_input_stream.hpp"


io::MMapInputStream::MMapInputStream(std::uint32_t mapping_size)
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
    if (_mapping_handler.is_open())
    {
        if (_mapping_handler.read_until_char('\n'))
        {
            text = _mapping_handler.get_content();
            _mapping_handler.reset_content();
        }
        else
        {

        }
    }
    else
    {
        throw std::runtime_error(
            "Tried to perform line read but the file is not opened yet.");
    }
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
                                                    std::uint32_t mapping_size)
    : _file_name(file.c_str()), _mapping_size(mapping_size)
{
    _file_size = sfs::file_size(_file_name);
    _mapped_file = bip::file_mapping(_file_name, _mode);
    reset();
    _page_size = _mapped_region.get_page_size();
    _file_open = true;
}

bool io::MMapInputStream::MappingHandler::is_open()
{
    return _file_open;
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

void io::MMapInputStream::MappingHandler::check_remap()
{
    if (_cursor > _mapping_size - 1 )
        _need_remap = true;
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
    // First, check if a remap is necessary
    if (_need_remap)
    {
        if ( !next_mapping() )
            return false;
    } // Check also if the end of file was reached
    else if (_eof)
        return false;

    // Initialize values that will be used
    int loop_ctr = 1; // one loop will be executed anyway
    uintmax_t backup_cursor = _cursor;
    uintmax_t backup_offset = _actual_offset;
    bool first_pass = true;
    bool found = false;
    bool is_success = true;
    char* char_ptr = nullptr;


    // Search in the mapping of size _mapping_size if the character is present
    do
    {
        // If it is not the first pass, do not need to care about backup_cursor
        if (!first_pass)
            char_ptr = static_cast<char*>(
                std::memchr(_address, c, _mapping_size));
        else
        { // If it is the first pass, take the backup_cursor into account
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
                is_success = false;
                break;
            }
        }
    } while (not first_pass and not found);

    // Compute the position of the end cursor depending on the char pointer
	// that was found and the address of the actual mapping
    uintmax_t end_cursor = 0; // offset of desired char in last mapping
    if (char_ptr != nullptr)
    {
        end_cursor = (uintmax_t) (char_ptr - _address);
    }
    else
    {
        end_cursor = _file_size - 1 - _actual_offset;
    }

	/*
    if (end_cursor - 1 == 0) // in case we encounter a "\n{1,}" situation
    {
        _content = "\n";
        return true; // TODO : get opinion about this
    }
	*/

    // In case found the character outside the file mapped
    if ( _actual_offset + end_cursor > _file_size - 1)
    {
        end_cursor = _file_size - 1 - _actual_offset;
        is_success = false;
    }

    // end_cursor - 1 to avoid counting "\n" as a "past character"
    const uintmax_t past_chars = (_actual_offset + end_cursor - 1)
                                 - (backup_offset + backup_cursor);

    char content[past_chars+1];
    content[past_chars] = '\0';
	if (loop_ctr > 1)
		remap(backup_offset);

	_cursor = backup_cursor;

    if (past_chars == 0)
    {
        _content = "";
        _cursor = backup_cursor + 1;
        check_remap();
        return true;
    }

    int idx = 0;
    for (int i = 0; i < loop_ctr; i++)
    {
        if ( i != 0 && i != loop_ctr - 1)
        {
            std::memcpy(&content[idx],
                   _address,
                   _mapping_size);
            idx += _mapping_size;
            next_mapping();
        }
        else if ( i == 0 && loop_ctr != 1 )
        {
            std::memcpy(&content[idx],
                   _address + backup_cursor,
                   _mapping_size - backup_cursor);
            idx = _mapping_size - backup_cursor;
            next_mapping();
        }
        else if ( i == 0 && loop_ctr == 1)
        {
            std::memcpy(&content[idx],
                   _address + backup_cursor,
                   end_cursor - backup_cursor);
        }
        else
        {
            std::memcpy(&content[idx],
                   _address,
                   end_cursor);
        }
    }
    if (is_success)
        _cursor = end_cursor + 1;

    check_remap();

    _content = std::string(content);
    return is_success;
}

std::string io::MMapInputStream::MappingHandler::get_content()
{
    return _content;
}

void io::MMapInputStream::MappingHandler::reset_content()
{
    _content = std::string();
}


bool io::MMapInputStream::MappingHandler::eof() const
{
    return _eof;
}
