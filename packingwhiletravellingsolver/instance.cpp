#include "packingwhiletravellingsolver/instance.hpp"

#include "packingwhiletravellingsolver/instance_builder.hpp"

#include "optimizationtools/utils/utils.hpp"
#include "optimizationtools/containers/indexed_set.hpp"

using namespace packingwhiletravellingsolver;

std::ostream& Instance::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of cities:  " << number_of_cities() << std::endl
            << "Number of items:   " << number_of_items() << std::endl
            << "Distance:          " << distance() << std::endl
            << "Capacity:          " << capacity() << std::endl
            << "Minimum speed:     " << speed_min_ << std::endl
            << "Maximum speed:     " << speed_max_ << std::endl
            << "Renting ratio:     " << renting_ratio_ << std::endl
            << "City weight:       " << city_weight() << std::endl
            << "Weight sum:        " << weight_sum_ << std::endl
            << "Weight ratio:      " << (double)weight_sum_ / capacity() << std::endl
            << "Extra profit:      " << unreduction_info_.extra_profit << std::endl
            ;
    }

    if (verbose >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::setw(12) << "Distance"
            << std::setw(12) << "# items"
            << std::endl
            << std::setw(12) << "------"
            << std::setw(12) << "--------"
            << std::setw(12) << "-------"
            << std::endl;
        for (CityId city_id = 0;
                city_id < number_of_cities();
                ++city_id) {
            const City& city = this->city(city_id);
            os
                << std::setw(12) << city_id
                << std::setw(12) << city.distance
                << std::setw(12) << city.item_ids.size()
                << std::endl;
        }
    }

    if (verbose >= 2) {
        os << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "City"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "--------"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id = 0;
                item_id < number_of_items();
                ++item_id) {
            const Item& item = this->item(item_id);
            os
                << std::setw(12) << item_id
                << std::setw(12) << item.city_id
                << std::setw(12) << item.weight
                << std::setw(12) << item.profit
                << std::endl;
        }
    }

    return os;
}

void Instance::write(
        std::string instance_path) const
{
    if (instance_path.empty())
        return;
    std::ofstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    file << "NAME: XXX" << std::endl;
    file << "COMMENT: generated by fontanf/travellingthiefsolver" << std::endl;
    file << "TYPE: TSP" << std::endl;
    file << "DIMENSION: " << number_of_cities() << std::endl;
    file << "MIN SPEED: " << speed_min_ << std::endl;
    file << "MAX SPEED: " << speed_max_ << std::endl;
    file << "RENTING RATIO: " << renting_ratio_ << std::endl;
    file << "NUMBER OF ITEMS: " << number_of_items() << std::endl;
    file << "CAPACITY OF KNAPSACK: " << capacity() << std::endl;
    file << "EDGE_WEIGHT_SECTION" << std::endl;
    for (CityId city_id = 0;
            city_id < number_of_cities();
            ++city_id) {
        file << city_id + 1 << " " << city(city_id).distance << std::endl;
    }
    file << "ITEMS SECTION (INDEX, PROFIT, WEIGHT, ASSIGNED NODE NUMBER): " << std::endl;
    for (ItemId item_id = 0;
            item_id < number_of_items();
            ++item_id) {
        const Item& item = this->item(item_id);
        file
            << item_id + 1 << " "
            << item.profit << " "
            << item.weight << " "
            << item.city_id + 1 << std::endl;
    }
    file << "EOF" << std::endl;
}

