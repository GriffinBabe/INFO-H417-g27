#pragma once
#include <string>
#include <vector>
#include <variant>

namespace merge {

/**
 * A little utility structure that register if a row is a real string or can be
 * interpreted as an integer.
 */
struct Header {
    bool is_set = false;

    std::vector<bool> is_int;

    void set_header(std::string const& str);
};


/**
 * Represents an entry in the database. Is the data structure to store the file
 * data and to sort and merge.
 */
class Entry {

public:
    /**
     * Default constructor.
     */
    Entry() = default;

    /**
     * Parses all the entry data from a file line and sets them in the _elements
     * vector.
     * @param line, the line to parse.
     */
    void parse_from(std::string const& line, Header& header);

    /**
     * Get the size of the entry in bytes. This takes into account the size
     * of the strings.
     * @return
     */
    std::uint32_t size() const;

    /**
     * Converts the entry back to a string.
     * @return the string formatted entry.
     */
    std::string to_string() const;

    /**
     * Compares the field of this entry to another.
     * @param field, the field number to compare (k-th column)
     * @param other, reference to the other entry
     * @return -1 if this < other
     *          0 if this == other
     *          1 if this > other
     */
    bool compare(int field, Entry const& other) const;

private:

    /**
     * The number of elements of the entry.
     */
    std::uint16_t _rows = 0;

    /**
     * The data of the entry.
     */
    std::vector<std::string> _elements;

    /**
     * Reference to the header.
     */
    Header* _header = nullptr;

};

}