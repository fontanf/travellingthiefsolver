#include "thieforienteeringsolver/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace thieforienteeringsolver;
namespace po = boost::program_options;

Output thieforienteeringsolver::run(
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

    } else if (algorithm_args[0] == "tree_search") {
        return tree_search(instance, info);

    } else if (algorithm_args[0] == "local_search") {
        return local_search(instance, info);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

