#include "performance/length_seq_read.hpp"
#include "stream/buffered_input_stream.hpp"
#include "stream/input_stdio_stream.hpp"
#include "stream/mmap_input_stream.hpp"
#include "stream/ouput_stdio_stream.hpp"
#include "stream/simple_input_stream.hpp"

bool tl::length_seq_read::count(const std::string& path,const std::string& fonction){
    try {
        output_file_path = std::string(OUTPUT_DIR) + "/output_"+fonction+"_seq_read.txt";
        uint16_t count = 0;
        std::chrono::time_point start = std::chrono::system_clock::now();
        std::unique_ptr<io::InputStream> stream = nullptr;
        if(fonction == "stdio"){
            stream = std::make_unique<io::StdioInputStream>();
        }
        else if(fonction == "simple"){
            stream = std::make_unique<io::SimpleInputStream>();
        }
        else if(fonction == "buffered"){
            stream = std::make_unique<io::BufferedInputStream>(1000,100);
        }
        else if(fonction == "mmap"){
            stream = std::make_unique<io::MMapInputStream>(100);
        }
        else{ return false;}
        if(!stream->open(path)){
            return false;
        }
        else {
            while (!stream->end_of_stream()) {
                std::string line = stream->readln();
                if (!line.empty())
                    count++;
            }
            std::chrono::time_point end = std::chrono::system_clock::now();
            std::chrono::duration<double> duration = end - start;
            auto dur =
                std::chrono::duration_cast<std::chrono::microseconds>(duration);
            std::string result = std::to_string(dur.count());
            output(&count, &result);
            return true;
        }
    }
    catch(std::exception e){
        return false;
    }
}

void tl::length_seq_read::output(uint16_t* count, std::string* duration){
    std::unique_ptr<io::OutputStream> stream =
    std::make_unique<io::StdioOutputStream>();
    stream->create(output_file_path);

    std::string input_s = "Line count = " + std::to_string(*count)
    + " ------ Duration = " + *duration + " µs";
    stream->writeln(input_s);
    stream->close();

}

