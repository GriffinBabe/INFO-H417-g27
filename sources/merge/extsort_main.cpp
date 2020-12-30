#include "merge/entry.hpp"
#include "stream/buffered_input_stream.hpp"
#include "stream/buffered_output_stream.hpp"
#include "stream/input_stdio_stream.hpp"
#include "stream/mmap_input_stream.hpp"
#include "stream/mmap_output_stream.hpp"
#include "stream/ouput_stdio_stream.hpp"
#include "stream/simple_input_stream.hpp"
#include "stream/simple_output_stream.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <queue>
#include <stdio.h>

namespace po = boost::program_options;

/**
 * Different possibilities of stream to use.
 */
enum StreamType { SIMPLE, STDIO, BUFFERED, MMAP };

/**
 * Path of the file to merge sort.
 */
std::string input_file;

/**
 * Max memory used by the program. This limits the number of loaded elements
 * of the file in the primary memory.
 */
std::uint32_t memory_size = 0;

/**
 * The number of the used column.
 */
std::uint16_t kth_column = 0;

std::uint32_t in_d = 0;

/**
 * Size of the buffer or the mapping region in case of using a
 * BufferedInputStream or a MMapInputStream
 */
std::uint32_t input_stream_size = 0;

/**
 * Size of the buffer or the mapping region in case of using a
 * BufferedOutputStream or a MMapOutputStream
 */
std::uint32_t output_stream_size = 0;

/**
 * Types of used streams, specified in command line arguments, uses STDIO by
 * default if not specified.
 */
StreamType input_type = STDIO;
StreamType output_type = STDIO;

std::string program_description =
    "Sorts a file with a limited primary memory usage by merge sorting multiple"
    "temporary files.";

std::string usage =
    "Usage: extsort [--help] input-stream [size] "
    "output-stream [size] input-file k M d";

/**
 * Temporary files prefix and suffix
 */
const std::string temp_file_path_prefix = "./merge_";
const std::string temp_file_path_suffix = ".temp";

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-file", 1);
        pos.add("k", 1);
        pos.add("M", 1);
        pos.add("d", 1);

        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message.")(
            "si", "will use the SimpleInputStream.")(
            "fi", "will use the StdioInputStream (by default).")(
            "mi",
            po::value<int>(),
            "mapping size, if specified will use a MMapInputStream.")(
            "bi",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedInputStream.")(
            "so", "will use the SimpleOutputStream.")(
            "fo", "will use the StdioOutputStream (by default).")(
            "mo",
            po::value<int>(),
            "mapping size, if specified will use a MMapOutputStream.")(
            "bo",
            po::value<int>(),
            "buffer size, if specified, will use a BufferedOutputStream.")(
            "input-file", po::value<std::string>(), "input csv file.")(
            "k", po::value<int>(), "k-th column")(
            "M", po::value<int>(), "size of the line buffer")(
            "d",
            po::value<int>(),
            "number of first elem in the queue to be merged");
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
            std::cout << "Using SimpleInputStream." << std::endl;
        }
        else if (vm.count("fi")) {
            input_type = STDIO;
            std::cout << "Using StdioInputStream." << std::endl;
        }
        else if (vm.count("bi")) {
            input_type = BUFFERED;
            input_stream_size = vm["bi"].as<int>();
            std::cout << "Using BufferedInputStream. Buffer: "
                      << input_stream_size << " bytes.";
        }
        else if (vm.count("mi")) {
            input_type = MMAP;
            input_stream_size = vm["mi"].as<int>();
            std::cout << "Using MMapInputStream. Mapping region: "
                      << input_stream_size << " bytes.";
        }
        else {
            std::cout << "Using StdioInputStream." << std::endl;
        }

        if (vm.count("so")) {
            output_type = SIMPLE;
            std::cout << "Using SimpleOutputStream." << std::endl;
        }
        else if (vm.count("fo")) {
            output_type = STDIO;
            std::cout << "Using StdioOutputStream." << std::endl;
        }
        else if (vm.count("bo")) {
            output_type = BUFFERED;
            output_stream_size = vm["bo"].as<int>();
            std::cout << "Using BufferedOutputStream. Buffer: "
                      << output_stream_size << " bytes." << std::endl;
        }
        else if (vm.count("mo")) {
            output_type = MMAP;
            output_stream_size = vm["mo"].as<int>();
            std::cout << "Using MMapOutputStream. Mapping region: "
                      << output_stream_size << " bytes." << std::endl;
        }
        else {
            std::cout << "Using StdioOutputStream." << std::endl;
        }

        if (!vm.count("input-file")) {
            std::cerr << usage << std::endl;
            return 1;
        }
        if (!vm.count("M")) {
            std::cerr << usage << std::endl;
            return 1;
        }
        if (!vm.count("d")) {
            std::cerr << usage << std::endl;
            return 1;
        }
        if (!vm.count("k")) {
            std::cerr << usage << std::endl;
            return 1;
        }

        input_file = vm["input-file"].as<std::string>();
        memory_size = vm["M"].as<int>();
        in_d = vm["d"].as<int>();
        kth_column = vm["k"].as<int>();
    }
    catch (boost::bad_any_cast const& exc) {
        std::cerr << "Bad parameter type: " << exc.what() << std::endl;
        return 1;
    }
    catch (po::error const& exc) {
        std::cerr << "Exeption while parsing arguments: " << exc.what()
                  << std::endl;
        return 1;
    }

    return 0;
}

