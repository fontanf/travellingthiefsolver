#include "travellingthiefsolver/packing_while_travelling/algorithms/sequential_value_correction.hpp"

#include "travellingthiefsolver/packing_while_travelling/algorithm_formatter.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

#include "knapsacksolver/knapsack/instance_builder.hpp"
#include "knapsacksolver/knapsack/algorithms/dynamic_programming_primal_dual.hpp"

using namespace travellingthiefsolver::packing_while_travelling;

namespace
{

Solution solve(
        const Instance& instance,
        AlgorithmFormatter& algorithm_formatter,
        double alpha)
{
    // Build knapsack instance.
    std::vector<ItemId> kp2pwt;
    knapsacksolver::knapsack::InstanceFromFloatProfitsBuilder kp_instance_builder;
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        const City& city = instance.city(item.city_id);

        // Compute the profit of the item in the knapsack instance.
        Profit profit = item.profit - alpha * item.weight * city.distance_to_end;

        // Skip items with negative profit.
        if (profit <= 0)
            continue;

        // Add the item to the knapsack instance.
        kp_instance_builder.add_item(profit, item.weight);
        kp2pwt.push_back(item_id);
    }
    kp_instance_builder.set_capacity(instance.capacity() - instance.city_weight());
    knapsacksolver::knapsack::Instance kp_instance = kp_instance_builder.build();

    // Solve knapsack problem.
    knapsacksolver::knapsack::DynamicProgrammingPrimalDualParameters kp_parameters;
    kp_parameters.verbosity_level = 0;
    auto kp_output = knapsacksolver::knapsack::dynamic_programming_primal_dual(
            kp_instance,
            kp_parameters);
    //std::cout << "kp " << kp_output.solution.profit() << std::endl;

    // Retrieve solution.
    SolutionBuilder solution_builder;
    solution_builder.set_instance(instance);
    // Loop though knapsack items.
    for (knapsacksolver::knapsack::ItemId kp_item_id = 0;
            kp_item_id < kp_instance.number_of_items();
            ++kp_item_id) {
        if (kp_output.solution.contains(kp_item_id)) {
            // Retrieve the id of the item in the pwt instance.
            ItemId item_id = kp2pwt[kp_item_id];
            // Add the item to the solution.
            solution_builder.add_item(item_id);
        }
    }
    Solution solution = solution_builder.build();
    //std::cout << "solution"
    //    << " profit " << solution.item_profit()
    //    << " cost " << solution.renting_cost()
    //    << " obj " << solution.objective()
    //    << std::endl;

    // Update solution.
    algorithm_formatter.update_solution(solution, "");

    return solution;
}

}

Output travellingthiefsolver::packing_while_travelling::sequential_value_correction(
        const Instance& instance,
        const SequentialValueCorrectionParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Sequential value correction");
    algorithm_formatter.print_header();

    // Reduction.
    if (parameters.reduction_parameters.reduce)
        return solve_reduced_instance(sequential_value_correction, instance, parameters, algorithm_formatter, output);

    double alpha_min = 0;

    double alpha_max = 0;
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        const Item& item = instance.item(item_id);
        const City& city = instance.city(item.city_id);
        double a = item.profit / item.weight / city.distance_to_end;
        alpha_max = std::max(alpha_max, a);
    }

    Profit obj_min = solve(instance, algorithm_formatter, alpha_min).objective_value();
    Profit obj_max = solve(instance, algorithm_formatter, alpha_max).objective_value();

    for (Counter number_of_iterations = 0;
            number_of_iterations < 64;
            ++number_of_iterations) {

        if (alpha_min >= alpha_max)
            break;

        double alpha_1 = 0.6666 * alpha_min + 0.3333 * alpha_max;
        Profit obj_1 = solve(instance, algorithm_formatter, alpha_1).objective_value();
        double alpha_2 = 0.3333 * alpha_min + 0.6666 * alpha_max;
        Profit obj_2 = solve(instance, algorithm_formatter, alpha_2).objective_value();

        if (alpha_1 <= alpha_min
                || alpha_2 >= alpha_max
                || alpha_1 >= alpha_2) {
            break;
        }

        //std::cout << "alpha_min " << alpha_min
        //    << " alpha_1 " << alpha_1
        //    << " alpha_2 " << alpha_2
        //    << " alpha_max " << alpha_max
        //    << std::endl;

        //std::cout << "obj_min " << obj_min
        //    << " obj_1 " << obj_1
        //    << " obj_2 " << obj_2
        //    << " obj_max " << obj_max
        //    << std::endl;

        if (obj_min >= obj_1
                && obj_min >= obj_2
                && obj_min >= obj_max) {
            alpha_max = alpha_2;
            obj_max = obj_2;
        } else if (obj_1 >= obj_min
                && obj_1 >= obj_2
                && obj_1 >= obj_max) {
            alpha_max = alpha_2;
            obj_max = obj_2;
        } else if (obj_2 >= obj_min
                && obj_2 >= obj_1
                && obj_2 >= obj_max) {
            alpha_min = alpha_1;
            obj_min = obj_1;
        } else if (obj_max >= obj_min
                && obj_max >= obj_1
                && obj_max >= obj_2) {
            alpha_min = alpha_1;
            obj_min = obj_1;
        }
    }

    algorithm_formatter.end();
    return output;
}
