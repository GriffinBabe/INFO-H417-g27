#include <chrono>
#include <iostream>
#include <stdio.h>
#include "resources_config.hpp"

namespace tl {
class length_seq_read {
public:
    length_seq_read() = default;
    virtual bool count(const std::string& path,const std::string& fonction);
    virtual void output(uint16_t* count, std::string* duration );
    std::string output_file_path = "" ;
};
}