void initialize_stream(std::unique_ptr<io::InputStream>& in_stream)
{
    switch (input_type) {
    case SIMPLE:
        in_stream = std::make_unique<io::SimpleInputStream>();
        break;
    case STDIO:
        in_stream = std::make_unique<io::StdioInputStream>();
        break;
    case BUFFERED:
        in_stream =
            std::make_unique<io::BufferedInputStream>(input_stream_size);
        break;
    case MMAP:
        in_stream = std::make_unique<io::MMapInputStream>(input_stream_size);
        break;
    }
}

void initialize_stream(std::unique_ptr<io::OutputStream>& out_stream)
{
    switch (output_type) {
    case SIMPLE:
        out_stream = std::make_unique<io::SimpleOutputStream>();
        break;
    case STDIO:
        out_stream = std::make_unique<io::StdioOutputStream>();
        break;
    case BUFFERED:
        out_stream =
            std::make_unique<io::BufferedOutputStream>(output_stream_size);
        break;
    case MMAP:
        out_stream = std::make_unique<io::MMapOutputStream>(output_stream_size);
        break;
    }
}

void save_to_file(std::unique_ptr<io::OutputStream>& stream,
                  std::string const& path,
                  std::vector<merge::Entry> const& entries)
{
    std::cout << "Saving " << entries.size() << " entries to file: " << path
              << std::endl;
    // write sorted elements
    bool success = stream->create(path);
    assert(success);
    for (auto const& entry : entries) {
        stream->writeln(entry.to_string());
    }
    success = stream->close();
    assert(success);
}

void sort_entries(std::vector<merge::Entry>& entries, std::uint16_t k)
{
    std::sort(entries.begin(),
              entries.end(),
              [&](const merge::Entry& fst, const merge::Entry& snd) {
                  return fst.compare(k, snd);
              });
}

/**
 * Sorts a deque of at most D entries.
 *
 * @param entries, the queue of entries to sort (the first element of the pair
 *                 is not relevant in this function).
 * @param k, the column to sort.
 */
void sort_entries(std::deque<std::pair<std::uint16_t, merge::Entry>>& entries,
                  std::uint16_t k)
{
    std::sort(entries.begin(),
              entries.end(),
              [&](std::pair<std::uint16_t, merge::Entry> const& fst,
                  std::pair<std::uint16_t, merge::Entry> const& snd) {
                  return fst.second.compare(k, snd.second);
              });
}

void split_file(std::unique_ptr<io::InputStream>& in_stream,
                std::unique_ptr<io::OutputStream>& out_stream,
                std::queue<std::string>& sub_files,
                std::uint16_t k_th)
{
    std::uint32_t used_memory = 0;

    std::uint32_t temp_file_count = 0;

    // list of entries, can be considered as the memory buffers
    std::vector<merge::Entry> entries;

    bool success = in_stream->open(input_file);
    assert(success);

    merge::Header table_header;
    while (!in_stream->end_of_stream()) {
        merge::Entry entry;
        std::string line = in_stream->readln();

        if (!line.empty()) {
            entry.parse_from(line, table_header);
            std::uint32_t entry_size = entry.size();
            if (used_memory + entry_size >= memory_size) {
                // memory limit reached, writing to file before adding the
                // entry to the vector (memory buffers).
                std::string temp_path = temp_file_path_prefix
                                        + std::to_string(temp_file_count)
                                        + temp_file_path_suffix;
                temp_file_count++;
                sub_files.push(temp_path);

                // sort the entries and write to file
                sort_entries(entries, k_th);
                save_to_file(out_stream, temp_path, entries);
                // clearing memory counter and entry array
                used_memory = 0;
                entries.clear();
            }
            // adds the entry to the vector (memory buffers)
            entries.push_back(entry);
            used_memory += entry_size;
        }
    }

    // checks if the entry vector is not empty
    if (!entries.empty()) {
        std::string temp_path = temp_file_path_prefix
                                + std::to_string(temp_file_count)
                                + temp_file_path_suffix;
        sub_files.push(temp_path);

        // sort the entries and write to file.
        sort_entries(entries, k_th);
        save_to_file(out_stream, temp_path, entries);
        entries.clear();
    }
}

