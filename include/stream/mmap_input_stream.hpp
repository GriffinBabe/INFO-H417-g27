#pragma once
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "stream/input_stream.hpp"

namespace bip = boost::interprocess;

namespace io {
class MMapInputStream: public InputStream {
public:
    MMapInputStream() = default;

    MMapInputStream(std::uint16_t read_size = 10,
                    std::uint16_t buffer_size = 10);

    virtual ~MMapInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const = 0;

private:
    bip::file_mapping _mapped_file;
    bip::mapped_region _mapped_region;


    std::uint16_t _read_size = 10;
    std::uint16_t _buffer_size = 10;
};
}
