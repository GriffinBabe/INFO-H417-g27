#include "output_stream.hpp"
#include<stdio.h>
#include <iostream>

namespace io {
class StdioOutputStream : public OutputStream {
public:
    StdioOutputStream() = default;

    virtual ~StdioOutputStream();

    virtual bool create(std::string const& path) override;

    virtual bool writeln(std::string str) override;

    virtual bool close() override;

private:
    FILE* _file = nullptr;
    bool _file_open = false;
};
}