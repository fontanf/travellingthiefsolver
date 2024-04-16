#include "travellingthiefsolver/packing_while_travelling/reduction.hpp"

#include "travellingthiefsolver/packing_while_travelling/instance_builder.hpp"
#include "travellingthiefsolver/packing_while_travelling/solution_builder.hpp"

#include "optimizationtools/containers/indexed_set.hpp"

using namespace travellingthiefsolver::packing_while_travelling;

bool Reduction::reduce_unprofitable_items()
{
    optimizationtools::IndexedSet unprofitable_items(instance().number_of_items());

    for (ItemId item_id = 0;
            item_id < instance().number_of_items();
            ++item_id) {
        const Item& item = instance().item(item_id);
        const City& city = instance().city(item.city_id);
        double speed_after_without = instance().maximum_speed()
            - (double)(city.weight_from_start
                    * (instance().maximum_speed() - instance().minimum_speed()))
            / instance().capacity();
        double speed_after_with = instance().maximum_speed()
            - (double)((city.weight_from_start + item.weight)
                    * (instance().maximum_speed() - instance().minimum_speed()))
            / instance().capacity();
        Profit cost_without = instance().renting_ratio()
            * (double)city.distance_to_end / speed_after_without;
        Profit cost_with = instance().renting_ratio()
            * (double)city.distance_to_end / speed_after_with;
        Profit min_cost = cost_with - cost_without;
        if (item.profit <= min_cost) {
            unprofitable_items.add(item_id);
        }
    }

    if (unprofitable_items.empty())
        return false;

    std::vector<ItemId> new_unreduction_operations;
    std::vector<ItemId> new_mandatory_items;

    // Update mandatory_items.
    new_mandatory_items = mandatory_items_;

    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = instance().number_of_items()
        - unprofitable_items.size();
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(instance().number_of_cities());
    new_unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0;
            city_id < instance().number_of_cities();
            ++city_id) {
        const City& city = instance().city(city_id);
        new_instance_builder.set_distance(city_id, city.distance);
        new_instance_builder.add_weight(city_id, city.weight);
    }
    new_instance_builder.set_minimum_speed(instance().minimum_speed());
    new_instance_builder.set_maximum_speed(instance().maximum_speed());
    new_instance_builder.set_renting_ratio(instance().renting_ratio());
    new_instance_builder.set_capacity(instance().capacity());

    // Add items.
    ItemId new_item_id = 0;
    for (auto it = unprofitable_items.out_begin();
            it != unprofitable_items.out_end();
            ++it) {
        ItemId item_id = *it;
        const Item& item = instance().item(item_id);
        if (new_item_id == -1)
            continue;
        new_unreduction_operations[new_item_id]
            = unreduction_operations_[item_id];
        new_instance_builder.add_item(item.city_id, item.weight, item.profit);
        new_item_id++;
    }

    unreduction_operations_ = new_unreduction_operations;
    mandatory_items_ = new_mandatory_items;
    instance_ = new_instance_builder.build();
    return true;
}

bool Reduction::reduce_compulsory_items()
{
    optimizationtools::IndexedSet compulsory_items(instance().number_of_items());

    if (instance().total_weight() <= instance().capacity()) {
        for (ItemId item_id = 0; item_id < instance().number_of_items(); ++item_id) {
            const Item& item = instance().item(item_id);
            const City& city = instance().city(item.city_id);
            double speed_after_without = instance().maximum_speed()
                - (double)((instance().total_weight() - item.weight)
                        * (instance().maximum_speed() - instance().minimum_speed()))
                / instance().capacity();
            double speed_after_with = instance().maximum_speed()
                - (double)(instance().total_weight()
                        * (instance().maximum_speed() - instance().minimum_speed()))
                / instance().capacity();
            Profit cost_without = instance().renting_ratio()
                * (double)city.distance_to_end / speed_after_without;
            Profit cost_with = instance().renting_ratio()
                * (double)city.distance_to_end / speed_after_with;
            Profit max_cost = cost_with - cost_without;
            if (item.profit >= max_cost) {
                compulsory_items.add(item_id);
            }
        }
    }

    if (compulsory_items.empty())
        return false;

    std::vector<ItemId> new_unreduction_operations;
    std::vector<ItemId> new_mandatory_items;

    // Update mandatory_sets.
    new_mandatory_items = mandatory_items_;
    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = instance().number_of_items()
        - compulsory_items.size();
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(instance().number_of_cities());
    new_unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0;
            city_id < instance().number_of_cities();
            ++city_id) {
        const City& city = instance().city(city_id);
        new_instance_builder.set_distance(city_id, city.distance);
        new_instance_builder.add_weight(city_id, city.weight);
    }
    new_instance_builder.set_minimum_speed(instance().minimum_speed());
    new_instance_builder.set_maximum_speed(instance().maximum_speed());
    new_instance_builder.set_renting_ratio(instance().renting_ratio());
    new_instance_builder.set_capacity(instance().capacity());

    // Update mandatory_items.
    for (ItemId item_id: compulsory_items) {
        const Item& item = instance().item(item_id);
        new_instance_builder.add_weight(item.city_id, item.weight);
        ItemId orig_item_id = unreduction_operations_[item_id];
        new_mandatory_items.push_back(orig_item_id);
    }
    // Add items.
    ItemId new_item_id = 0;
    for (auto it = compulsory_items.out_begin();
            it != compulsory_items.out_end();
            ++it) {
        ItemId item_id = *it;
        const Item& item = instance().item(item_id);
        if (new_item_id == -1)
            continue;
        new_unreduction_operations[new_item_id] = unreduction_operations_[item_id];
        new_instance_builder.add_item(item.city_id, item.weight, item.profit);
        new_item_id++;
    }

    unreduction_operations_ = new_unreduction_operations;
    mandatory_items_ = new_mandatory_items;
    instance_ = new_instance_builder.build();
    return true;
}

