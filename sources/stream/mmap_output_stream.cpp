#include "stream/mmap_output_stream.hpp"

io::MMapOutputStream::MMapOutputStream(std::uint32_t mapping_size)
    : _mapping_size(mapping_size)
{}

io::MMapOutputStream::~MMapOutputStream()
{}

bool io::MMapOutputStream::create(std::string const& path)
{
    _file_name = path;
    try
    {
        _mapping_handler = MappingHandler(_file_name, _mapping_size);
        if (_mapping_handler.is_created())
            _mapping_handler.set_init();

        return _mapping_handler.is_created();
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

bool io::MMapOutputStream::writeln(std::string str)
{
    if (_mapping_handler.is_init())
        return _mapping_handler.writeln_text(str.c_str());
    else
        return false;
}

bool io::MMapOutputStream::close()
{
    return _mapping_handler.empty_mapping();
}


//-------------------------------------------------------------------
// Private class MappingHandler method definitions
//-------------------------------------------------------------------

io::MMapOutputStream::MappingHandler::MappingHandler(std::string& file,
                                                    std::uint32_t mapping_size)
    : _file_name(file.c_str()), _mapping_size(mapping_size)
{
    _is_created = create_file();
    _mapped_file = bip::file_mapping(_file_name, _mode);
    reset();
    _page_size = _mapped_region.get_page_size();
}

void io::MMapOutputStream::MappingHandler::set_init()
{
    _is_init = true;
}

bool io::MMapOutputStream::MappingHandler::is_init()
{
    return _is_init;
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
        fbuf.sputc(0);
        fbuf.close();
        return true;
    }
    else
        return false;
}

bool io::MMapOutputStream::MappingHandler::is_created()
{
    return _is_created;
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
    return true;
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
    uint32_t length = strlen(text);
    uint32_t first_size = 0; // In case _flush_offset is not zero
    uint32_t last_size = 0; // In case length is not multiple of _mapping_size
    uint32_t complete_loops = 0; // Number of full memcpy to perform.
    if ( _mapping_size - _flush_offset < length )
    {
		std::cout << "HelloFirst" << std::endl; // TODO
        first_size = _mapping_size - _flush_offset; // If 0, remap occurs.
        last_size = (length - first_size) % _mapping_size;
        complete_loops = (length - first_size - last_size) / _mapping_size;
    }
    else
	{
		std::cout << "HelloSecond" << std::endl; // TODO
        first_size = length;
	}
        //first_size = length - _mapping_size - (_flush_offset+1);

    { // Increase the size of the file to allow mapping. It has its own scope.
		std::cout << "Increase size START" << std::endl; // TODO
        std::filebuf fbuf;
        fbuf.open(_file_name, std::ios_base::in | std::ios_base::out);
        fbuf.pubseekoff(_actual_offset + length + 1, std::ios_base::beg);
        fbuf.sputc(0);
        //fbuf.sputc('\n'); // TODO
        fbuf.close();
		std::cout << "Increase size END" << std::endl; // TODO
    }

	std::cout << "YoSTART" << std::endl; // TODO
    if (first_size > 0)
    {
		std::cout << "Yo1" << std::endl; // TODO
		memcpy(_address, &text, first_size);
		std::cout << "Yo1_memcpied" << std::endl; // TODO
        _mapped_region.flush(_flush_offset, first_size);
        _flush_offset += first_size;
    }

    for (int i = 0; i < complete_loops; i++)
    {
		std::cout << "Flush Offset " <<_flush_offset << std::endl; // TODO
		std::cout << "Mapping size " <<_mapping_size << std::endl; // TODO
		std::cout << "Length: "<< length << std::endl; // TODO
		std::cout << "Yo2" << std::endl; // TODO
        next_mapping();
		std::cout << "Yo2_remapped" << std::endl; // TODO
        memcpy(_address,
               &text[first_size + i*_mapping_size],
               _mapping_size);
		std::cout << "Yo2_memcpied" << std::endl; // TODO
        _mapped_region.flush(_flush_offset, _mapping_size);
        _flush_offset += _mapping_size; // FIXME line was added
    }

    if (last_size > 0)
    {
		std::cout << "Yo3" << std::endl; // TODO
        next_mapping();
		std::cout << "Yo3_remapped" << std::endl; // TODO
		std::cout << "Length: "<< length << std::endl; // TODO
		std::cout << "Flush Offset: " <<_flush_offset << std::endl; // TODO
		std::cout << "Mapping size: " <<_mapping_size << std::endl; // TODO
		std::cout << "Last size: "<< last_size << std::endl; // TODO
        memcpy(_address,
               &text[first_size + complete_loops*_mapping_size],
               last_size); // FIXME
		std::cout << "Yo3_memcpied" << std::endl; // TODO
        _mapped_region.flush(_flush_offset, last_size);
        _flush_offset += last_size;
    }
	std::cout << "YoEND" << std::endl; // TODO
    //_actual_offset += 1; // TODO: why was this here?
	*_address += _flush_offset; // FIXME
		
    return true;
}

bool io::MMapOutputStream::MappingHandler::empty_mapping()
{
    reset_vars();
    _mapped_region = bip::mapped_region();
    _mapped_file = bip::file_mapping();
    return true;
}
