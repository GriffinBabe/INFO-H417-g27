#pragma once
#include "input_stream.hpp"
#include<stdio.h>

namespace io {
class StdioInputStream: public InputStream {
public:
    StdioInputStream() = default;

    virtual ~StdioInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;
private:

    /**
     * File descriptor.
     */
    FILE* _file = nullptr;

    /**
     * Store input string
     */
    char str [100];

    /**
     * Tracks if the file is open or not.
     */
    bool _file_open = false;

};
}

