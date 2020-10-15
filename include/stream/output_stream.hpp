#pragma once
#include <memory>

namespace io {

/**
 * Base abstract class for all the output streams.
 * Output streams write text files line by line.
 */
class OutputStream {
public:
    /**
     * Virtual destructor that will be overridden by child classes.
     */
    virtual ~OutputStream();

    /**
     * Creates a new file at the given path.
     * If the file already exists, it will overwrite it.
     * @param path, the path were to create the file.
     * @return true if the file was created successfully, false otherwise.
     */
    virtual bool create(std::string const& path) = 0;

    /**
     * Writes a line in the opened file. The end of line character
     * is written automatically added in the function.
     * @param string, the string to write. No need to have an EOL character at
     * the end.
     * @return true if the line has been wrote successfully, false otherwise.
     */
    virtual bool writeln(std::string str) = 0;

    /**
     * Closes the opened file. Returns true if the file was already closed but
     * emits a warning.
     * @return true if the file was successfully closed, false otherwise.
     */
    virtual bool close() = 0;

private:
protected:
};
} // namespace io