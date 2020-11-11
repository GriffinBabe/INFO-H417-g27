#include "length.hpp"
#include <resources_config.hpp>
#include <string>
#ifndef INFO_H417_LENGTH_STDIO_H
#define INFO_H417_LENGTH_STDIO_H
namespace tl {
class length_stdio : public length {
public:
    length_stdio() = default;
    virtual bool count(const std::string& path) override;
    virtual void output(uint16_t* count, std::string* duration ) override;
    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_stdio_length.txt"; //temp
};
}
#endif // INFO_H417_LENGTH_STDIO_H
