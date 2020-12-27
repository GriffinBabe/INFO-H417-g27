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
	return _mapping_handler.seek(pos);
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

void io::MMapInputStream::MappingHandler::check_eof()
{
	if (_cursor + _actual_offset >= _file_size )
		_eof = true;
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
    bool first_pass = true; // for the do-while, != behavior if true
    bool is_found = false; // indicate if found the character c
    char* char_ptr = nullptr; // pointer to the address of c
	char* content = nullptr;
	char* char_temp = nullptr;
	uintmax_t content_size = 0;
	uintmax_t previous_size = 0;
	uintmax_t added_size = 0;

    // Search in the mapping of size _mapping_size if the character is present
    do
    {
        // If it is not the first pass, do not need to care about backup_cursor
        if (!first_pass)
		{
            char_ptr = static_cast<char*>(
                std::memchr(_address, c, _mapping_size));
			if (char_ptr != nullptr) // Char is found
			{
				is_found = true;
			}
			else
			{ // Char is not found
				if ( next_mapping() ) // If mapping the next region is possible
				{// Neither the first mapped region nor the last
					added_size = _mapping_size;
					char_temp = new char[content_size];
					std::memcpy(char_temp, content, content_size);
					delete[] content;
					previous_size = content_size;
					content_size += added_size;
					content = new char[content_size];
					std::memcpy(content, char_temp, previous_size);
					delete[] char_temp;
					memset(&(content[content_size-1]), '\0', 1);
					std::memcpy(&(content[previous_size]),
								_address,
								added_size);
				}
				else // Otherwise stop here
					break;
			}
		}
        else
        { // If it is the first pass, take the _cursor into account
            char_ptr = static_cast<char*>(
                std::memchr(_address + _cursor,
                            c, _mapping_size - _cursor));
			if (char_ptr != nullptr) // Char is found
			{
				is_found = true;
			}
			else
			{
				if ( next_mapping() ) // If mapping next region is possible
				{// If the first mapped region but not the last
					added_size = _mapping_size - _cursor;
					added_size += 1;
					content_size += added_size;
					content = new char[content_size];
					memset(&(content[content_size-1]), '\0', 1);
					std::memcpy(content,
						   _address + _cursor,
						   content_size-1);
					first_pass = false;
				}
				else // Otherwise stop here
					break;
			}
        }

    } while (not is_found);

    // Compute the position of the end cursor depending on the char pointer
	// that was found and the address of the actual mapping
    uintmax_t end_cursor = 0; // offset of desired char in last mapping
    if (char_ptr != nullptr)
        end_cursor = (uintmax_t) (char_ptr - _address);
    else
        end_cursor = _file_size - 1 - _actual_offset;

    // In case found the character outside the file mapped
    if ( _actual_offset + end_cursor > _file_size - 1)
    {
        end_cursor = _file_size - 1 - _actual_offset;
        is_found = false;
		_eof = true;
    }

    // end_cursor - 1 to avoid counting "\n" as a "past character"
    added_size =  end_cursor - _cursor;

	// If no chars were passed (instantaneously found a '\n'), then return
    if (content_size + added_size == 0)
    {
        _content = "";
        _cursor = _cursor + 1;
        check_remap();
        return true;
    }

	if ( first_pass )
	{// If the first and last mapped region
		added_size += 1;
		content_size += added_size;
		content = new char[content_size];
		memset(&(content[content_size-1]), '\0', 1);
		std::memcpy(content,
			   _address + _cursor,
			   content_size - 1);
	}
	else
	{ // If this is the last mapped region
		char_temp = new char[content_size];
		std::memcpy(char_temp, content, content_size);
		delete[] content;
		previous_size = content_size;
		content_size += end_cursor;
		content = new char[content_size];
		std::memcpy(content, char_temp, previous_size);
		delete[] char_temp;
		memset(&(content[content_size-1]), '\0', 1);
		std::memcpy(&(content[previous_size]),
			   _address,
			   end_cursor);
	}

	// Update the cursor if the character was found
    if (is_found)
        _cursor = end_cursor + 1;

	// Check if a remap will be necessary
    check_remap();

	// Check if the current cursor is at the end of the file
	check_eof();

	// Build the string instance and return it to _content
    _content = std::string(content);
	delete[] content;
    return is_found;
}

bool io::MMapInputStream::MappingHandler::seek(std::uint32_t pos)
{
	uintmax_t max_range = _actual_offset + _mapping_size;
	if ( pos >= _actual_offset and pos < max_range )
	{
		_cursor = pos - _actual_offset;
		return true;
	}
	else
	{
		return remap(pos);
	}
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
