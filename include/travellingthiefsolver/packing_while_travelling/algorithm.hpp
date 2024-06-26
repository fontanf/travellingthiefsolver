#pragma once

#include "travellingthiefsolver/packing_while_travelling/reduction.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

#include "optimizationtools//utils//output.hpp"
#include "optimizationtools//utils//utils.hpp"

#include <iomanip>

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

inline optimizationtools::ObjectiveDirection objective_direction()
{
    return optimizationtools::ObjectiveDirection::Maximize;
}

/**
 * Output structure for a set covering problem.
 */
struct Output: optimizationtools::Output
{
    /** Constructor. */
    Output(const Instance& instance):
        solution(SolutionBuilder().set_instance(instance).build()),
        bound(instance.total_item_profit()){ }


    /** Solution. */
    Solution solution;

    /** Bound. */
    Profit bound = std::numeric_limits<Profit>::infinity();

    /** Elapsed time. */
    double time = 0.0;


    std::string solution_value() const
    {
        return optimizationtools::solution_value(
            objective_direction(),
            solution.feasible(),
            solution.objective_value());
    }

    double absolute_optimality_gap() const
    {
        return optimizationtools::absolute_optimality_gap(
                objective_direction(),
                solution.feasible(),
                solution.objective_value(),
                bound);
    }

    double relative_optimality_gap() const
    {
       return optimizationtools::relative_optimality_gap(
            objective_direction(),
            solution.feasible(),
            solution.objective_value(),
            bound);
    }

    virtual nlohmann::json to_json() const
    {
        return nlohmann::json {
            {"Solution", solution.to_json()},
            {"Value", solution_value()},
            {"Bound", bound},
            {"AbsoluteOptimalityGap", absolute_optimality_gap()},
            {"RelativeOptimalityGap", relative_optimality_gap()},
            {"Time", time},
        };
    }

    virtual int format_width() const { return 30; }

    virtual void format(std::ostream& os) const
    {
        int width = format_width();
        os
            << std::setw(width) << std::left << "Value: " << solution_value() << std::endl
            << std::setw(width) << std::left << "Bound: " << bound << std::endl
            << std::setw(width) << std::left << "Absolute optimality gap: " << absolute_optimality_gap() << std::endl
            << std::setw(width) << std::left << "Relative optimality gap (%): " << relative_optimality_gap() * 100 << std::endl
            << std::setw(width) << std::left << "Time (s): " << time << std::endl
            ;
    }
};

using NewSolutionCallback = std::function<void(const Output&, const std::string&)>;

struct Parameters: optimizationtools::Parameters
{
    /** Callback function called when a new best solution is found. */
    NewSolutionCallback new_solution_callback = [](const Output&, const std::string&) { };

    /** Reduction parameters. */
    ReductionParameters reduction_parameters;


    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = optimizationtools::Parameters::to_json();
        json.merge_patch(
            {"Reduction",
                {"Enable", reduction_parameters.reduce},
                {"MaximumNumberOfRounds", reduction_parameters.maximum_number_of_rounds},
                {"ExpensiveReduction", reduction_parameters.enable_expensive_reduction},
                });
        return json;
    }

    virtual int format_width() const override { return 23; }

    virtual void format(std::ostream& os) const override
    {
        optimizationtools::Parameters::format(os);
        int width = format_width();
        os
            << "Reduction" << std::endl
            << std::setw(width) << std::left << "    Enable: " << reduction_parameters.reduce << std::endl
            << std::setw(width) << std::left << "    Max. # of rounds: " << reduction_parameters.maximum_number_of_rounds << std::endl
            << std::setw(width) << std::left << "    Expensive reduction: " << reduction_parameters.enable_expensive_reduction << std::endl
            ;
    }
};

}
}
