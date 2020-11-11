#include <boost/program_options.hpp>
#include <iostream>
#include <performance/length_stdio.hpp>
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    // Parse arguments with Boost: https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
    // issues with boost, will implement later
    if (argc == 3){
        const std::string target = std::string(RESOURCES_DIR) + "/" +argv[1]+".csv";
        const std::string argf(argv[2]);
        if (argf == "stdio"){
            std::unique_ptr<tl::length_stdio> stdio_length = std::make_unique<tl::length_stdio>();
            if (stdio_length != nullptr){
                stdio_length->count(target);
            }
        }
    }

    return 0;
{
}
}