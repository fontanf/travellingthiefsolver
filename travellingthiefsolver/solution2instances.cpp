#include "travellingthiefsolver/utils.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options

    std::string instance_path = "";
    std::string format = "default";
    std::string output_path = "";
    std::string certificate_path = "";

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(&instance_path)->required(), "set input file (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: standard)")
        ("output,o", po::value<std::string>(&output_path), "set JSON output file")
        ("certificate,c", po::value<std::string>(&certificate_path), "set certificate file")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        po::notify(vm);
    } catch (const po::required_option& e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    // Read instance.
    Instance instance(instance_path, format);

    // Read solution.
    Solution solution(instance, certificate_path);

    // Create PWT instance and write it.
    auto pwt_instance = create_pwt_instance(instance, solution);
    pwt_instance.write(output_path + ".pwt");

    // Create TWP instance and write it.
    auto twp_instance = create_twp_instance(instance, solution);
    twp_instance.write(output_path + ".twp");

    return 0;
}

