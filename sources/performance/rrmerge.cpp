#include <boost/program_options.hpp>
#include <iostream>

#include <stream/buffered_input_stream.hpp>
#include <stream/input_stdio_stream.hpp>
#include <stream/mmap_input_stream.hpp>
#include <stream/simple_input_stream.hpp>

namespace po = boost::program_options;

enum StreamType { SIMPLE, STDIO, BUFFERED, MMAP };
// TODO -> select best input streams to use with X
// TODO Y will be any output stream

/**
 * Types of used streams, specified in command line arguments, uses STDIO by
 * default if not specified.
 */
StreamType input_type = STDIO;
StreamType output_type = STDIO;

/**
 * BufferedInputStream buffer size or MMapInputStream mapped size.
 */
std::uint32_t input_buffered_map_size = 10;
std::uint32_t output_buffered_map_size = 10;

/**
 * The input file paths, specified in the command line arguments.
 */
std::vector<std::string> input_files;

std::string program_description =
    "Merges the content of the input files by copying one line of each file "
	"in a round robin fashion, until all of them have been processed.";

std::string usage = "Usage: rrmerge [--help] input-stream [size] "
					"output-stream [size] input-files\n"
					"where there should be at least 2 input files";

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-files", -1);
		// - 1 => translate positional options into input-file options

        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message.")(
            "simple-input,si", "will use the SimpleInputStream.")(
            "fgets-input,fi", "will use the StdioInputStream.")(
            "map-input,mi",
            po::value<int>(),
            "mapping size, if specified will use a MMapInputStream.")(
            "buffer-input,bi",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedInputStream.")(
            "simple-output,so", "will use the SimpleOutputStream.")(
            "fgets-output,fo", "will use the StdioOutputStream.")(
            "map-output,mo",
            po::value<int>(),
            "mapping size, if specified will use a MMapOutputStream.")(
            "buffer-output,bo",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedOutputStream.")(
            "input-files", po::value<std::vector<std::string>>(),
			"input csv file.");

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

        if (vm.count("simple-input")) {
            input_type = SIMPLE;
        }

        if (vm.count("fgets-input")) {
            input_type = STDIO;
        }

        if (vm.count("buffer-input")) {
            input_type = BUFFERED;
            input_buffered_map_size = vm["buffer-input"].as<int>();
        }

        if (vm.count("map-input")) {
            input_type = MMAP;
            input_buffered_map_size = vm["map-input"].as<int>();
        }

        if (vm.count("simple-output")) {
            output_type = SIMPLE;
        }

        if (vm.count("fgets-output")) {
            output_type = STDIO;
        }

        if (vm.count("buffer-output")) {
            output_type = BUFFERED;
            output_buffered_map_size = vm["buffer-output"].as<int>();
        }

        if (vm.count("map-output")) {
            output_type = MMAP;
            output_buffered_map_size = vm["map-output"].as<int>();
        }

        if (vm.count("input-files") > 1) {
			input_files = vm["input-files"].as<std::vector<std::string>>();
        }
		else {
            std::cerr << usage << std::endl;
            return 1;
        }

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

    // initializes the specified streams.
    std::shared_ptr<io::InputStream> input_stream;
    std::shared_ptr<io::InputStream> output_stream;

	// TODO : below
    switch (input_type) {
    case SIMPLE:
        std::cout << "Using SimpleInputStream." << std::endl;
        input_stream = std::make_shared<io::SimpleInputStream>();
        break;
    case STDIO:
        std::cout << "Using StdioInputStream." << std::endl;
        input_stream = std::make_shared<io::StdioInputStream>();
        break;
    case BUFFERED:
        std::cout << "Using BufferedInputStream." << std::endl;
        input_stream = std::make_shared<io::BufferedInputStream>(
				input_buffered_map_size,
				input_buffered_map_size);
        break;
    case MMAP:
        std::cout << "Using MMapInputStream." << std::endl;
        input_stream = std::make_shared<io::MMapInputStream>(
				input_buffered_map_size);
        break;
    }

    switch (output_type) {
    case SIMPLE:
        std::cout << "Using SimpleOutputStream." << std::endl;
        output_stream = std::make_shared<io::SimpleOutputStream>();
        break;
    case STDIO:
        std::cout << "Using StdioOutputStream." << std::endl;
        output_stream = std::make_shared<io::StdioOutputStream>();
        break;
    case BUFFERED:
        std::cout << "Using BufferedOutputStream." << std::endl;
        output_stream = std::make_shared<io::BufferedOutputStream>(
				output_buffered_map_size,
                output_buffered_map_size);
        break;
    case MMAP:
        std::cout << "Using MMapOutputStream." << std::endl;
        output_stream = std::make_shared<io::MMapOutputStream>(
				output_buffered_map_size);
        break;
    }

    bool result = input_stream->open(input_file);
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
