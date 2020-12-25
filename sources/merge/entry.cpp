#include "merge/entry.hpp"
#include <boost/algorithm/string.hpp>

void merge::Entry::parse_from(const std::string& line, Header& header)
{
    _header = &header;
    // if the header hasn't been set yet then
    if (!_header->is_set) {
        _header->set_header(line);
    }
    // splits the line by comma and puts it in the _elements file
    // http://www.cplusplus.com/forum/general/235644/
    std::string::size_type pos = 0, last_pos = 0, length = line.length();

    using value_type = typename std::vector<std::string>::value_type;
    using size_type = typename std::vector<std::string>::size_type;

    while (last_pos < length + 1) {
        pos = line.find_first_of(',', last_pos);
        if (pos == std::string::npos) {
            pos = length;
        }

        if (pos != last_pos) {
            _elements.push_back(
                value_type(line.data() + last_pos, (size_type) pos - last_pos));
        }
        last_pos = pos + 1;
    }

    _rows = _elements.size();
}

std::uint32_t merge::Entry::size() const
{
    std::uint32_t size = 0;
    for (auto const& str : _elements) {
        size += str.size();
    }
    return size;
}

std::string merge::Entry::to_string() const
{
    std::string this_string;
    this_string.reserve(this->size());
    for (auto const& str : _elements) {
        this_string += str;
    }
    return this_string;
}

bool merge::Entry::compare(int field, const Entry& other) const
{
    // get reference
    auto const& this_field = _elements.at(field);
    auto const& other_field = other._elements.at(field);

    if (_header->is_int[field]) {
        int fst = std::atoi(this_field.c_str());
        int snd = std::atoi(other_field.c_str());
        return (fst < snd);
    }
    else {
        return (this_field < other_field);
    }
}

void merge::Header::set_header(const std::string& str)
{
    std::vector<std::string> strings;
    boost::split(strings, str, boost::is_any_of(","));

    for (auto const& str : strings) {
        bool is_number = true;
        auto it = str.begin();
        while (it != str.end()) {
            if (!std::isdigit(*it)) {
                is_number = false;
                break;
            }
            it++;
        }

        is_int.push_back(is_number);
    }

    is_set = true;
}
