#include "merge/entry.hpp"
#include <boost/algorithm/string.hpp>
#include <locale>

merge::Entry::Entry(std::string const& line)
{
    parse_from(line);
}

void merge::Entry::parse_from(const std::string& line)
{
    // splits the line by comma and puts it in the _elements file
    boost::split(_elements, line, boost::is_any_of(","));
    _rows = _elements.size();
}

std::uint32_t merge::Entry::size() const
{
    std::uint32_t size = 0;
    for (auto const& st : _elements) {
        size += st.size(); // length of the string in bytes.
    }
    return size;
}

std::string merge::Entry::to_string() const
{
    std::stringstream ss;
    for (int i = 0; i < _elements.size(); i++) {
        ss << _elements[i];
        if (i != _elements.size()) {
            ss << ",";
        }
    }

    return ss.str();
}

int merge::Entry::compare(int field, const Entry& other) const
{
    // get reference
    auto const& this_field = _elements.at(field);
    auto const& other_field = other._elements.at(field);

    // check if number or string
    bool is_number = true;
    std::string::const_iterator it = this_field.begin();
    while (it != this_field.end()) {
        if (!std::isdigit(*it)) {
            is_number = false;
            break;
        }
        it++;
    }

    if (is_number) {
        // compare number
        int this_int = std::atoi(this_field.c_str());
        int other_int = std::atoi(this_field.c_str());
        return this_int < other_int ? -1 : this_int == other_int ? 0 : 1;
    }
    else {
        // compare string
        return this_field < other_field ? -1 : this_field == other_field ? 0: 1;
    }
}