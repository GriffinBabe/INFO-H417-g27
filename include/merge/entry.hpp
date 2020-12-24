#pragma once
#include <string>
#include <vector>

namespace merge {


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
     * Constructor that calls the parse_from function directly inside.
     * @param line
     */
    Entry(std::string const& line);

    /**
     * Parses all the entry data from a file line and sets them in the _elements
     * vector.
     * @param line, the line to parse.
     */
    void parse_from(std::string const& line);

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
    int compare(int field, Entry const& other) const;

private:

    /**
     * The number of elements of the entry.
     */
    std::uint16_t _rows = 0;

    /**
     * The data of the entry.
     */
    std::vector<std::string> _elements;

};

}