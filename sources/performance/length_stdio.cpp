#include "performance/length_stdio.hpp"
#include "stream/input_stdio_stream.hpp"
#include "stream/ouput_stdio_stream.hpp"
#include <resources_config.hpp>

bool tl::length_stdio::count(const std::string& path){
    uint16_t count = 0;
    std::chrono::time_point start = std::chrono::system_clock::now();
    std::unique_ptr<io::InputStream> stream =
        std::make_unique<io::StdioInputStream>();
    stream->open(path);
    while (!stream->end_of_stream()) {
        std::string line = stream->readln();
        if (!line.empty())
            count++;
    }
    std::chrono::time_point end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration= end - start;
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    std::string result = std::to_string(dur.count());
    output(&count,&result);
}

bool tl::length_stdio::output(uint16_t* count, std::string* duration){
    std::unique_ptr<io::OutputStream> stream =
        std::make_unique<io::StdioOutputStream>();
        stream->create(output_file_path);

        std::string input_s = "Line count = " + std::to_string(*count) + " ------ Duration = " + *duration + " ms" ;
        stream->writeln(input_s);
        stream->close();
}