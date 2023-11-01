#include "travellingthiefsolver/packingwhiletravelling/algorithms/dynamic_programming.hpp"

using namespace travellingthiefsolver::packingwhiletravelling;

Output travellingthiefsolver::packingwhiletravelling::dynamic_programming(
        const Instance& original_instance,
        DynamicProgrammingOptionalParameters parameters)
{
    init_display(original_instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Dynamic Programming" << std::endl
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
    std::vector<u_int8_t> items(instance.number_of_items(), 0);
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
                items[item_id] = 0;
            } else {
                items[item_id] = 1;
                current_weight -= instance.item(item_id).weight;
            }
        }
    }

    // create the solution
    Solution solution(instance, items);

    // Update output.
    std::stringstream ss;
    ss << "final solution";
    output.update_solution(solution, ss, parameters.info);

    return output.algorithm_end(parameters.info);
}
