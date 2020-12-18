#pragma once
#include <filesystem>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "stream/input_stream.hpp"
#include <iostream> // TODO

namespace bip = boost::interprocess;
namespace sfs = std::filesystem;

namespace io {
class MMapInputStream: public InputStream {
public:
    MMapInputStream() = default;

    virtual ~MMapInputStream();

    MMapInputStream(std::uint32_t mapping_size = 10);

    virtual bool open(std::string const& file) override;

    virtual std::string readln() override;

    virtual bool seek(std::uint32_t pos) override;

    [[nodiscard]] virtual bool end_of_stream() const override;

private:
    class MappingHandler {
    public:
        MappingHandler() = default;

        MappingHandler(std::string& file,
                       std::uint32_t mapping_size = 10);

        /**
         * Return the value of _file_open: is the file open or not?
         * @return bool, yes or no to the previous question
         */
        bool is_open();

        /**
         * Map the next region of the file.
         * @return bool, if the next mapping was successful
         */
        bool next_mapping();

        /**
         * Map a region using the given offset
         * @param offset to use
         * @return bool, if the remap was successful
         */
         bool remap(uintmax_t offset);

         /**
          * Check if a remap will be necessary
          */
         void check_remap();

        /**
         * Reset variables to initial values.
         */
        void reset_vars();

        /**
         * Remap a region as done during the first initialisation.
         */
        void reset();

        /**
         * Read until the first instance of a character from the given 
		 * position.
         * @param c, char to search
         * @return bool, if the char was found
         */
        bool read_until_char(char c);

        /**
         * Return content stored in the char array _content.
         * Content is mainly fed using read_until_char method.
         * @return characters stored in _content char array
         */
        //const char* get_content();
        std::string get_content();

        /**
         * Reset the _content array.
         */
        void reset_content();

        /**
         * Indicate if reached end of stream.
         * @return bool, yes or no to the question: is end of stream reached?
         */
         [[nodiscard]] bool eof() const;

    private:
        /**
         * Name of the file to map.
         */
        const char* _file_name = nullptr;

        /**
         * Determine if the corresponding file is "open" or not
         */
         bool _file_open = false;

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
        std::uint32_t _mapping_size = 10;

        /**
        * Current read position in the mapped region.
        */
        std::uint32_t _cursor = 0;

        /**
         * Indicate if a remap will be necessary
         */
         bool _need_remap = false;


        /**
         * Mode of interaction with the mapped file.
         */
        bip::mode_t _mode = bip::read_only;

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
         * Indicate if end of stream is reached.
         */
         bool _eof = false;

        /**
         * Content read from _mapped_region, with size _read_size.
         * Mainly fed using read_until_char method.
         */
        //char _content[]; // TODO fix ? source for error in test?
        std::string _content;
    };

    /**
     * Size of the mapping to perform.
     */
    std::uint32_t _mapping_size = 10;

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
