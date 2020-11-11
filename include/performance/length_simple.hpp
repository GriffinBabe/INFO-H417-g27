#include "length.hpp"
#include <resources_config.hpp>
#include <string>

namespace tl {
class length_simple : public length {
public:
    length_simple() = default;
    virtual bool count(const std::string& path) override;
    virtual void output(uint16_t* count, std::string* duration ) override;
    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_simple_length.txt"; //temp
};
}
