#include "length.hpp"

namespace tl {
class length_buffered : public length {
public:
    length_buffered() = default;
    virtual bool count(const std::string& path) override;
    virtual void output(uint16_t* count, std::string* duration ) override;
    const std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_buffered_length.txt"; //temp
};
}
