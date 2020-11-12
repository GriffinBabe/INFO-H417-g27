#include <boost/program_options.hpp>
#include <performance/length_seq_read.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    // Parse arguments with Boost: https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
    // issues with boost, will implement later
    if (argc == 3){
        const std::string target = std::string(RESOURCES_DIR) + "/" +argv[1]+ ".csv";
        const std::string argf(argv[2]);
        std::unique_ptr<tl::length_seq_read> seq_read = std::make_unique<tl::length_seq_read>();
        if (seq_read != nullptr){
            if(seq_read->count(target,argf) == false){
                std::cerr
                    << "Error : couldn't perform sequential reading with args "+target+" "+argf
                    << std::endl;
            }
        }

    }
    return 0;
}
