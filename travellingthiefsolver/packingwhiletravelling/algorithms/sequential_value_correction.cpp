#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"

#include "travellingthiefsolver/packingwhiletravelling/solution_builder.hpp"

#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

using namespace travellingthiefsolver::packingwhiletravelling;

Solution solve(
        const Instance& instance,
        SequentialValueCorrectionOptionalParameters& parameters,
        Output& output,
        double alpha)
{
    // Build knapsack instance.
    std::vector<ItemId> kp2pwt;
    knapsacksolver::Instance kp_instance;
    kp_instance.set_capacity(instance.capacity() - instance.city_weight());
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        const City& city = instance.city(item.city_id);
        Profit profit = item.profit - alpha * item.weight * city.distance_to_end;
        // Skip items with negative profit.
        if (profit <= 0)
            continue;
        knapsacksolver::Profit kp_profit = std::round(profit * 10000);
        kp_instance.add_item(item.weight, kp_profit);
        kp2pwt.push_back(item_id);
    }

    // Solve knapsack problem.
    knapsacksolver::DynamicProgrammingPrimalDualOptionalParameters kp_parameters;
    kp_parameters.set_combo();
    //kp_parameters.info.set_verbosity_level(1);
    auto kp_output = knapsacksolver::dynamic_programming_primal_dual(
            kp_instance,
            kp_parameters);
    //std::cout << "kp " << kp_output.solution.profit() << std::endl;

    // Retrieve solution.
    SolutionBuilder solution_builder(instance);
    for (knapsacksolver::ItemIdx kp_item_id = 0;
            kp_item_id < kp_instance.number_of_items();
            ++kp_item_id) {
        if (kp_output.solution.contains_idx(kp_item_id)) {
            ItemId item_id = kp2pwt[kp_item_id];
            solution_builder.add_item(item_id);
        }
    }
    Solution solution = solution_builder.build();
    //std::cout << "solution"
    //    << " profit " << solution.item_profit()
    //    << " cost " << solution.renting_cost()
    //    << " obj " << solution.objective()
    //    << std::endl;

    // Update output.
    std::stringstream ss;
    //ss << "iteration " << number_of_iterations;
    output.update_solution(solution, ss, parameters.info);

    return solution;
}

Output travellingthiefsolver::packingwhiletravelling::sequential_value_correction(
        const Instance& original_instance,
        SequentialValueCorrectionOptionalParameters parameters)
{
    init_display(original_instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Sequential value correction" << std::endl
        << std::endl;

    // Reduction.
    std::unique_ptr<Instance> reduced_instance = nullptr;
    if (parameters.reduction_parameters.reduce) {
        reduced_instance = std::unique_ptr<Instance>(
                new Instance(
                    original_instance.reduce(
                        parameters.reduction_parameters)));
        parameters.info.os()
            << "Reduced instance" << std::endl
            << "----------------" << std::endl;
        reduced_instance->print(parameters.info.os(), parameters.info.verbosity_level());
        parameters.info.os() << std::endl;
    }
    const Instance& instance = (reduced_instance == nullptr)? original_instance: *reduced_instance;

    Output output(original_instance, parameters.info);

    double alpha_min = 0;

    double alpha_max = 0;
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        const Item& item = instance.item(item_id);
        const City& city = instance.city(item.city_id);
        double a = item.profit / item.weight / city.distance_to_end;
        alpha_max = std::max(alpha_max, a);
    }

    Profit obj_min = solve(instance, parameters, output, alpha_min).objective();
    Profit obj_max = solve(instance, parameters, output, alpha_max).objective();

    for (Counter number_of_iterations = 0;
            number_of_iterations < 64;
            ++number_of_iterations) {

        if (alpha_min >= alpha_max)
            break;

        double alpha_1 = 0.6666 * alpha_min + 0.3333 * alpha_max;
        Profit obj_1 = solve(instance, parameters, output, alpha_1).objective();
        double alpha_2 = 0.3333 * alpha_min + 0.6666 * alpha_max;
        Profit obj_2 = solve(instance, parameters, output, alpha_2).objective();

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

    return output.algorithm_end(parameters.info);
}
