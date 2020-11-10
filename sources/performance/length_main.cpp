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
    if (argc == 3){
        const std::string target = argv[1];
        std::string argf(argv[2]);
        if (argf == "stdio"){
            std::cout << "stdio";
            // std::unique_ptr<tl::length_stdio> stdio_length = std::make_unique<tl::length_stdio::count>(argf*);
        }
    }

    return 0;
}