bool Reduction::reduce_polyakovskiy2017(
        Counter maximum_number_of_rounds)
{
    optimizationtools::IndexedSet unprofitable_items(instance().number_of_items());
    optimizationtools::IndexedSet compulsory_items(instance().number_of_items());

    std::vector<double> profit_prime(instance().number_of_items(), 0);
    for (ItemId item_id = 0;
            item_id < instance().number_of_items();
            ++item_id) {
        profit_prime[item_id] = instance().item(item_id).profit;
    }

    std::vector<Weight> w_max(instance().number_of_cities(), 0);
    std::vector<Weight> w_comp(instance().number_of_cities(), 0);

    double ups = (instance().maximum_speed() - instance().minimum_speed())
        / instance().capacity();

    for (Counter round_number = 0;
            round_number < maximum_number_of_rounds;
            ++round_number) {
        //std::cout << "round_number " << round_number
        //    << " number_of_compulsory_items " << number_of_compulsory_items
        //    << " number_of_unprofitable_items " << number_of_unprofitable_items
        //    << std::endl;

        // Flag to determine if an object has been found as
        // compulsory/unprofitable
        bool found = false;

        // Compute compulsory items.

        // Compute maximal possible weight of items that can be collected in
        // each city.
        w_max[0] = 0;
        Weight w_sum = 0;
        for (CityId city_id = 1;
                city_id < instance().number_of_cities();
                ++city_id) {
            const City& city = instance().city(city_id);

            w_max[city_id] = w_max[city_id - 1] + city.weight;
            w_sum += city.weight;

            // Loop through the items of the city.
            for (ItemId item_id: instance().city(city_id).item_ids) {
                const Item& item = instance().item(item_id);

                // Don't consider unprofitable items.
                if (unprofitable_items.contains(item_id))
                    continue;

                // Update w_max.
                w_sum += item.weight;
                w_max[city_id] += item.weight;
                if (w_max[city_id] > instance().capacity())
                    w_max[city_id] = instance().capacity();
            }
        }

        if (w_sum <= instance().capacity()) {

            // Loop through the cities in reverse order.
            for (CityId city_id_1 = instance().number_of_cities() - 1;
                    city_id_1 >= 0;
                    --city_id_1) {
                const City& city_1 = instance().city(city_id_1);

                // Loop through the items of the city.
                for (ItemId item_pos_1 = 0;
                        item_pos_1 < (ItemId)city_1.item_ids.size();
                        ++item_pos_1) {
                    ItemId item_id_1 = city_1.item_ids[item_pos_1];
                    const Item& item_1 = instance().item(item_id_1);

                    if (unprofitable_items.contains(item_id_1)
                            || compulsory_items.contains(item_id_1)) {
                        continue;
                    }

                    double c_max = 0;
                    double c_min = 0;
                    bool flag_prime = false;

                    for (CityId city_id_2 = city_id_1;
                            city_id_2 < instance().number_of_cities();
                            ++city_id_2) {
                        const City& city_2 = instance().city(city_id_2);

                        ItemId pos_max = (city_id_2 != city_id_1)?
                            city_2.item_ids.size():
                            item_pos_1;
                        for (ItemId item_pos_2 = 0;
                                item_pos_2 < pos_max;
                                ++item_pos_2) {
                            ItemId item_id_2 = city_2.item_ids[item_pos_2];
                            const Item& item_2 = instance().item(item_id_2);

                            // Only consider non unprofitable items.
                            if (unprofitable_items.contains(item_id_2))
                                continue;

                            // If true, item is not compulsory.
                            if (!compulsory_items.contains(item_id_2)
                                    && item_1.weight >= item_2.weight
                                    && item_1.profit - c_min <= profit_prime[item_id_2]) {
                                profit_prime[item_id_1] = profit_prime[item_id_2] + c_min;
                                flag_prime = true;
                                break;
                            }

                            // If true, item is compulsory.
                            if (compulsory_items.contains(item_id_2)
                                    && item_1.weight <= item_2.weight
                                    && item_1.profit - c_max > profit_prime[item_id_2]) {
                                compulsory_items.add(item_id_1);
                                profit_prime[item_id_1] = profit_prime[item_id_2] + c_max;
                                found = true;
                                flag_prime = true;
                                break;
                            }
                        }

                        if (flag_prime)
                            break;

                        // Distance between city j and city j + 1
                        double distance_to_next = (city_id_2 == instance().number_of_cities() - 1)?
                            instance().city(0).distance:
                            instance().city(city_id_2 + 1).distance;

                        // Update least/most increamental costs
                        c_min += instance().renting_ratio() * (double)distance_to_next
                            / (instance().maximum_speed() - ups * (w_comp[city_id_2] + item_1.weight))
                            - instance().renting_ratio() * (double)distance_to_next
                            / (instance().maximum_speed() - ups * w_comp[city_id_2]);
                        c_max += instance().renting_ratio() * (double)distance_to_next
                            / (instance().maximum_speed() - ups * w_max[city_id_2])
                            - instance().renting_ratio() * (double)distance_to_next
                            / (instance().maximum_speed() - ups * (w_max[city_id_2] - item_1.weight));
                    }

                    if (flag_prime) {
                        continue;
                    }

                    // If the item is compulsory indenpendently of any other item.
                    profit_prime[item_id_1] = c_max;
                    if (c_max < item_1.profit) {
                        compulsory_items.add(item_id_1);
                        found = true;
                    }
                }
            }
        }

        // Compute unprofitable items

        // Compute weight of compulsory items in all current and preceding cities
        w_comp[0] = 0;
        for (CityId city_id = 1;
                city_id < instance().number_of_cities();
                ++city_id) {
            const City& city = instance().city(city_id);

            w_comp[city_id] = w_comp[city_id - 1] + city.weight;

            for (ItemId item_id: city.item_ids) {
                if (!compulsory_items.contains(item_id))
                    continue;
                w_comp[city_id] += instance().item(item_id).weight;
            }
        }

        // Loop through the cities in reverse order.
        for (CityId city_id_1 = instance().number_of_cities() - 1;
                city_id_1 > 0;
                --city_id_1) {
            const City& city_1 = instance().city(city_id_1);

            // Loop through the items of the city.
            for (ItemId item_pos_1 = 0;
                    item_pos_1 < (ItemId)city_1.item_ids.size();
                    ++item_pos_1) {
                ItemId item_id_1 = city_1.item_ids[item_pos_1];
                const Item& item_1 = instance().item(item_id_1);

                if (unprofitable_items.contains(item_id_1)
                        || compulsory_items.contains(item_id_1)) {
                    continue;
                }

                double c_max = 0;
                double c_min = 0;
                bool flag_prime = false;

                for (CityId city_id_2 = city_id_1;
                        city_id_2 < instance().number_of_cities();
                        ++city_id_2) {
                    const City& city_2 = instance().city(city_id_2);

                    ItemId pos_max = (city_id_2 != city_id_1)?
                        city_2.item_ids.size():
                        item_pos_1;
                    for (ItemId item_pos_2 = 0;
                           item_pos_2 < pos_max;
                           ++item_pos_2) {
                        ItemId item_id_2 = city_2.item_ids[item_pos_2];
                        const Item& item_2 = instance().item(item_id_2);

                        // Only consider non compulsory items
                        if (compulsory_items.contains(item_id_2))
                            continue;

                        // If true, item is not unprofitable
                        if (!unprofitable_items.contains(item_id_2)
                                && item_1.weight <= item_2.weight
                                && item_1.profit - c_max > profit_prime[item_id_2]) {
                            profit_prime[item_id_1] = profit_prime[item_id_2] + c_max;
                            flag_prime = true;
                            break;
                        }

                        // If ture, item is unprofitable
                        if (unprofitable_items.contains(item_id_2)
                                && item_1.weight >= item_2.weight
                                && item_1.profit - c_min <= profit_prime[item_id_2]) {
                            unprofitable_items.add(item_id_1);
                            profit_prime[item_id_1] = profit_prime[item_id_2] + c_min;
                            found = true;
                            flag_prime = true;
                            break;
                        }
                    }

                    if (flag_prime)
                        break;

                    // Distance between city j and city j + 1
                    double distance_to_next = (city_id_2 == instance().number_of_cities() - 1)?
                        instance().city(0).distance:
                        instance().city(city_id_2 + 1).distance;

                    c_min += instance().renting_ratio() * (double)distance_to_next
                        / (instance().maximum_speed() - ups * (w_comp[city_id_2] + item_1.weight))
                        - instance().renting_ratio() * (double)distance_to_next
                        / (instance().maximum_speed() - ups * w_comp[city_id_2]);

                    // If item is unprofitable by itself.
                    if (c_min >= item_1.profit) {
                        unprofitable_items.add(item_id_1);
                        profit_prime[item_id_1] = c_min;
                        found = true;
                        flag_prime = true;
                        break;
                    }

                    c_max += instance().renting_ratio() * (double)distance_to_next
                        / (instance().maximum_speed() - ups * w_max[city_id_2])
                        - instance().renting_ratio() * (double)distance_to_next
                        / (instance().maximum_speed() - ups * (w_max[city_id_2] - item_1.weight));
                }

                if (flag_prime)
                    continue;;
                profit_prime[item_id_1] = c_min;
            }
        }

        if (!found)
            break;
    }

    if (compulsory_items.empty()
            && unprofitable_items.empty()) {
        return false;
    }

    std::vector<ItemId> new_unreduction_operations;
    std::vector<ItemId> new_mandatory_items;

    // Update mandatory_sets.
    new_mandatory_items = mandatory_items_;
    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = instance().number_of_items()
        - compulsory_items.size()
        - unprofitable_items.size();
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(instance().number_of_cities());
    new_unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0;
            city_id < instance().number_of_cities();
            ++city_id) {
        const City& city = instance().city(city_id);
        new_instance_builder.set_distance(city_id, city.distance);
        new_instance_builder.add_weight(city_id, city.weight);
    }
    new_instance_builder.set_minimum_speed(instance().minimum_speed());
    new_instance_builder.set_maximum_speed(instance().maximum_speed());
    new_instance_builder.set_renting_ratio(instance().renting_ratio());
    new_instance_builder.set_capacity(instance().capacity());

    // Update mandatory_items.
    for (ItemId item_id: compulsory_items) {
        const Item& item = instance().item(item_id);
        new_instance_builder.add_weight(item.city_id, item.weight);
        ItemId orig_item_id = unreduction_operations_[item_id];
        new_mandatory_items.push_back(orig_item_id);
    }

    // Add items.
    ItemId new_item_id = 0;
    for (ItemId item_id = 0; item_id < instance().number_of_items(); ++item_id) {
        if (!compulsory_items.contains(item_id)
                && !unprofitable_items.contains(item_id)) {
            const Item& item = instance().item(item_id);
            if (new_item_id == -1)
                continue;
            new_unreduction_operations[new_item_id] = unreduction_operations_[item_id];
            new_instance_builder.add_item(item.city_id, item.weight, item.profit);
            new_item_id++;
        }
    }

    unreduction_operations_ = new_unreduction_operations;
    mandatory_items_ = new_mandatory_items;
    instance_ = new_instance_builder.build();
    return true;
}

