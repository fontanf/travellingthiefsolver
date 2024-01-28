#include "travellingthiefsolver/travellingthief/algorithm_formatter.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <iomanip>

using namespace travellingthiefsolver::travellingthief;

void AlgorithmFormatter::start(
        const std::string& algorithm_name)
{
    output_.json["Parameters"] = parameters_.to_json();

    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << "=====================================" << std::endl
        << "        TravellingThiefSolver        " << std::endl
        << "=====================================" << std::endl
        << std::endl
        << "Problem" << std::endl
        << "-------" << std::endl
        << "Travelling thief problem" << std::endl
        << std::endl
        << "Instance" << std::endl
        << "--------" << std::endl;
    output_.solution.instance().format(*os_, parameters_.verbosity_level);
    *os_
        << std::endl
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << algorithm_name << std::endl
        << std::endl
        << "Parameters" << std::endl
        << "----------" << std::endl;
    parameters_.format(*os_);
}

void AlgorithmFormatter::print_header()
{
    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << std::right
        << std::endl
        << std::setw(12) << "Time (s)"
        << std::setw(12) << "Distance"
        << std::setw(12) << "Cost"
        << std::setw(12) << "# items"
        << std::setw(12) << "Profit"
        << std::setw(12) << "Objective"
        << std::setw(24) << "Comment"
        << std::endl
        << std::setw(12) << "--------"
        << std::setw(12) << "--------"
        << std::setw(12) << "----"
        << std::setw(12) << "-------"
        << std::setw(12) << "------"
        << std::setw(12) << "---------"
        << std::setw(24) << "-------"
        << std::endl;
    print("");
}

void AlgorithmFormatter::print(
        const std::string& s)
{
    if (parameters_.verbosity_level == 0)
        return;
    std::streamsize precision = std::cout.precision();
    *os_
        << std::setw(12) << std::fixed << std::setprecision(3) << output_.time << std::defaultfloat << std::setprecision(precision)
        << std::setw(12) << output_.solution.distance()
        << std::setw(12) << output_.solution.renting_cost()
        << std::setw(12) << output_.solution.number_of_items()
        << std::setw(12) << output_.solution.item_profit()
        << std::setw(12) << output_.solution.objective_value()
        << std::setw(24) << s << std::endl;
}

void AlgorithmFormatter::update_solution(
        const Solution& solution,
        const std::string& s)
{
    if (optimizationtools::is_solution_strictly_better(
                objective_direction(),
                output_.solution.feasible(),
                output_.solution.objective_value(),
                solution.feasible(),
                solution.objective_value())) {
        output_.time = parameters_.timer.elapsed_time();
        output_.solution = solution;
        print(s);
        output_.json["IntermediaryOutputs"].push_back(output_.to_json());
        parameters_.new_solution_callback(output_, s);
    }
}

void AlgorithmFormatter::update_bound(
        Profit bound,
        const std::string& s)
{
    if (optimizationtools::is_bound_strictly_better(
            objective_direction(),
            output_.bound,
            bound)) {
        output_.time = parameters_.timer.elapsed_time();
        output_.bound = bound;
        print(s);
        output_.json["IntermediaryOutputs"].push_back(output_.to_json());
        parameters_.new_solution_callback(output_, s);
    }
}

void AlgorithmFormatter::end()
{
    output_.time = parameters_.timer.elapsed_time();
    output_.json["Output"] = output_.to_json();

    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << std::endl
        << "Final statistics" << std::endl
        << "----------------" << std::endl;
    output_.format(*os_);
    *os_
        << std::endl
        << "Solution" << std::endl
        << "--------" << std::endl;
    output_.solution.format(*os_, parameters_.verbosity_level);
}