bool Instance::reduce_unprofitable_items()
{
    optimizationtools::IndexedSet unprofitable_items(number_of_items());

    for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
        const Item& item = this->item(item_id);
        const City& city = this->city(item.city_id);
        double speed_after_without = speed_max_ - (double)(city.weight_from_start * (speed_max_ - speed_min_)) / capacity();
        double speed_after_with = speed_max_ - (double)((city.weight_from_start + item.weight) * (speed_max_ - speed_min_)) / capacity();
        Profit cost_without = renting_ratio() * (double)city.distance_to_end / speed_after_without;
        Profit cost_with = renting_ratio() * (double)city.distance_to_end / speed_after_with;
        Profit min_cost = cost_with - cost_without;
        if (item.profit <= min_cost) {
            unprofitable_items.add(item_id);
        }
    }

    if (unprofitable_items.empty())
        return false;

    UnreductionInfo new_unreduction_info;
    new_unreduction_info.original_instance = unreduction_info_.original_instance;

    // Update mandatory_sets.
    new_unreduction_info.mandatory_items = unreduction_info_.mandatory_items;
    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = number_of_items() - unprofitable_items.size();
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(number_of_cities());
    new_unreduction_info.unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0; city_id < number_of_cities(); ++city_id) {
        new_instance_builder.set_distance(city_id, city(city_id).distance);
        new_instance_builder.add_weight(city_id, city(city_id).weight);
    }
    new_instance_builder.set_minimum_speed(speed_min_);
    new_instance_builder.set_maximum_speed(speed_max_);
    new_instance_builder.set_renting_ratio(renting_ratio_);
    new_instance_builder.set_capacity(capacity_);

    // Add items.
    ItemId new_item_id = 0;
    for (auto it = unprofitable_items.out_begin();
            it != unprofitable_items.out_end();
            ++it) {
        ItemId item_id = *it;
        const Item& item = this->item(item_id);
        if (new_item_id == -1)
            continue;
        new_unreduction_info.unreduction_operations[new_item_id]
            = unreduction_info_.unreduction_operations[item_id];
        new_instance_builder.add_item(item.city_id, item.weight, item.profit);
        new_item_id++;
    }

    *this = new_instance_builder.build();
    unreduction_info_ = new_unreduction_info;
    //print(std::cout, 1);
    return true;
}

bool Instance::reduce_compulsory_items()
{
    optimizationtools::IndexedSet compulsory_items(number_of_items());

    if (weight_sum_ <= capacity()) {
        for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
            const Item& item = this->item(item_id);
            const City& city = this->city(item.city_id);
            double speed_after_without = speed_max_ - (double)((weight_sum_ - item.weight) * (speed_max_ - speed_min_)) / capacity();
            double speed_after_with = speed_max_ - (double)(weight_sum_ * (speed_max_ - speed_min_)) / capacity();
            Profit cost_without = renting_ratio() * (double)city.distance_to_end / speed_after_without;
            Profit cost_with = renting_ratio() * (double)city.distance_to_end / speed_after_with;
            Profit max_cost = cost_with - cost_without;
            if (item.profit >= max_cost) {
                compulsory_items.add(item_id);
            }
        }
    }

    if (compulsory_items.empty())
        return false;

    UnreductionInfo new_unreduction_info;
    new_unreduction_info.original_instance = unreduction_info_.original_instance;

    // Update mandatory_sets.
    new_unreduction_info.mandatory_items = unreduction_info_.mandatory_items;
    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = number_of_items() - compulsory_items.size();
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(number_of_cities());
    new_unreduction_info.unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0; city_id < number_of_cities(); ++city_id) {
        new_instance_builder.set_distance(city_id, city(city_id).distance);
        new_instance_builder.add_weight(city_id, city(city_id).weight);
    }
    new_instance_builder.set_minimum_speed(speed_min_);
    new_instance_builder.set_maximum_speed(speed_max_);
    new_instance_builder.set_renting_ratio(renting_ratio_);
    new_instance_builder.set_capacity(capacity_);

    // Update mandatory_items.
    for (ItemId item_id: compulsory_items) {
        const Item& item = this->item(item_id);
        new_instance_builder.add_weight(item.city_id, item.weight);
        ItemId orig_item_id = unreduction_info_.unreduction_operations[item_id];
        new_unreduction_info.mandatory_items.push_back(orig_item_id);
    }
    // Add items.
    ItemId new_item_id = 0;
    for (auto it = compulsory_items.out_begin();
            it != compulsory_items.out_end();
            ++it) {
        ItemId item_id = *it;
        const Item& item = this->item(item_id);
        if (new_item_id == -1)
            continue;
        new_unreduction_info.unreduction_operations[new_item_id]
            = unreduction_info_.unreduction_operations[item_id];
        new_instance_builder.add_item(item.city_id, item.weight, item.profit);
        new_item_id++;
    }

    *this = new_instance_builder.build();
    unreduction_info_ = new_unreduction_info;
    //print(std::cout, 1);
    return true;
}

