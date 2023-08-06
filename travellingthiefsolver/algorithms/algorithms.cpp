#include "travellingthiefsolver/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver;
namespace po = boost::program_options;

Output travellingthiefsolver::run(
        std::string algorithm,
        const Instance& instance,
        const Solution& initial_solution,
        std::mt19937_64& generator,
        optimizationtools::Info info)
{
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for (Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm.empty() || algorithm_args[0].empty()) {
        throw std::invalid_argument("Missing algorithm.");

    } else if (algorithm_args[0] == "local_search") {
        return local_search(instance, info);

    } else if (algorithm_args[0] == "efficient_local_search") {
        EfficientLocalSearchOptionalParameters parameters;
        parameters.info = info;
        parameters.initial_solution = &initial_solution;
        return efficient_local_search(instance, generator, parameters);

    } else if (algorithm_args[0] == "efficient_genetic_local_search") {
        EfficientLocalSearchOptionalParameters parameters;
        parameters.info = info;
        return efficient_genetic_local_search(instance, generator, parameters);

    } else if (algorithm_args[0] == "iterative_tsp_pwt") {
        IterativeTspPwtOptionalParameters parameters;
        parameters.info = info;
        return iterative_tsp_pwt(instance, generator, parameters);

    } else if (algorithm_args[0] == "iterative_tsp_pwt_ttp") {
        IterativeTspPwtTtpOptionalParameters parameters;
        parameters.info = info;
        return iterative_tsp_pwt_ttp(instance, generator, parameters);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