void merge_files(std::queue<std::string>& queue,
                 std::uint32_t& out_file_counter,
                 std::uint16_t k_th,
                 std::uint16_t d)
{
    // if there is only one file remaining
    if (queue.size() <= 1) {
        std::cout << "Merge sort finished, output in file: " << queue.front()
                  << std::endl;
        return;
    }

    std::queue<std::string> out_queue;

    // utility to increase sort speed.
    merge::Header header;

    // take the first D elements of the queue and starts reading them
    while (!queue.empty()) {
        // takes at most d files (less if there are less than d available files
        // to merge).
        std::uint16_t opened_files = (queue.size() < d) ? queue.size() : d;

        // Temporary treated merge files, will be deleted at the end of the
        // function
        std::vector<std::string> treated_merge_files;

        // if there is only one file opened then put it to the out queue
        // and forwards it in the next iteration of merge_files
        // (little optimization)
        if (d == 1) {
            out_queue.push(queue.front());
            queue.pop();
            break;
        }

        std::vector<std::unique_ptr<io::InputStream>> input_streams(
            opened_files);
        std::unique_ptr<io::OutputStream> output_stream;

        // initializes and opens the input stream
        for (auto& input : input_streams) {
            initialize_stream(input);
            bool success = input->open(queue.front());
            treated_merge_files.push_back(queue.front());
            assert(success);
            queue.pop();
        }

        // initializes output stream and creates new file
        initialize_stream(output_stream);
        std::string temp_path = temp_file_path_prefix
                                + std::to_string(out_file_counter++)
                                + temp_file_path_suffix;
        bool success = output_stream->create(temp_path);
        out_queue.push(temp_path);

        // vector containing all the entries to merge
        std::deque<std::pair<std::uint16_t, merge::Entry>> to_merge_entries;

        // for each stream reads the initial line, parses the entry and add
        // it to the entry merge queue
        for (std::uint16_t i = 0; i < input_streams.size(); i++) {
            // reads line and parses it as an entry, then adds it to a queue
            merge::Entry entry;
            std::string line = input_streams[i]->readln();
            if (!line.empty()) {
                entry.parse_from(line, header);
                to_merge_entries.emplace_back(i, entry);
            }
        }

        // merges the files by:
        // 1) sorting the entries
        // 2) writing the first entry to the output file,
        // 3) reading and adding a new entry from the queue
        // with the input stream of the entry that was just printed.
        while (!to_merge_entries.empty()) {
            sort_entries(to_merge_entries, k_th);
            std::pair<std::uint16_t, merge::Entry> top_entry =
                to_merge_entries.front();
            output_stream->writeln(top_entry.second.to_string());
            to_merge_entries.pop_front();

            if (!input_streams[top_entry.first]->end_of_stream()) {
                merge::Entry new_entry;
                std::string line = input_streams[top_entry.first]->readln();
                if (!line.empty()) {
                    new_entry.parse_from(line, header);
                    to_merge_entries.emplace_back(top_entry.first, new_entry);
                }
            }
        }

        // at this point all the lines of the d files have been merged
        success = output_stream->close();
        assert(success);

        // deletes the temporary merge files
        for (auto const& str : treated_merge_files) {
            bool success = remove(str.c_str());
            assert((success == 0));
        }
    }

    // next step of merge
    merge_files(out_queue, out_file_counter, k_th, d);
}

int main(int argc, char** argv)
{
    int ret_status = parse_arguments(argc, argv);
    if (ret_status != 0) {
        return ret_status;
    }
    std::unique_ptr<io::InputStream> in_stream;
    std::unique_ptr<io::OutputStream> out_stream;

    std::chrono::time_point start = std::chrono::system_clock::now();

    // initializes the streams DUH!
    initialize_stream(in_stream);
    initialize_stream(out_stream);

    // queue of all the temporary files
    std::cout
        << "------ Start splitting table in multiple sorted temporary files"
        << std::endl;
    std::queue<std::string> sub_files;


    // splits the main files in multiple temporary files and fills
    // the queue to those file paths.
    split_file(in_stream, out_stream, sub_files, kth_column);

    std::chrono::time_point end_split = std::chrono::system_clock::now();
    std::cout << "------ Temporary files created, start merging" << std::endl;

    // merges the temporary file in one final file.
    std::uint32_t temp_file_counter = sub_files.size();
    merge_files(sub_files, temp_file_counter, kth_column, in_d);

    std::chrono::time_point end = std::chrono::system_clock::now();

    // Prints total duration
    std::chrono::duration<double> duration = end - start;
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "\n ------ Total Duration: " << dur.count() << "ms." << "\n" << std::endl;

    // Prints split phase duration
    std::chrono::duration<double> split_duration = end_split - start;
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(split_duration);
    std::cout << " ------ Split Duration: " << dur.count() << "ms." << std::endl;

    // Prints merge phase duration
    std::chrono::duration<double> merge_duration = end - end_split;
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(merge_duration);
    std::cout << " ------ Merge Duration: " << dur.count() << "ms." << std::endl;

    return 0;
}
