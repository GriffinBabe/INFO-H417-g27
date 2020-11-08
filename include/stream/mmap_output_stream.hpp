#pragma once
#include <fstream>
#include <filesystem>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "stream/output_stream.hpp"

namespace bip = boost::interprocess;
namespace sfs = std::filesystem;

namespace io {
class MMapOutputStream: public OutputStream {
public:
    MMapOutputStream() = default;

    virtual ~MMapOutputStream();

    MMapOutputStream(std::uint16_t mapping_size);

    virtual bool create(std::string const& path) override;

    virtual bool writeln(std::string str) override;

    virtual bool close() override;

private:
    class MappingHandler {
    public:
        MappingHandler() = default;

        MappingHandler(std::string& file,
                       std::uint16_t mapping_size = 10);

        /**
         * Initialize a file with the _file_name name.
         * @return bool, yes or no the operation was successful
         */
        bool create_file();

        /**
         * Tell if the file creation was correctly performed
         */
         bool is_created();

        /**
         * Map the next region of the file.
         * @return bool, if the next mapping was successful
         */
        bool next_mapping();

        /**
         * Map a region using the given offset.
         * @param offset to use
         * @return bool, if the remap was successful
         */
        bool remap(uintmax_t offset);

        /**
         * Reset variables to initial values.
         */
        void reset_vars();

        /**
         * Remap a region as done during the first initialisation.
         */
        void reset();

        /**
         * Write text in the file, end the writing operation with end of line
         * character.
         * @param text, what has to be written to file
         * @return bool, yes or no the operation was successful
         */
        bool writeln_text(const char *text);

        /**
         * Close all current mapping.
         */
         bool empty_mapping();

    private:
        /**
         * Name of the file to map.
         */
        const char* _file_name = nullptr;

        /**
         * Size of the file.
         */
        uintmax_t _file_size = 0;

        /**
         * Actual offset in the content of the file
         */
        uintmax_t _actual_offset = 0;

        /**
         * Size of the file to map in memory.
         */
        std::uint16_t _mapping_size = 10;

        /**
         * Give the number of character to flush in mapped file.
         */
        uint16_t _flush_offset = 0;

        /**
         * Mode of interaction with the mapped file.
         */
        bip::mode_t _mode = bip::read_write;

        /**
         * Wraps a file-mapping used to create mapped regions
         * from the file to map
         */
        bip::file_mapping _mapped_file;

        /**
         * Part/region of the file-mapping to mapped into memory.
         */
        bip::mapped_region _mapped_region;

        /**
         * Real size of the mapped region
         */
        std::size_t _mapped_size;

        /**
         * Starting address of the region.
         */
        char* _address = nullptr;

        /**
         * Size of pages.
         */
        std::size_t _page_size;

        /**
         * Indicate if the file was created
         */
        bool _is_created = false;
    };


    /**
     * Size of the mapping to perform.
     */
    std::uint16_t _mapping_size = 10;

    /**
     * Name of the file to map and to read from.
     */
    std::string _file_name;

    /**
     * Object handling the mapping and reading procedures.
     */
    MappingHandler _mapping_handler;
};
}