bool Instance::reduce_polyakovskiy2017(
        Counter maximum_number_of_rounds)
{
    std::vector<int> unprofitable_items(number_of_items(), 0);
    std::vector<int> compulsory_items(number_of_items(), 0);
    ItemId number_of_unprofitable_items = 0;
    ItemId number_of_compulsory_items = 0;

    std::vector<double> profit_prime(number_of_items(), 0);
    for (ItemId item_id = 0; item_id < number_of_items(); ++item_id)
        profit_prime[item_id] = this->item(item_id).profit;

    std::vector<Weight> w_max(number_of_cities(), 0);
    std::vector<Weight> w_comp(number_of_cities(), 0);

    double ups = (maximum_speed() - minimum_speed()) / capacity();

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
        for (CityId city_id = 1; city_id < number_of_cities(); ++city_id) {
            const City& city = this->city(city_id);

            w_max[city_id] = w_max[city_id - 1] + city.weight;
            w_sum += city.weight;

            // Loop through the items of the city.
            for (ItemId item_id: this->city(city_id).item_ids) {
                const Item& item = this->item(item_id);

                // Don't consider unprofitable items.
                if (unprofitable_items[item_id] == 1)
                    continue;

                // Update w_max.
                w_sum += item.weight;
                w_max[city_id] += item.weight;
                if (w_max[city_id] > capacity())
                    w_max[city_id] = capacity();
            }
        }

        if (w_sum <= capacity()) {

            // Loop through the cities in reverse order.
            for (CityId city_id_1 = number_of_cities() - 1;
                    city_id_1 >= 0;
                    --city_id_1) {
                const City& city_1 = this->city(city_id_1);

                // Loop through the items of the city.
                for (ItemId item_pos_1 = 0;
                        item_pos_1 < (ItemId)city_1.item_ids.size();
                        ++item_pos_1) {
                    ItemId item_id_1 = city_1.item_ids[item_pos_1];
                    const Item& item_1 = this->item(item_id_1);

                    if (unprofitable_items[item_id_1] == 1
                            || compulsory_items[item_id_1] == 1) {
                        continue;
                    }

                    double c_max = 0;
                    double c_min = 0;
                    bool flag_prime = false;

                    for (CityId city_id_2 = city_id_1;
                            city_id_2 < number_of_cities();
                            ++city_id_2) {
                        const City& city_2 = this->city(city_id_2);

                        ItemId pos_max = (city_id_2 != city_id_1)?
                            city_2.item_ids.size():
                            item_pos_1;
                        for (ItemId item_pos_2 = 0;
                                item_pos_2 < pos_max;
                                ++item_pos_2) {
                            ItemId item_id_2 = city_2.item_ids[item_pos_2];
                            const Item& item_2 = this->item(item_id_2);

                            // Only consider non unprofitable items.
                            if (unprofitable_items[item_id_2] == 1)
                                continue;

                            // If true, item is not compulsory.
                            if (compulsory_items[item_id_2] == 0
                                    && item_1.weight >= item_2.weight
                                    && item_1.profit - c_min <= profit_prime[item_id_2]) {
                                profit_prime[item_id_1] = profit_prime[item_id_2] + c_min;
                                flag_prime = true;
                                break;
                            }

                            // If true, item is compulsory.
                            if (compulsory_items[item_id_2] == 1
                                    && item_1.weight <= item_2.weight
                                    && item_1.profit - c_max > profit_prime[item_id_2]) {
                                compulsory_items[item_id_1] = 1;
                                number_of_compulsory_items++;
                                profit_prime[item_id_1] = profit_prime[item_id_2] + c_max;
                                found = true;
                                flag_prime = true;
                                break;
                            }
                        }

                        if (flag_prime)
                            break;

                        // Distance between city j and city j + 1
                        double distance_to_next = (city_id_2 == this->number_of_cities() - 1)?
                            this->city(0).distance:
                            this->city(city_id_2 + 1).distance;

                        // Update least/most increamental costs
                        c_min += renting_ratio() * (double)distance_to_next
                            / (maximum_speed() - ups * (w_comp[city_id_2] + item_1.weight))
                            - renting_ratio() * (double)distance_to_next
                            / (maximum_speed() - ups * w_comp[city_id_2]);
                        c_max += renting_ratio() * (double)distance_to_next
                            / (maximum_speed() - ups * w_max[city_id_2])
                            - renting_ratio() * (double)distance_to_next
                            / (maximum_speed() - ups * (w_max[city_id_2] - item_1.weight));
                    }

                    if (flag_prime) {
                        continue;
                    }

                    // If the item is compulsory indenpendently of any other item.
                    profit_prime[item_id_1] = c_max;
                    if (c_max < item_1.profit) {
                        compulsory_items[item_id_1] = 1;
                        number_of_compulsory_items++;
                        found = true;
                    }
                }
            }
        }

        // Compute unprofitable items

        // Compute weight of compulsory items in all current and preceding cities
        w_comp[0] = 0;
        for (CityId city_id = 1; city_id < number_of_cities(); ++city_id) {
            const City& city = this->city(city_id);

            w_comp[city_id] = w_comp[city_id - 1] + city.weight;

            for (ItemId item_id: city.item_ids) {
                if (compulsory_items[item_id] == 0)
                    continue;
                w_comp[city_id] += this->item(item_id).weight;
            }
        }

        // Loop through the cities in reverse order.
        for (CityId city_id_1 = number_of_cities() - 1;
                city_id_1 > 0;
                --city_id_1) {
            const City& city_1 = this->city(city_id_1);

            // Loop through the items of the city.
            for (ItemId item_pos_1 = 0;
                    item_pos_1 < (ItemId)city_1.item_ids.size();
                    ++item_pos_1) {
                ItemId item_id_1 = city_1.item_ids[item_pos_1];
                const Item& item_1 = this->item(item_id_1);

                if (unprofitable_items[item_id_1] == 1
                        || compulsory_items[item_id_1] == 1) {
                    continue;
                }

                double c_max = 0;
                double c_min = 0;
                bool flag_prime = false;

                for (CityId city_id_2 = city_id_1;
                        city_id_2 < number_of_cities();
                        ++city_id_2) {
                    const City& city_2 = this->city(city_id_2);

                    ItemId pos_max = (city_id_2 != city_id_1)?
                        city_2.item_ids.size():
                        item_pos_1;
                    for (ItemId item_pos_2 = 0;
                           item_pos_2 < pos_max;
                           ++item_pos_2) {
                        ItemId item_id_2 = city_2.item_ids[item_pos_2];
                        const Item& item_2 = this->item(item_id_2);

                        // Only consider non compulsory items
                        if (compulsory_items[item_id_2] == 1)
                            continue;

                        // If true, item is not unprofitable
                        if (unprofitable_items[item_id_2] == 0
                                && item_1.weight <= item_2.weight
                                && item_1.profit - c_max > profit_prime[item_id_2]) {
                            profit_prime[item_id_1] = profit_prime[item_id_2] + c_max;
                            flag_prime = true;
                            break;
                        }

                        // If ture, item is unprofitable
                        if (unprofitable_items[item_id_2] == 1
                                && item_1.weight >= item_2.weight
                                && item_1.profit - c_min <= profit_prime[item_id_2]) {
                            unprofitable_items[item_id_1] = 1;
                            number_of_unprofitable_items++;
                            profit_prime[item_id_1] = profit_prime[item_id_2] + c_min;
                            found = true;
                            flag_prime = true;
                            break;
                        }
                    }

                    if (flag_prime)
                        break;

                    // Distance between city j and city j + 1
                    double distance_to_next = (city_id_2 == this->number_of_cities() - 1)?
                        this->city(0).distance:
                        this->city(city_id_2 + 1).distance;

                    c_min += renting_ratio() * (double)distance_to_next
                        / (maximum_speed() - ups * (w_comp[city_id_2] + item_1.weight))
                        - renting_ratio() * (double)distance_to_next
                        / (maximum_speed() - ups * w_comp[city_id_2]);

                    // If item is unprofitable by itself.
                    if (c_min >= item_1.profit) {
                        unprofitable_items[item_id_1] = 1;
                        number_of_unprofitable_items++;
                        profit_prime[item_id_1] = c_min;
                        found = true;
                        flag_prime = true;
                        break;
                    }

                    c_max += renting_ratio() * (double)distance_to_next
                        / (maximum_speed() - ups * w_max[city_id_2])
                        - renting_ratio() * (double)distance_to_next
                        / (maximum_speed() - ups * (w_max[city_id_2] - item_1.weight));
                }

                if (flag_prime)
                    continue;;
                profit_prime[item_id_1] = c_min;
            }
        }

        if (!found)
            break;
    }

    if (number_of_compulsory_items == 0
            && number_of_unprofitable_items == 0) {
        return false;
    }

    UnreductionInfo new_unreduction_info;
    new_unreduction_info.original_instance = unreduction_info_.original_instance;

    // Update mandatory_sets.
    new_unreduction_info.mandatory_items = unreduction_info_.mandatory_items;
    // Create new instance and compute unreduction_operations.
    ItemId new_number_of_items = number_of_items()
        - number_of_compulsory_items
        - number_of_unprofitable_items;
    InstanceBuilder new_instance_builder;
    new_instance_builder.add_cities(number_of_cities());
    new_unreduction_info.unreduction_operations = std::vector<ItemId>(new_number_of_items);
    for (CityId city_id = 0; city_id < number_of_cities(); ++city_id) {
        new_instance_builder.set_distance(city_id, city(city_id).distance);
        new_instance_builder.add_weight(city_id, city(city_id).weight);
    }
    new_instance_builder.set_minimum_speed(speed_min_);
    new_instance_builder.set_maximum_speed(speed_max_);
    new_instance_builder.set_renting_ratio(renting_ratio_);
    new_instance_builder.set_capacity(capacity_);

    // Update mandatory_items.
    for (ItemId item_id = 0;
            item_id < (ItemId)compulsory_items.size();
            ++item_id) {
        if (compulsory_items[item_id] == 1) {
            const Item& item = this->item(item_id);
            new_instance_builder.add_weight(item.city_id, item.weight);
            ItemId orig_item_id = unreduction_info_.unreduction_operations[item_id];
            new_unreduction_info.mandatory_items.push_back(orig_item_id);
        }
    }

    // Add items.
    ItemId new_item_id = 0;
    for (ItemId item_id = 0; item_id < this->number_of_items(); ++item_id) {
        if (compulsory_items[item_id] == 0 && unprofitable_items[item_id] == 0){
            const Item& item = this->item(item_id);
            if (new_item_id == -1)
                continue;
            new_unreduction_info.unreduction_operations[new_item_id]
                = unreduction_info_.unreduction_operations[item_id];
            new_instance_builder.add_item(item.city_id, item.weight, item.profit);
            new_item_id++;
        }
    }

    *this = new_instance_builder.build();
    unreduction_info_ = new_unreduction_info;
    return true;
}

