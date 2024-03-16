#include "travellingthiefsolver/travellingthief/instance_builder.hpp"
#include "travellingthiefsolver/travellingthief/utils.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver::travellingthief;

template <typename Distances>
void run(
        const Distances& distances,
        const Instance& instance,
        const std::string& certificate_path,
        const std::string& output_path)
{
    Solution solution(
            distances,
            instance,
            certificate_path);

    // Create PWT instance and write it.
    auto pwt_instance = create_pwt_instance(distances, instance, solution);
    pwt_instance.write(output_path + ".pwt");

    // Create TWP instance and write it.
    auto twp_instance = create_twp_instance(instance, solution);
    twp_instance.write(output_path + ".twp");
}

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
    InstanceBuilder instance_builder;
    instance_builder.read(instance_path, format);
    const Instance instance = instance_builder.build();

    // Read solution.
    FUNCTION_WITH_DISTANCES(
            run,
            instance.distances(),
            instance,
            certificate_path,
            output_path);

    return 0;
}
