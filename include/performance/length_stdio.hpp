#include "length.hpp"
#include <resources_config.hpp>
#include <string>
#ifndef INFO_H417_LENGTH_STDIO_H
#define INFO_H417_LENGTH_STDIO_H
namespace tl {
class length_stdio : public length {
    virtual ~length_stdio();
    virtual bool count(const std::string& path) ;
    virtual bool output(uint16_t* count, std::string* duration );
    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_stdio_length.txt"; //temp
};
}
#endif // INFO_H417_LENGTH_STDIO_H
