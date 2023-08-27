#include "packingwhiletravellingsolver/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace packingwhiletravellingsolver;
namespace po = boost::program_options;

Output packingwhiletravellingsolver::run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info)
{
    (void)initial_solution;
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for (Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm.empty() || algorithm_args[0].empty()) {
        throw std::invalid_argument("Missing algorithm.");

    } else if (algorithm_args[0] == "greedy") {
        // auto parameters = read_greedy_args(algorithm_argv);
        GreedyOptionalParameters parameters;
        parameters.info = info;
        return greedy(instance, parameters);

    } else if (algorithm_args[0] == "sequential_value_correction") {
        SequentialValueCorrectionOptionalParameters parameters;
        parameters.info = info;
        return sequential_value_correction(instance, parameters);

    } else if (algorithm_args[0] == "dynamic_programming") {
        DynamicProgrammingOptionalParameters parameters;
        parameters.info = info;
        return dynamic_programming(instance, parameters);

    } else if (algorithm_args[0] == "efficient_local_search") {
        EfficientLocalSearchOptionalParameters parameters;
        parameters.info = info;
        return efficient_local_search(instance, parameters);

    } else if (algorithm_args[0] == "large_neighborhood_search") {
        LargeNeighborhoodSearchOptionalParameters parameters;
        parameters.info = info;
        return large_neighborhood_search(instance, generator, parameters);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

