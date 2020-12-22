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
	std::cout<<"Welcome"<<std::endl; // TODO
	uint32_t offset = _actual_offset;
    uint32_t length = strlen(text);
    uint32_t first_size = 0; // In case _flush_offset is not zero
    uint32_t last_size = 0; // In case length is not multiple of _mapping_size
    uint32_t complete_loops = 0; // Number of full memcpy to perform.
    if ( _mapping_size - _flush_offset < length )
    { // If the content cannot fit in the actual mapping: 3 parts
        first_size = _mapping_size - _flush_offset; // If 0, remap occurs.
        last_size = (length - first_size) % _mapping_size; // What exceeds
        complete_loops = (length - first_size - last_size) / _mapping_size;
		// Number of loops using the complete mapping size
    }
    else
	{ // All fit in the current mapping
        first_size = length;
	}

	if ( _need_remap ) // FIXME
		offset += _mapping_size; // FIXME

	std::cout<<"HelloFILEBUF"<<std::endl; // TODO
    { // Increase the size of the file to allow mapping. It has its own scope.
        std::filebuf fbuf;
        fbuf.open(_file_name, std::ios_base::in | std::ios_base::out);
        fbuf.pubseekoff(offset + _flush_offset + (length-1) + 1,
						std::ios_base::beg); // + 1 is for the \n character
		std::cout<<"End of file: "<<_actual_offset + _flush_offset + length <<std::endl; // TODO
        fbuf.sputc(0);
        fbuf.close();
    }
	std::cout<<"ByeFILEBUF"<<std::endl; // TODO

	if ( _need_remap ) // FIXME
	{
		next_mapping();
		_need_remap = false;
	}

	std::cout<<"Hello1"<<std::endl; // TODO
    if (first_size > 0)
    { // Copy everything that will fit in the current mapping
		std::cout<<"Flush offset: "<<_flush_offset<<std::endl; // TODO
		std::cout<<"Mapping size: "<<_mapping_size<<std::endl; // TODO
		std::cout<<"First size: "<<first_size<<std::endl; // TODO
		std::cout<<"Length: "<<length<<std::endl; // TODO
		memcpy(_address, text, first_size);
		std::cout<<"MEMCPY OK"<<std::endl; // TODO
		if ( last_size or complete_loops) // Will only flush if region is full
			//_mapped_region.flush(_flush_offset, first_size); // FIXME
			_mapped_region.flush(_actual_offset, _mapping_size);
			
        _flush_offset += first_size;
		_address += first_size;
    }

	std::cout<<"Hello2"<<std::endl; // TODO
    for (int i = 0; i < complete_loops; i++)
    { // Copy what exceeded the previous mapping and require full mapping size
        next_mapping();
        memcpy(_address,
               &(text[first_size + i*_mapping_size]),
               _mapping_size);
        //_mapped_region.flush(_flush_offset, _mapping_size); // FIXME
        _mapped_region.flush(_actual_offset, _mapping_size);
        _flush_offset += _mapping_size;
		_address += _mapping_size;
    }

	std::cout<<"Hello3"<<std::endl; // TODO
    if (last_size > 0)
    { // Copy what exceeded previous mappings but not feeling a whole region
        next_mapping();
        memcpy(_address,
               &(text[first_size + complete_loops*_mapping_size]),
               last_size);
        //_mapped_region.flush(_flush_offset, last_size);
        _flush_offset += last_size; // We only flush last_size chars
		_address += last_size;
    }

	std::cout<<"Hello4"<<std::endl; // TODO
	*_address = '\n';
	std::cout<<"Hello4"<<std::endl; // TODO
	_address += 1;
	_flush_offset += 1; // We only flush last_size chars
	_flush_offset %= _mapping_size;

	std::cout<<"Hello5"<<std::endl; // TODO
	if (_flush_offset == 0) // FIXME
	{ // ALL is FIXME
        _mapped_region.flush(_actual_offset, _mapping_size);
		_need_remap = true;
	}
	std::cout<<"Hello6"<<std::endl; // TODO

	// FIXME if we do not increment actual offset
	// if flush offset will reach 0, we should flush with mapping size only
	//
	
	std::cout<<"Goodbye"<<std::endl; // TODO
		
    return true;
}

bool io::MMapOutputStream::MappingHandler::empty_mapping()
{
    reset_vars();
    _mapped_region = bip::mapped_region();
    _mapped_file = bip::file_mapping();
    return true;
}
