#pragma once
#include <memory>
#include <string>

namespace io {

/**
 * Base abstract class for all the input streams.
 * Input streams read text files line by line.
 */
class InputStream {
public:
    /**
     * Opens a file at the given path.
     * @param file, the path to the file to read.
     * @return true if the file was open successfully, false otherwise.
     */
    virtual bool open(std::string const& file) = 0;

    /**
     * Reads the next line of an opened file.
     * @return a smart pointer to the read char array.
     * @throws std::runtime_error if the file hasn't been initialized yet.
     */
    virtual std::shared_ptr<char> readln() = 0;

    /**
     * Moves the file cursor to a certain position, so the next readln reads
     * from that position to the end of the line.
     * @oaram pos, the position where to put the cursor
     * @return true if the cursor was successfully moved, false otherwise.
     */
    virtual bool seek(std::uint32_t pos) = 0;

    /**
     * Checks if we reached the end of stream (file).
     * @return true if we reached the end of the stream, false otherwise.
     */
    [[nodiscard]] virtual bool end_of_stream() const = 0;
    // nodiscard makes the compiler check if the returned value of the function
    // is at least used/compared once. (in this case the value is a boolean)
    // nodiscard is a good practice for getters, as we ensure that no getter
    // functions are called without purpose.

private:
protected:
};

} // namespace io