Instance Instance::reduce(ReductionParameters parameters) const
{
    // Initialize reduced instance.
    Instance instance_new = *this;
    instance_new.unreduction_info_ = UnreductionInfo();
    instance_new.unreduction_info_.original_instance = this;
    instance_new.unreduction_info_.unreduction_operations = std::vector<ItemId>(number_of_items());
    for (ItemId item_id = 0;
            item_id < number_of_items();
            ++item_id) {
        instance_new.unreduction_info_.unreduction_operations[item_id] = item_id;
    }

    if (!parameters.enable_expensive_reduction) {
        for (Counter round_number = 0;
                round_number < parameters.maximum_number_of_rounds;
                ++round_number) {
            bool found = false;
            found |= instance_new.reduce_unprofitable_items();
            found |= instance_new.reduce_compulsory_items();
            if (!found)
                break;
        }
    } else {
        instance_new.reduce_polyakovskiy2017(
                parameters.maximum_number_of_rounds);
    }

    instance_new.unreduction_info_.extra_profit = 0;
    for (ItemId orig_item_id: instance_new.unreduction_info_.mandatory_items) {
        instance_new.unreduction_info_.extra_profit += item(orig_item_id).profit;
    }

    return instance_new;
}

void packingwhiletravellingsolver::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
        << "======================================" << std::endl
        << "       Packing while travelling       " << std::endl
        << "======================================" << std::endl
        << std::endl
        << "Instance" << std::endl
        << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
