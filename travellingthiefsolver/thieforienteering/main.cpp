#include "travellingthiefsolver/thieforienteering/instance_builder.hpp"
#include "travellingthiefsolver/thieforienteering/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver::thieforienteering;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options

    std::string algorithm = "";
    std::string instance_path = "";
    std::string format = "default";
    std::string initial_solution_path = "";
    std::string output_path = "";
    std::string certificate_path = "";
    std::string log_path = "";
    int loglevelmax = 999;
    int seed = 0;
    int verbosity_level = 1;
    double time_limit = std::numeric_limits<double>::infinity();
    double goal = 0;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("input,i", po::value<std::string>(&instance_path)->required(), "set input file (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: standard)")
        ("output,o", po::value<std::string>(&output_path), "set JSON output file")
        ("initial-solution,", po::value<std::string>(&initial_solution_path), "")
        ("certificate,c", po::value<std::string>(&certificate_path), "set certificate file")
        ("time-limit,t", po::value<double>(&time_limit), "Time limit in seconds")
        ("seed,s", po::value<int>(&seed), "set seed")
        ("goal,g", po::value<double>(&goal), "set goal")
        ("verbosity-level,v", po::value<int>(&verbosity_level), "set verbosity level")
        ("log,l", po::value<std::string>(&log_path), "set log file")
        ("loglevelmax", po::value<int>(&loglevelmax), "set log max level")
        ("log2stderr", "write log to stderr")
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

    // Run algorithm

    InstanceBuilder instance_builder;
    instance_builder.read(instance_path, format);
    const Instance instance = instance_builder.build();

    optimizationtools::Info info = optimizationtools::Info()
        .set_verbosity_level(verbosity_level)
        .set_time_limit(time_limit)
        .set_certificate_path(certificate_path)
        .set_json_output_path(output_path)
        .set_only_write_at_the_end(false)
        .set_log_path(log_path)
        .set_log2stderr(vm.count("log2stderr"))
        .set_maximum_log_level(loglevelmax)
        .set_sigint_handler()
        ;

    std::mt19937_64 generator(seed);
    Solution solution(instance, initial_solution_path);

    auto output = run(
            algorithm,
            instance,
            solution,
            generator,
            info);

    output.solution.write_csv("solution.csv");

    return 0;
}

