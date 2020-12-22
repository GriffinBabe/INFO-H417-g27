#include <boost/program_options.hpp>
#include <iostream>

#include <stream/buffered_input_stream.hpp>
#include <stream/input_stdio_stream.hpp>
#include <stream/mmap_input_stream.hpp>
#include <stream/simple_input_stream.hpp>
#include <stream/buffered_output_stream.hpp>
#include <stream/ouput_stdio_stream.hpp>
#include <stream/mmap_output_stream.hpp>
#include <stream/simple_output_stream.hpp>

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

/**
 * The output file path, specified in the command line arguments.
 */
std::string output_file;

std::string program_description =
    "Merges the content of the input files by copying one line of each file "
	"in a round robin fashion, until all of them have been processed.";

std::string usage = "Usage: rrmerge [--help] input-stream [size] "
					"output-stream [size] input-files; "
					"\tthere should be at least 2 input files";

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
            "si", "will use the SimpleInputStream.")(
            "fi", "will use the StdioInputStream.")(
            "mi",
            po::value<int>(),
            "mapping size, if specified will use a MMapInputStream.")(
            "bi",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedInputStream.")(
            "so", "will use the SimpleOutputStream.")(
            "fo", "will use the StdioOutputStream.")(
            "mo",
            po::value<int>(),
            "mapping size, if specified will use a MMapOutputStream.")(
            "bo",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedOutputStream.")(
            "output-file,o",
			po::value<std::string>(),
			"output csv file.")(
            "input-files,i",
			po::value<std::vector<std::string>>(),
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

        if (vm.count("si")) {
            input_type = SIMPLE;
        }

        if (vm.count("fi")) {
            input_type = STDIO;
        }

        if (vm.count("bi")) {
            input_type = BUFFERED;
            input_buffered_map_size = vm["bi"].as<int>();
        }

        if (vm.count("mi")) {
            input_type = MMAP;
            input_buffered_map_size = vm["mi"].as<int>();
        }

        if (vm.count("so")) {
            output_type = SIMPLE;
        }

        if (vm.count("fo")) {
            output_type = STDIO;
        }

        if (vm.count("bo")) {
            output_type = BUFFERED;
            output_buffered_map_size = vm["bo"].as<int>();
        }

        if (vm.count("mo")) {
            output_type = MMAP;
            output_buffered_map_size = vm["mo"].as<int>();
        }

        if (vm.count("output-file")) {
            output_file = vm["output-file"].as<std::string>();
        }

        if (!vm.count("output-file")) {
            output_file = "./default_output.csv";
        }

        if (vm.count("input-files")) {
			input_files = vm["input-files"].as<std::vector<std::string>>();
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
	if ( input_files.size() < 2 )
	{
        std::cerr << usage << std::endl;
        std::cerr << "At least two input files are necessary." << std::endl;
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
	std::vector<std::shared_ptr<io::InputStream>> input_streams;
	for ( int i = 0; i < input_files.size(); i++ )
	{
		input_streams.push_back(std::shared_ptr<io::InputStream>());
	}

	std::shared_ptr<io::OutputStream> output_stream;

    switch (input_type) {
    case SIMPLE:
        std::cout << "Using SimpleInputStream." << std::endl;
		for ( auto& ptr : input_streams )
			ptr = std::make_shared<io::SimpleInputStream>();
        break;
    case STDIO:
        std::cout << "Using StdioInputStream." << std::endl;
		for ( auto& ptr : input_streams )
			ptr = std::make_shared<io::StdioInputStream>();
        break;
    case BUFFERED:
        std::cout << "Using BufferedInputStream." << std::endl;
		for ( auto& ptr : input_streams )
			ptr = std::make_shared<io::BufferedInputStream>(
					input_buffered_map_size,
					input_buffered_map_size);
        break;
    case MMAP:
        std::cout << "Using MMapInputStream." << std::endl;
		for ( auto& ptr : input_streams )
			ptr = std::make_shared<io::MMapInputStream>(
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
                output_buffered_map_size);
        break;
    case MMAP:
        std::cout << "Using MMapOutputStream." << std::endl;
        output_stream = std::make_shared<io::MMapOutputStream>(
				output_buffered_map_size);
        break;
    }

    bool input_result = false;
	auto iss = input_streams.begin();
	auto ifs = input_files.begin();
	while (iss != input_streams.end() and ifs != input_files.end())
	{
		input_result = (*iss)->open(*ifs);
		assert(input_result);
		iss++; ifs ++;
	}
	
	bool output_result = output_stream->create(output_file);
	assert(output_result);

    std::chrono::time_point start = std::chrono::system_clock::now();
	bool eof = false;
	int not_eof = 0;
	while (!eof)
	{
		for (auto& input_stream : input_streams)
		{
			if (!input_stream->end_of_stream())
			{
				std::string line = input_stream->readln();
				if (!line.empty())
					std::cout << line; // TODO
					output_stream->writeln(line);
				not_eof++;
			}
		}
		
		if (not_eof > 0)
			not_eof = 0;
		else
			eof = true;
	}
    std::chrono::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    std::cout << " ------ Duration: " << dur.count() << "ms." << std::endl;

	output_stream->close();

    return 0;
}
