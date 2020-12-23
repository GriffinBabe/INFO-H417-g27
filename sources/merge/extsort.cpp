#include <boost/program_options.hpp>
#include <iostream>
#include <resources_config.hpp>
#include <stream/input_stdio_stream.hpp>
#include <stream/ouput_stdio_stream.hpp>
#include <stream/output_stream.hpp>
#include <map>
namespace po = boost::program_options;

std::string input_file;
u_int32_t in_M = 0;
int in_k = 1;
int in_d = 1;
int output_count = 1; //todo : implements queue to keep files reference
bool isRowNumber = false; // is true if the rows k-th is consisted of numbers

int parse_arguments(int argc, char** argv)
{
    // Parse arguments with Boost:
    // https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html

    try {
        po::positional_options_description pos;
        pos.add("input-file", 1);
        po::options_description desc("Allowed options");
        desc.add_options()("k",po::value<int>(),"k-th column")
            ("input-file", po::value<std::string>(), "input csv file.")
            ("M",po::value<u_int32_t >(),"size of the line buffer")
            ("d",po::value<int>(),"number of first elem in the queue to be merged");
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
        in_M = vm["M"].as<u_int32_t>();
        in_d = vm["d"].as<int>();
        in_k = vm["k"].as<int>();
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
std::string* extract_row_elem(std::string* input){ //TODO : consider row element starting and ending with "
    std::vector <std::string> rows ;
    std::string buff;
    std::stringstream iss(*input);
    while(std::getline(iss,buff, ',') && rows.size() < in_k )
    {
        rows.push_back(buff);
    }
    return new std::string(rows[in_k-1]);;
}

void output_file(std::map<int, std::string>* _hashmap){
    std::string output_file_path =
        std::string(OUTPUT_DIR) + "/output_"+std::to_string(output_count)+".txt";
    std::unique_ptr<io::OutputStream> stream = std::make_unique<io::StdioOutputStream>();
    stream->create(output_file_path);
    for(int i = 0; i<(*_hashmap).size() ; i++){
        stream->writeln((*_hashmap).at(i));
    }
    stream->close();
    output_count++;
}

bool is_number(std::string* a)
{
    std::string::const_iterator it = (*a).begin();
    while (it != (*a).end() && std::isdigit(*it)) ++it;
    return !(*a).empty() && it == (*a).end(); //returns true if the string is only composed of digits char
}

bool compare_rows(std::string* a, std::string* b){
    bool resp = false;
    if(isRowNumber){
        resp = stoi(*a)>stoi(*b);
    }
    else{
        resp =(*a).compare(*b)>0;
    }
    delete(a); delete(b);
    return resp;
}
//sort lines by their k-th elem // TODO: optimise memory usage
int sort_map(std::map<int, std::string>* _hashmap){
    std::string temp;
    std::string *elemjm1;
    std::string *elemj;
    //insertion sorting algorithm
    for(int i = 1; i < (*_hashmap).size() ; i++){
        int j = i;
        elemjm1=&((*_hashmap).at(j-1)); // pointer to line j-1
        elemj=&((*_hashmap).at(j)); // pointer to line j
        // while elem k-th in line j-1 is greater than line j (alphabetical comparison)
        while(j > 0 && compare_rows(extract_row_elem(elemjm1),extract_row_elem(elemj))){
            temp = *elemjm1;
            // swap the lines
            std::cout<<"hey"<<std::endl;
            auto const result = (*_hashmap).insert(std::pair<int,std::string>(j-1,*elemj));
            if (not result.second) { result.first->second = *elemj; }

            auto const results2 = (*_hashmap).insert(std::pair<int,std::string>(j,temp));
            if (not results2.second) { results2.first->second = temp; }

            j-=1;
            if(j>0){
                elemjm1=&((*_hashmap).at(j-1)); // pointer to line j-1
                elemj=&((*_hashmap).at(j)); // pointer to line j
            }
        }
    }
}

int read_line(){
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::StdioInputStream>();
    stream->open(input_file);
    int16_t size = 0; //actual size in byte of all the line stored in the list
    std::map<int, std::string> hashmap_lines ; //hashmap of lines already read
    int count = 0;
    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        if(count==0 and output_count==1){
            std::string *temp = extract_row_elem(&line);
            isRowNumber = is_number(temp);
        }
        u_int32_t string_size = line.length();
        if (!line.empty()) {
            if (size + string_size < in_M) { // if the capacity isn't exceeded
                hashmap_lines.insert(std::pair<int,std::string>(count,line));
                count ++;
                size += string_size;

            }
            else {
                // CALL SORT FUNCTION ON THIS MAP
                sort_map(&hashmap_lines);
                // CALL OUTPUT FUNCTION TO WRITE A NEW OUTPUT FILE
                // FLUSH LIST AND ADD THE OVERFLOW LINE IN IT
                output_file(&hashmap_lines);
                hashmap_lines.clear();
                count = 0;
                hashmap_lines.insert(std::pair<int,std::string>(count,line));
                count++;
                size = string_size;
            }
        }
    }
    if(!hashmap_lines.empty()){ // If the list isn't empty, process it
        // CALL SORT FUNCTION ON THIS LIST
        // CALL OUTPUT FUNCTION TO WRITE A NEW OUTPUT FILE
        sort_map(&hashmap_lines);
        output_file(&hashmap_lines);
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