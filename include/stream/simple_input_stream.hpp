#include "input_stream.hpp"

namespace io {
class SimpleInputStream : public InputStream {
public:
    SimpleInputStream() = default;
    ~SimpleInputStream();

    virtual bool open(std::string const& file) override;

    virtual std::shared_ptr<char[]> readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:
    /**
     * File descriptor.
     */
    FILE* _file = nullptr;

    /**
     * The base size allocation for the buffer in the read() function.
     */
    std::uint16_t buffer_size;
};
} // namespace io
