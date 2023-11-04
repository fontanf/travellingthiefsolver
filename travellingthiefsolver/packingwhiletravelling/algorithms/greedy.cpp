#include "travellingthiefsolver/packingwhiletravelling/algorithms/greedy.hpp"

#include "travellingthiefsolver/packingwhiletravelling/solution_builder.hpp"

#include <limits>
#include <iostream>

using namespace travellingthiefsolver::packingwhiletravelling;

Output travellingthiefsolver::packingwhiletravelling::greedy(
    const Instance &original_instance,
    GreedyOptionalParameters parameters)
{
    init_display(original_instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Greedy" << std::endl
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
    SolutionBuilder solution_builder(instance);
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
    std::stringstream ss;
    ss << "greedy solution";
    output.update_solution(
            solution,
            ss,
            parameters.info);

    return output.algorithm_end(parameters.info);
}
