#include "travellingthiefsolver/packingwhiletravelling/algorithms/large_neighborhood_search.hpp"

#include "travellingthiefsolver/packingwhiletravelling/instance_builder.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/greedy.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

using namespace travellingthiefsolver::packingwhiletravelling;

void LargeNeighborhoodSearchOutput::print_statistics(
        optimizationtools::Info& info) const
{
    if (info.verbosity_level() >= 1) {
        info.os() << "Number of iterations:         " << number_of_iterations << std::endl;
    }
    info.add_to_json("Algorithm", "NumberOfIterations", number_of_iterations);
}

LargeNeighborhoodSearchOutput travellingthiefsolver::packingwhiletravelling::large_neighborhood_search(
        const Instance& original_instance,
        std::mt19937_64& generator,
        LargeNeighborhoodSearchOptionalParameters parameters)
{
    init_display(original_instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Large neighborhood search" << std::endl
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

    LargeNeighborhoodSearchOutput output(original_instance, parameters.info);

    // Get an initial solution.
    GreedyOptionalParameters greedy_parameters;
    greedy_parameters.scoring_function = 3;
    //Solution solution_cur = greedy(instance, greedy_parameters).solution;
    Solution solution_cur = sequential_value_correction(instance).solution;

    // Update output.
    std::stringstream ss;
    ss << "initial solution";
    output.update_solution(solution_cur, ss, parameters.info);

    optimizationtools::IndexedSet removed_items(instance.number_of_items());
    optimizationtools::IndexedSet fixed_items(instance.number_of_items());

    Counter number_of_iterations_without_improvement = 0;
    for (output.number_of_iterations = 0; !parameters.info.needs_to_end();
            ++output.number_of_iterations,
            ++number_of_iterations_without_improvement) {
        // Check stop criteria.
        if (parameters.maximum_number_of_iterations != -1
                && output.number_of_iterations >= parameters.maximum_number_of_iterations)
            break;
        if (parameters.maximum_number_of_iterations_without_improvement != -1
                && number_of_iterations_without_improvement >= parameters.maximum_number_of_iterations_without_improvement)
            break;

        // Ruin.
        removed_items.clear();
        fixed_items.clear();
        if (true) {
            std::uniform_real_distribution<double> d(0, 1);
            for (ItemId item_id = 0;
                    item_id < instance.number_of_items();
                    ++item_id) {
                if (solution_cur.contains(item_id)) {
                    double p = d(generator);
                    if (p < 0.8) {
                        fixed_items.add(item_id);
                    }
                }
            }
        } else if (true) {
            std::uniform_real_distribution<double> d(0, 1);
            for (CityId city_id = 0;
                    city_id < instance.number_of_cities();
                    ++city_id) {
                const City& city = instance.city(city_id);
                double p = d(generator);
                if (p < 0.9) {
                    for (ItemId item_id: city.item_ids) {
                        if (solution_cur.contains(item_id)) {
                            fixed_items.add(item_id);
                        } else {
                            removed_items.add(item_id);
                        }
                    }
                }
            }
        } else if (true) {
            CityId length = instance.number_of_cities() / 10;
            std::uniform_int_distribution<CityId> d(1, instance.number_of_cities() - length);
            CityId city_id_start = d(generator);
            for (CityId city_id = 0;
                    city_id < instance.number_of_cities();
                    ++city_id) {
                const City& city = instance.city(city_id);
                if (city_id < city_id_start
                        || city_id >= city_id_start + length) {
                    for (ItemId item_id: city.item_ids) {
                        if (solution_cur.contains(item_id)) {
                            fixed_items.add(item_id);
                        } else {
                            removed_items.add(item_id);
                        }
                    }
                }
            }
        }

        // Recreate.
        // Build new instance.
        std::vector<ItemId> items_new2orig;
        InstanceBuilder new_instance_builder;
        new_instance_builder.add_cities(instance.number_of_cities());
        for (CityId city_id = 0;
                city_id < instance.number_of_cities();
                ++city_id) {
            new_instance_builder.set_distance(
                    city_id,
                    instance.city(city_id).distance);
            new_instance_builder.add_weight(
                    city_id,
                 instance.city(city_id).weight);
        }
        new_instance_builder.set_minimum_speed(instance.minimum_speed());
        new_instance_builder.set_maximum_speed(instance.maximum_speed());
        new_instance_builder.set_renting_ratio(instance.renting_ratio());
        new_instance_builder.set_capacity(instance.capacity());
        // Update mandatory_items.
        for (ItemId item_id: fixed_items) {
            const Item& item = instance.item(item_id);
            new_instance_builder.add_weight(item.city_id, item.weight);
        }
        // Add items.
        for (ItemId item_id = 0;
                item_id < instance.number_of_items();
                ++item_id) {
            if (fixed_items.contains(item_id)
                    || removed_items.contains(item_id))
                continue;
            const Item& item = instance.item(item_id);
            items_new2orig.push_back(item_id);
            new_instance_builder.add_item(
                    item.city_id,
                    item.weight,
                    item.profit);
        }
        Instance new_instance = new_instance_builder.build();

        //GreedyOptionalParameters greedy_parameters;
        //greedy_parameters.scoring_function = 3;
        //greedy_parameters.info.set_verbosity_level(1);
        //Solution new_solution = greedy(new_instance, greedy_parameters).solution;
        SequentialValueCorrectionOptionalParameters svc_parameters;
        //svc_parameters.info.set_verbosity_level(1);
        Solution new_solution = sequential_value_correction(new_instance, svc_parameters).solution;

        // Retrieve solution.
        std::vector<uint8_t> items(instance.number_of_items());
        // Add fixed items.
        for (ItemId item_id: fixed_items)
            items[item_id] = 1;
        // Add items from the solution of the new instance.
        for (ItemId new_item_id = 0;
                new_item_id < new_instance.number_of_items();
                ++new_item_id) {
            if (new_solution.contains(new_item_id)) {
                ItemId item_id = items_new2orig[new_item_id];
                items[item_id] = 1;
            }
        }
        Solution solution(instance, items);

        // Update output.
        std::stringstream ss;
        ss << "iteration " << output.number_of_iterations;
        output.update_solution(solution, ss, parameters.info);

        if (solution.is_strictly_better_than(solution_cur))
            solution_cur = solution;
    }

    output.algorithm_end(parameters.info);
    return output;
}
