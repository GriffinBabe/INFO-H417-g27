#include <boost/program_options.hpp>
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
StreamType type = STDIO;

/**
 * BufferedInputStream buffer size or MMapInputStream mapped size.
 */
std::uint32_t buffered_map_size = 10;

/**
 * The input file path, specified in the command line arguments.
 */
std::string input_file;

std::string program_description =
    "Reads the input-file line by line with the specified stream (default is "
    "simple_stream). Increments a counter each line by the size of the read "
    "line, outputs it at the end of the operation.";

std::string usage = "Usage: length [--help] input-file";

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-file", 1);

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

        if (!vm.count("input-file")) {
            std::cerr << usage << std::endl;
            return 1;
        }

        input_file = vm["input-file"].as<std::string>();
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

    bool result = stream->open(input_file);
    assert(result);

    std::uint64_t sum = 0;

    std::chrono::time_point start = std::chrono::system_clock::now();
    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        if (!line.empty())
            sum += line.size();
    }
    std::chrono::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    std::cout << "Sum of read characters: " << sum << std::endl;
    std::cout << " ------ Duration: " << dur.count() << "ms." << std::endl;

    return 0;
}
