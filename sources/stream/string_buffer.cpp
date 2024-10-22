#include "stream/string_buffer.hpp"
#include <cstring>

util::StringBuffer::StringBuffer(std::uint32_t _base_allocation)
    : _allocated_size(_base_allocation)
{
    reallocate();
}

util::StringBuffer::StringBuffer()
{
    _allocated_size = 10;
    reallocate();
}

void util::StringBuffer::add(char c)
{
    if (_cursor >= _allocated_size) {
        reallocate();
    }
    _data.get()[_cursor++] = c;
}

void util::StringBuffer::add(char* c, int len)
{
    if (_cursor + len >= _allocated_size) {
        reallocate(_cursor + len);
    }
    memcpy(_data.get() + _cursor, c, sizeof(char) * len);
    _cursor += len;
}

void util::StringBuffer::reset()
{
    _cursor = 0;
}

std::string util::StringBuffer::get()
{
    std::string str(_cursor, '\0');
    std::memcpy(str.data(), _data.get(), sizeof(char) * _cursor);
    return str;
}

void util::StringBuffer::reallocate()
{
    _allocated_size *= 2;
    std::unique_ptr<char> tmp(new char[_allocated_size]);
    std::memcpy(tmp.get(), _data.get(), sizeof(char) * _cursor);
    std::swap(tmp, _data);
}

void util::StringBuffer::reallocate(std::uint32_t min_size)
{
    while (_allocated_size < min_size) {
        _allocated_size *= 2;
    }
    std::unique_ptr<char> tmp(new char[_allocated_size]);
    std::memcpy(tmp.get(), _data.get(), sizeof(char) * _cursor);
    std::swap(tmp, _data);
}
