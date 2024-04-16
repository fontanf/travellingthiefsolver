#include "travellingthiefsolver/packing_while_travelling/algorithms/greedy.hpp"

#include "travellingthiefsolver/packing_while_travelling/algorithm_formatter.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

using namespace travellingthiefsolver::packing_while_travelling;

Output travellingthiefsolver::packing_while_travelling::greedy(
    const Instance& instance,
    const GreedyParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Greedy");
    algorithm_formatter.print_header();

    // Reduction.
    if (parameters.reduction_parameters.reduce)
        return solve_reduced_instance(greedy, instance, parameters, algorithm_formatter, output);

    // Compute scores
    std::vector<double> scores(instance.number_of_items(), 0);
    double upsilon = (instance.maximum_speed() - instance.minimum_speed()) / instance.capacity();
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        const Item& item = instance.item(item_id);
        const City& city = instance.city(item.city_id);
        switch (parameters.scoring_function) {
        case 0: {
            double speed_with = (instance.maximum_speed() - upsilon * (city.weight_from_start + item.weight));
            double time_with = city.distance_to_end / speed_with;
            Profit cost_with = instance.renting_ratio() * time_with;
            scores[item_id] = item.profit - cost_with;
            break;
        } case 1: {
            scores[item_id] = item.profit / (item.weight * city.distance_to_end);
            break;
        } case 2: {
            double speed_without = (instance.maximum_speed() - upsilon * (city.weight_from_start));
            double speed_with = (instance.maximum_speed() - upsilon * (city.weight_from_start + item.weight));
            double time_without = city.distance_to_end / speed_without;
            double time_with = city.distance_to_end / speed_with;
            double cost_without = instance.renting_ratio() * time_without;
            double cost_with = instance.renting_ratio() * time_with;
            scores[item_id] = item.profit - cost_with + cost_without;
            scores[item_id] /= item.weight;
            break;
        } case 3: {
            double speed_without = (instance.maximum_speed() - upsilon * (instance.total_weight() - item.weight));
            double speed_with = (instance.maximum_speed() - upsilon * (instance.total_weight()));
            double time_without = city.distance_to_end / speed_without;
            double time_with = city.distance_to_end / speed_with;
            double cost_without = instance.renting_ratio() * time_without;
            double cost_with = instance.renting_ratio() * time_with;
            scores[item_id] = item.profit - cost_with + cost_without;
            scores[item_id] /= item.weight;
            break;
        } default: {
            throw std::invalid_argument("");
        }
        }
    }

    // Sort the index of items, according to their score value
    std::vector<ItemId> sorted_item_ids(scores.size());
    std::iota(sorted_item_ids.begin(), sorted_item_ids.end(), 0);
    // Only sort if not all items fit into the knapsack.
    if (instance.total_weight() > instance.capacity()) {
        std::sort(
                sorted_item_ids.begin(),
                sorted_item_ids.end(),
                [&scores](ItemId item_id_1, ItemId item_id_2) -> bool
                {
                    return scores[item_id_1] > scores[item_id_2];
                });
    }

    // Add items while it is possible/profitable
    SolutionBuilder solution_builder;
    solution_builder.set_instance(instance);
    Weight weight = instance.city_weight();
    for (ItemId item_id: sorted_item_ids) {
        if (weight + instance.item(item_id).weight <= instance.capacity()) {
            // Add the item to knapsack
            solution_builder.add_item(item_id);
            weight += instance.item(item_id).weight;
        }

        if (weight == instance.capacity()) {
            break;
        }
    }
    Solution solution = solution_builder.build();

    // Update output.
    algorithm_formatter.update_solution(
            solution,
            "greedy solution");

    algorithm_formatter.end();
    return output;
}
