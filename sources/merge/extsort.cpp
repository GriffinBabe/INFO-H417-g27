#include <boost/program_options.hpp>
#include <iostream>
#include <resources_config.hpp>
#include <stream/input_stdio_stream.hpp>
#include <stream/ouput_stdio_stream.hpp>
#include <stream/output_stream.hpp>
#include <map>
namespace po = boost::program_options;

std::string input_file;
int16_t in_M = 0;
int8_t in_k = 1;
int16_t in_d = 1;

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-file", 1);
        po::options_description desc("Allowed options");
        desc.add_options()("k",po::value<int8_t>(),"k-th column")
            ("input-file", po::value<std::string>(), "input csv file.")
            ("M",po::value<int16_t>(),"size of the line buffer")
            ("d",po::value<int16_t>(),"number of first elem in the queue to be merged");
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(pos)
                      .run(),
                  vm);
        po::notify(vm);
        if (!vm.count("input-file")) {
            std::cerr << "no input file" << std::endl;
            return 1;
        }
        if(!vm.count("M")){
            std::cerr << "no buffer size" << std::endl;
            return 1;
        }
        if(!vm.count("d")){
            std::cerr << "no elem number" << std::endl;
            return 1;
        }
        if(!vm.count("k")){
            std::cerr << "no column number" << std::endl;
            return 1;
        }
        input_file = vm["input-file"].as<std::string>();
        input_file = std::string(RESOURCES_DIR) + "/" + input_file;
        in_M = vm["M"].as<int16_t>();
        in_d = vm["d"].as<int16_t>();
        in_k = vm["k"].as<int8_t>();
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

//returns the k-th elem of the line
std::string extract_row_elem(std::string* input){
    std::vector <std::string> rows;
    std::string buff;
    std::stringstream iss(*input);
    while(std::getline(iss,buff, ',') && rows.size() < in_k )
    {
        rows.push_back(buff);
    }
    return rows[in_k-1];
}

//sort lines by their k-th elem
int sort_list(std::map<int, std::string> _hashmap){
    //comparing two string with std::compare is efficient for sorting
}

int read_line(){
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::StdioInputStream>();
    stream->open(input_file);
    int16_t size = 0; //actual size in byte of all the line stored in the list
    std::map<int, std::string> hashmap_lines; //hashmap of lines already read
    int count = 0;
    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        uint16_t string_size = line.length();
        if (!line.empty()) {
            count ++;
            if (size + string_size < in_M) { // if the capacity isn't exceeded
                hashmap_lines.insert(std::pair<int,std::string>(count,line));
                size += string_size;
            }
            else {
                // CALL SORT FUNCTION ON THIS LIST
                // CALL OUTPUT FUNCTION TO WRITE A NEW OUTPUT FILE
                // FLUSH LIST AND ADD THE OVERFLOW LINE IN IT
                size = string_size;
            }
        }
    }
    if(!hashmap_lines.empty()){ // If the list isn't empty, process it
        // CALL SORT FUNCTION ON THIS LIST
        // CALL OUTPUT FUNCTION TO WRITE A NEW OUTPUT FILE
    }
}
int main(int argc, char** argv)
{
    int ret_status = parse_arguments(argc, argv);
    if (ret_status != 0) {
        return ret_status;
    }
    //on devra utiliser la combinaison read/write optimale trouvée au point 1.3
    //en attendant des streams stdio sont utilisés de manière temporaire
    read_line();

}