#include "travellingthiefsolver/packing_while_travelling/algorithms/dynamic_programming.hpp"

#include "travellingthiefsolver/packing_while_travelling/algorithm_formatter.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

using namespace travellingthiefsolver::packing_while_travelling;

Output travellingthiefsolver::packing_while_travelling::dynamic_programming(
        const Instance& instance,
        const DynamicProgrammingParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic Programming");
    algorithm_formatter.print_header();

    // Reduction.
    if (parameters.reduction_parameters.reduce)
        return solve_reduced_instance(dynamic_programming, instance, parameters, algorithm_formatter, output);

    ItemId number_of_rows = 1 + instance.number_of_cities() + instance.number_of_items();
    std::vector<std::vector<double>> beta(
            number_of_rows,
            std::vector<double>(
                instance.capacity() + 1,
                -std::numeric_limits<double>::infinity()));

    // Initialize table.
    beta[0][0]
        = -instance.renting_ratio()
        * instance.city(0).distance_to_end
        / instance.maximum_speed();

    // compute the rest of the table
    ItemId row = 1;
    std::vector<std::pair<bool, ItemId>> rows(number_of_rows, {false, -1});
    Weight weight_min = 0;
    Weight weight_max = 0;
    for (CityId city_id = 0;
            city_id < instance.number_of_cities();
            ++city_id) {
        const City& city = instance.city(city_id);

        weight_min += city.weight;
        weight_max = std::min(weight_max + city.weight, instance.capacity());
        for (Weight weight = weight_min; weight <= weight_max; ++weight) {
            if (weight - city.weight < 0
                    || (beta[row - 1][weight - city.weight]
                        == -std::numeric_limits<double>::infinity())) {
                beta[row][weight] = -std::numeric_limits<double>::infinity();
            } else {
                double speed_with
                    = instance.maximum_speed()
                    - (double)weight
                    * (instance.maximum_speed() - instance.minimum_speed())
                    / instance.capacity();
                double speed_without
                    = instance.maximum_speed()
                    - (double)(weight - city.weight)
                    * (instance.maximum_speed() - instance.minimum_speed())
                    / instance.capacity();
                Profit cost_with
                    = instance.renting_ratio()
                    * (double)city.distance_to_end
                    / speed_with;
                Profit cost_without
                    = instance.renting_ratio()
                    * (double)city.distance_to_end
                    / speed_without;
                double value
                    = beta[row - 1][weight - city.weight]
                    - cost_with
                    + cost_without;
                beta[row][weight] = value;
            }
        }
        rows[row] = {true, city_id};
        row++;

        for (ItemId item_id: city.item_ids) {
            const Item& item = instance.item(item_id);
            weight_max = std::min(weight_max + item.weight, instance.capacity());
            for (Weight weight = weight_min;
                    weight <= weight_max;
                    ++weight) {
                if (weight - item.weight < 0
                        || (beta[row - 1][weight - item.weight]
                            == -std::numeric_limits<double>::infinity())) {
                    beta[row][weight] = beta[row - 1][weight];
                } else {
                    double speed_with
                        = instance.maximum_speed()
                        - (double)weight
                        * (instance.maximum_speed() - instance.minimum_speed())
                        / instance.capacity();
                    double speed_without
                        = instance.maximum_speed()
                        - (double)(weight - item.weight)
                        * (instance.maximum_speed() - instance.minimum_speed())
                        / instance.capacity();
                    Profit cost_with
                        = instance.renting_ratio()
                        * (double)city.distance_to_end
                        / speed_with;
                    Profit cost_without
                        = instance.renting_ratio()
                        * (double)city.distance_to_end
                        / speed_without;
                    double value
                        = beta[row - 1][weight - item.weight]
                        + item.profit
                        - cost_with
                        + cost_without;
                    beta[row][weight] = std::max(
                            beta[row - 1][weight],
                            value);
                }
            }
            rows[row] = {false, item_id};
            row++;
        }
    }

    // For each i and k, the entry β(i,k) stores the maximal possible benefit b(S) over all subsets S of {1,...,i} having weight exactly k.
    // In particular, maxk β(n, k) is the value of an optimal solution, which can be obtained via backtracking.
    double optimal_weight = 0;
    for (Weight weight = 0; weight <= instance.capacity(); ++weight) {
        if (beta[number_of_rows - 1][optimal_weight]
                < beta[number_of_rows - 1][weight]) {
            optimal_weight = weight;
        }
    }

    // backtracking
    SolutionBuilder solution_builder;
    solution_builder.set_instance(instance);
    Weight current_weight = optimal_weight;
    for (ItemId row = number_of_rows - 1; row > 0; --row) {
        if (rows[row].first) {
            // City.
            CityId city_id = rows[row].second;
            current_weight -= instance.city(city_id).weight;
        } else {
            // Item.
            ItemId item_id = rows[row].second;
            if (beta[row][current_weight]
                    == beta[row - 1][current_weight]) {
            } else {
                solution_builder.add_item(item_id);
                current_weight -= instance.item(item_id).weight;
            }
        }
    }

    // create the solution
    Solution solution = solution_builder.build();

    // Update output.
    algorithm_formatter.update_solution(solution, "final solution");

    algorithm_formatter.end();
    return output;
}
