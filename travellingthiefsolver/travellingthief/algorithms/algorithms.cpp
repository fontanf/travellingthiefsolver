#include "travellingthiefsolver/travellingthief/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace travellingthiefsolver::travellingthief;
namespace po = boost::program_options;

Output travellingthiefsolver::travellingthief::run(
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

    } else if (algorithm_args[0] == "tree-search") {
        return tree_search(instance, info);

    } else if (algorithm_args[0] == "local-search") {
        return local_search(instance, info);

    } else if (algorithm_args[0] == "efficient-local-search") {
        EfficientLocalSearchOptionalParameters parameters;
        parameters.info = info;
        parameters.initial_solution = &initial_solution;
        return efficient_local_search(instance, generator, parameters);

    } else if (algorithm_args[0] == "efficient-genetic-local-search") {
        EfficientLocalSearchOptionalParameters parameters;
        parameters.info = info;
        return efficient_genetic_local_search(instance, generator, parameters);

    } else if (algorithm_args[0] == "iterative-tsp-pwt") {
        IterativeTspPwtOptionalParameters parameters;
        parameters.info = info;
        return iterative_tsp_pwt(instance, generator, parameters);

    } else if (algorithm_args[0] == "iterative-tsp-pwt-ttp") {
        IterativeTspPwtTtpOptionalParameters parameters;
        parameters.info = info;
        return iterative_tsp_pwt_ttp(instance, generator, parameters);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

