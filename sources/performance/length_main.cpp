#include <boost/program_options.hpp>
#include <performance/length_stdio.hpp>
#include <performance/length_simple.hpp>
#include <performance/length_buffered.hpp>
#include <performance/length_mmap.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    // Parse arguments with Boost: https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
    // issues with boost, will implement later
    if (argc == 3){
        const std::string target = std::string(RESOURCES_DIR) + "/" +argv[1]+ ".csv";
        const std::string argf(argv[2]);
        if (argf == "stdio"){
            std::unique_ptr<tl::length_stdio> stdio_length = std::make_unique<tl::length_stdio>();
            if (stdio_length != nullptr){
                stdio_length->count(target);
            }
        }
        else if (argf == "simple"){
            std::unique_ptr<tl::length_simple> simple_length = std::make_unique<tl::length_simple>();
            if (simple_length != nullptr){
                simple_length->count(target);
            }
        }
        else if (argf == "buffered"){
            std::unique_ptr<tl::length_buffered> buffered_length = std::make_unique<tl::length_buffered>();
            if (buffered_length != nullptr){
                buffered_length->count(target);
            }
        }
        else if (argf == "mmap"){
            std::unique_ptr<tl::length_mmap> mmap_length = std::make_unique<tl::length_mmap>();
            if (mmap_length != nullptr){
                mmap_length->count(target);
            }
        }
    }
    return 0;
}
