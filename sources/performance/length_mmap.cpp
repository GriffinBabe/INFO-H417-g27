#include "performance/length_mmap.hpp"
#include "stream/mmap_input_stream.hpp"
#include "stream/mmap_output_stream.hpp"
#include <iostream>
#include <resources_config.hpp>

bool tl::length_mmap::count(const std::string& path){
    try {
        uint16_t count = 0;
        std::chrono::time_point start = std::chrono::system_clock::now();
        std::unique_ptr<io::InputStream> stream =
            std::make_unique<io::MMapInputStream>(100);
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

void tl::length_mmap::output(uint16_t* count, std::string* duration){
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::MMapOutputStream>(100);
    stream->create(output_file_path);

    std::string input_s = "Line count = " + std::to_string(*count)
                          + " ------ Duration = " + *duration + " µs";
    stream->writeln(input_s);
    stream->close();

}

