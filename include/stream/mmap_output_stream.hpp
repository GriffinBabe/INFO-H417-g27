#pragma once
#include <boost/interprocess/file_mapping.hpp>
#include "stream/output_stream.hpp"

namespace io {
class MMapOutputStream: public OutputStream {
public:
    MMapOutputStream() = default;

    virtual ~MMapOutputStream();

    MMapOutputStream(std::uint16_t buffer_size);

    virtual bool create(std::string const& path) override;

    virtual bool writeln(std::string str) override;

    virtual bool close() override;

private:
/**
* Character write buffer wrapper
*/

    std::uint16_t _buffer_size = 10;

    FILE* _file = nullptr;

    bool _file_open = false;
};
}