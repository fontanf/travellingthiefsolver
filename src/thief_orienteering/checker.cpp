#include "travellingthiefsolver/thief_orienteering/instance_builder.hpp"
#include "travellingthiefsolver/thief_orienteering/solution.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver::thief_orienteering;

template <typename Distances>
Solution build_solution(
        const Distances& distances,
        const Instance& instance,
        const std::string& certificate_path)
{
    return Solution(
            distances,
            instance,
            certificate_path);
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options

    std::string instance_path = "";
    std::string format = "default";
    std::string certificate_path = "";

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(&instance_path)->required(), "set input file (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: standard)")
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
    Solution solution = FUNCTION_WITH_DISTANCES(
            build_solution,
            instance.distances(),
            instance,
            certificate_path);

    std::cout
        << "Solution" << std::endl
        << "--------" << std::endl
        ;
    solution.format(std::cout, 2);

    return 0;
}
