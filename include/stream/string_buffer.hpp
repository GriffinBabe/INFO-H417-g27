#pragma once
#include <iostream>
#include <memory>

namespace util {

/**
 * Base reusable buffer used by streams in the readln() function.
 * A basic size is allocated, if the size is reached, then the double size is
 * reallocated. The buffer doesn't free at the end of the line read, it is
 * just reused.
 *
 * This class has been written because std::string, std::stringstream and
 * std::vector constructions and deletions are too slow for our usage.
 */
class StringBuffer {
public:
    StringBuffer();

    StringBuffer(std::uint32_t _base_allocation);

    /**
     * Copies a single character into the buffer. If the buffer is full, then it
     * reallocates it and doubles it's size until the size is big enough.
     * The old data is then copied to the buffer.
     *
     * @param c
     */
    void add(char c);

    /**
     * Copies a string of specified length to the buffer. If the buffer is full,
     * then it reallocates it and doubles it's size until the size is big enough.
     * The old data is then copied to the buffer.
     *
     * @param c
     * @param len
     */
    void add(char* c, int len);

    /**
     * Sets the cursor to the base position to reuse the buffer.
     */
    void reset();

    /**
     * Gets the buffered string in a std::string container.
     * @return the buffered string.
     */
    std::string get();

private:

    void reallocate();

    /**
     * Reallocates the buffer. The new allocated space is doubled.
     */
    void reallocate(std::uint32_t min_size);

    std::uint32_t _allocated_size = 0;

    std::uint32_t _cursor = 0;

    std::unique_ptr<char> _data;
};
} // namespace util
