#include "travellingthiefsolver/packing_while_travelling/instance_builder.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

#include "travellingthiefsolver/packing_while_travelling/algorithms/greedy.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/dynamic_programming.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/efficient_local_search.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/large_neighborhood_search.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver::packing_while_travelling;

namespace po = boost::program_options;

void read_args(
        Parameters& parameters,
        const po::variables_map& vm)
{
    parameters.timer.set_sigint_handler();
    parameters.messages_to_stdout = true;
    if (vm.count("time-limit"))
        parameters.timer.set_time_limit(vm["time-limit"].as<double>());
    if (vm.count("verbosity-level"))
        parameters.verbosity_level = vm["verbosity-level"].as<int>();
    if (vm.count("log"))
        parameters.log_path = vm["log"].as<std::string>();
    parameters.log_to_stderr = vm.count("log-to-stderr");
    bool only_write_at_the_end = vm.count("only-write-at-the-end");
    if (!only_write_at_the_end) {
        std::string certificate_path = vm["certificate"].as<std::string>();
        std::string json_output_path = vm["output"].as<std::string>();
        parameters.new_solution_callback = [
            json_output_path,
            certificate_path](
                    const Output& output,
                    const std::string&)
        {
            output.write_json_output(json_output_path);
            output.solution.write(certificate_path);
        };
    }
}

Output run(
        const Instance& instance,
        const po::variables_map& vm)
{
    std::mt19937_64 generator(vm["seed"].as<Seed>());

    SolutionBuilder solution_builder;
    solution_builder.set_instance(instance);
    if (vm.count("initial-solution"))
        solution_builder.read(vm["initial-solution"].as<std::string>());
    Solution solution = solution_builder.build();

    // Run algorithm.
    std::string algorithm = vm["algorithm"].as<std::string>();
    if (algorithm == "greedy") {
        GreedyParameters parameters;
        read_args(parameters, vm);
        return greedy(instance, parameters);
    } else if (algorithm == "sequential-value-correction") {
        SequentialValueCorrectionParameters parameters;
        read_args(parameters, vm);
        return sequential_value_correction(instance, parameters);
    } else if (algorithm == "dynamic-programming") {
        DynamicProgrammingParameters parameters;
        read_args(parameters, vm);
        return dynamic_programming(instance, parameters);
    } else if (algorithm == "efficient-local-search") {
        EfficientLocalSearchParameters parameters;
        read_args(parameters, vm);
        return efficient_local_search(instance, parameters);
    } else if (algorithm == "large-neighborhood-search") {
        LargeNeighborhoodSearchParameters parameters;
        read_args(parameters, vm);
        return large_neighborhood_search(instance, generator, parameters);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm + "\".");
    }
}

int main(int argc, char *argv[])
{
    // Parse program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>()->default_value("large-neighborhood-search"), "set algorithm")
        ("input,i", po::value<std::string>()->required(), "set input file (required)")
        ("format,f", po::value<std::string>()->default_value(""), "set input file format (default: standard)")
        ("unicost,u", "set unicost")
        ("output,o", po::value<std::string>()->default_value(""), "set JSON output file")
        ("initial-solution,", po::value<std::string>()->default_value(""), "")
        ("certificate,c", po::value<std::string>()->default_value(""), "set certificate file")
        ("seed,s", po::value<Seed>()->default_value(0), "set seed")
        ("time-limit,t", po::value<double>(), "set time limit in seconds")
        ("verbosity-level,v", po::value<int>(), "set verbosity level")
        ("only-write-at-the-end,e", "only write output and certificate files at the end")
        ("log,l", po::value<std::string>(), "set log file")
        ("log-to-stderr", "write log to stderr")

        //("maximum-number-of-iterations,", po::value<int>(), "set the maximum number of iterations")
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

    // Build instance.
    InstanceBuilder instance_builder;
    instance_builder.read(
            vm["input"].as<std::string>(),
            vm["format"].as<std::string>());
    const Instance instance = instance_builder.build();

    // Run.
    Output output = run(instance, vm);

    // Write outputs.
    std::string certificate_path = vm["certificate"].as<std::string>();
    std::string json_output_path = vm["output"].as<std::string>();
    output.write_json_output(json_output_path);
    output.solution.write(certificate_path);

    return 0;
}