Reduction::Reduction(
        const Instance& instance,
        const ReductionParameters& parameters):
    original_instance_(&instance),
    instance_(instance)
{
    // Initialize reduced instance.
    unreduction_operations_ = std::vector<ItemId>(instance.number_of_items());
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        unreduction_operations_[item_id] = item_id;
    }

    if (!parameters.enable_expensive_reduction) {
        for (Counter round_number = 0;
                round_number < parameters.maximum_number_of_rounds;
                ++round_number) {
            bool found = false;
            found |= reduce_unprofitable_items();
            found |= reduce_compulsory_items();
            if (!found)
                break;
        }
    } else {
        reduce_polyakovskiy2017(
                parameters.maximum_number_of_rounds);
    }

    extra_profit_ = 0;
    for (ItemId orig_item_id: mandatory_items_)
        extra_profit_ += instance.item(orig_item_id).profit;
}

Solution Reduction::unreduce_solution(
        const Solution& solution) const
{
    SolutionBuilder new_solution_builder;
    new_solution_builder.set_instance(*original_instance_);

    for (ItemId item_id: mandatory_items_)
        new_solution_builder.add_item(item_id);

    for (ItemId item_id = 0;
            item_id < instance().number_of_items();
            ++item_id) {
        if (solution.contains(item_id)) {
            ItemId item_id_2 = unreduction_operations_[item_id];
            new_solution_builder.add_item(item_id_2);
        }
    }

    return new_solution_builder.build();
}

Profit Reduction::unreduce_bound(
        Profit bound) const
{
    return extra_profit_ + bound;
}
