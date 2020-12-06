#include <boost/program_options.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <iostream>

#include <stream/buffered_input_stream.hpp>
#include <stream/input_stdio_stream.hpp>
#include <stream/mmap_input_stream.hpp>
#include <stream/simple_input_stream.hpp>

namespace po = boost::program_options;

enum StreamType { SIMPLE, STDIO, BUFFERED, MMAP };

/**
 * Type of used stream, specified in command line arguments, uses STDIO by
 * default if not specified.
 */
StreamType type = SIMPLE;

/**
 * BufferedInputStream buffer size or MMapInputStream mapped size.
 */
std::uint32_t buffered_map_size = 10;

/**
 * The input file path, specified in the command line arguments.
 */
std::string input_file;

/**
 * The number of times the operation of random jumping + readln() is repeated
 */
std::uint32_t jump_numbers = 0;

std::string program_description =
    "Jumps into a random position of the input-file and reads a line, this "
    "operation is repeated jump-numbers times";

std::string usage = "Usage: randjump [--help] input-file jump-numbers";

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-file", 1);
        pos.add("jump-numbers", 1);

        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message.")(
            "simple,s", "will use the SimpleInputStream.")(
            "fgets,f", "will use the StdioInputStream.")(
            "map,m",
            po::value<int>(),
            "mapping size, if specified will use a MMapInputStream.")(
            "buffer,b",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedInputStream.")(
            "input-file", po::value<std::string>(), "input csv file.")(
            "jump-numbers",
            po::value<int>(),
            "number of times to repeat the operation.");

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(pos)
                      .run(),
                  vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << usage << std::endl;
            std::cout << program_description << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        if (vm.count("simple")) {
            type = SIMPLE;
        }

        if (vm.count("fgets")) {
            type = STDIO;
        }

        if (vm.count("buffer")) {
            type = BUFFERED;
            buffered_map_size = vm["buffer"].as<int>();
        }

        if (vm.count("map")) {
            type = MMAP;
            buffered_map_size = vm["map"].as<int>();
        }

        if (!vm.count("input-file") || !vm.count("jump-numbers")) {
            std::cerr << usage << std::endl;
            return 1;
        }

        input_file = vm["input-file"].as<std::string>();
        jump_numbers = vm["jump-numbers"].as<int>();
    }
    catch (boost::bad_any_cast const& exc) {
        std::cerr << usage << std::endl;
        std::cerr << "Bad parameter type: " << exc.what() << std::endl;
        return 1;
    }
    catch (po::error const& exc) {
        std::cerr << usage << std::endl;
        std::cerr << "Exeption while parsing arguments: " << exc.what()
                  << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    int ret_status = parse_arguments(argc, argv);
    if (ret_status != 0) {
        return ret_status;
    }

    std::cout << "[====== Rand Jump =====]" << std::endl;

    std::uint32_t sum = 0;
    std::vector<std::uint32_t> positions;

    // get the file size
    FILE* file = fopen(input_file.c_str(), "r");
    if (file == NULL) {
        std::cerr << "Couldn't open file: " << input_file << std::endl;
        return 1;
    }
    fseek(file, 0L, SEEK_END);
    std::uint32_t size = ftell(file);
    if (fclose(file) != 0) {
        std::cerr << "Couldn't properly close file." << std::endl;
    }

    std::cout << "File size is: " << size << "." << std::endl;

    // initializes all the random positions
    boost::random::random_device dev;
    boost::random::mt19937 rng(dev());
    boost::uniform_int<> dist(0, size);
    boost::variate_generator<boost::mt19937, boost::uniform_int<>> dice(rng,
                                                                        dist);
    for (std::uint32_t count = 0; count < jump_numbers; count++) {
        int x = dice();
        positions.push_back(x);
    }

    // initializes the specified stream.
    std::shared_ptr<io::InputStream> stream;

    switch (type) {
    case SIMPLE:
        std::cout << "Using SimpleInputStream." << std::endl;
        stream = std::make_shared<io::SimpleInputStream>();
        break;
    case STDIO:
        std::cout << "Using StdioInputStream." << std::endl;
        stream = std::make_shared<io::StdioInputStream>();
        break;
    case BUFFERED:
        std::cout << "Using BufferedInputStream." << std::endl;
        stream = std::make_shared<io::BufferedInputStream>(buffered_map_size,
                                                           buffered_map_size);
        break;
    case MMAP:
        std::cout << "Using MMapInputStream." << std::endl;
        stream = std::make_shared<io::MMapInputStream>(buffered_map_size);
        break;
    }

    assert(stream->open(input_file));

    // starts the process
    for (std::uint32_t count = 0; count < jump_numbers; count++) {
        stream->seek(positions[count]);
        std::string line = stream->readln();
        sum += line.size();
    }

    std::cout << "Sum of read characters: " << sum << std::endl;

    return 0;
}
