#include <boost/program_options.hpp>
#include <iostream>
#include <resources_config.hpp>

namespace po = boost::program_options;
std::string input_file;

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
            ("M",po::value<int>(),"size of the line buffer")
            ("d",po::value<int>(),"reference to input stream");
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
        input_file = vm["input-file"].as<std::string>();
        input_file = std::string(RESOURCES_DIR) + "/" + input_file;
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

int main(int argc, char** argv)
{
    int ret_status = parse_arguments(argc, argv);
    if (ret_status != 0) {
        return ret_status;
    }
}