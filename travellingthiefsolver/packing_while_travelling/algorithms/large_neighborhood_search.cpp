#include "travellingthiefsolver/packing_while_travelling/algorithms/large_neighborhood_search.hpp"

#include "travellingthiefsolver/packing_while_travelling/algorithm_formatter.hpp"
#include "travellingthiefsolver/packing_while_travelling/instance_builder.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/greedy.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/sequential_value_correction.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

using namespace travellingthiefsolver::packing_while_travelling;

LargeNeighborhoodSearchOutput travellingthiefsolver::packing_while_travelling::large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        const LargeNeighborhoodSearchParameters& parameters)
{
    LargeNeighborhoodSearchOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Large neighborhood search");
    algorithm_formatter.print_header();

    // Reduction.
    if (parameters.reduction_parameters.reduce) {
        return solve_reduced_instance(
                [&generator](
                    const Instance& instance,
                    const LargeNeighborhoodSearchParameters& parameters)
                {
                    return large_neighborhood_search(
                            instance,
                            generator,
                            parameters);
                },
                instance,
                parameters,
                algorithm_formatter,
                output);
    }

    // Get an initial solution.
    GreedyParameters greedy_parameters;
    greedy_parameters.scoring_function = 3;
    //Solution solution_cur = greedy(instance, greedy_parameters).solution;
    SequentialValueCorrectionParameters svc_parameters;
    svc_parameters.verbosity_level = 0;
    Solution solution_cur = sequential_value_correction(instance, svc_parameters).solution;

    // Update output.
    algorithm_formatter.update_solution(solution_cur, "initial solution");

    optimizationtools::IndexedSet removed_items(instance.number_of_items());
    optimizationtools::IndexedSet fixed_items(instance.number_of_items());

    Counter number_of_iterations_without_improvement = 0;
    for (output.number_of_iterations = 0;
            !parameters.timer.needs_to_end();
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

        //GreedyParameters greedy_parameters;
        //greedy_parameters.scoring_function = 3;
        greedy_parameters.verbosity_level = 0;
        //Solution new_solution = greedy(new_instance, greedy_parameters).solution;
        SequentialValueCorrectionParameters svc_parameters;
        svc_parameters.verbosity_level = 0;
        Solution new_solution = sequential_value_correction(new_instance, svc_parameters).solution;

        // Retrieve solution.
        SolutionBuilder solution_builder;
        solution_builder.set_instance(instance);
        // Add fixed items.
        for (ItemId item_id: fixed_items)
            solution_builder.add_item(item_id);
        // Add items from the solution of the new instance.
        for (ItemId new_item_id = 0;
                new_item_id < new_instance.number_of_items();
                ++new_item_id) {
            if (new_solution.contains(new_item_id)) {
                ItemId item_id = items_new2orig[new_item_id];
                solution_builder.add_item(item_id);
            }
        }
        Solution solution = solution_builder.build();

        // Update output.
        std::stringstream ss;
        ss << "iteration " << output.number_of_iterations;
        algorithm_formatter.update_solution(solution, ss.str());

        if (optimizationtools::is_solution_strictly_better(
                    objective_direction(),
                    solution_cur.feasible(),
                    solution_cur.objective_value(),
                    solution.feasible(),
                    solution.objective_value())) {
            solution_cur = solution;
        }
    }

    algorithm_formatter.end();
    return output;
}
