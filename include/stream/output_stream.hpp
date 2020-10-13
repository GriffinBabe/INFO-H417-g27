#pragma once
#include <string>
#include <memory>

namespace io {

/**
 * Base abstract class for all the output streams.
 * Output streams write text files line by line.
 */
class OutputStream {
public:

    /**
     * Creates a new file at the given path.
     * @param path, the path were to create the file.
     * @return true if the file was created successfully, false otherwise.
     */
    virtual bool create(std::string const& path) = 0;

    /**
     * Writes a line in the opened file.
     * @param string, the string to write.
     * @return true if the line has been wrote successfully, false otherwise.
     */
    virtual bool writeln(std::shared_ptr<char[]> string) = 0;

    /**
     * Closes the opened file.
     * @return true if the file was successff-ully closed, false otherwise.
     */
    virtual bool close() = 0;

private:
protected:
};
} // namespace io