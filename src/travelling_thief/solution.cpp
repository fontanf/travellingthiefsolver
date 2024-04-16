#include "travellingthiefsolver/travelling_thief/solution.hpp"

using namespace travellingthiefsolver::travelling_thief;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    cities_(instance.number_of_cities()),
    items_is_selected_(instance.number_of_items(), false)
{
    // Add initial city.
    city_ids_.push_back(0);
    cities_[0].position = 0;
}

bool Solution::feasible() const
{
    return (item_weight_ <= instance().capacity())
        && (number_of_cities() == instance().number_of_cities());
}

nlohmann::json Solution::to_json() const
{
    return nlohmann::json {
        {"NumberOfCities", number_of_cities()},
        {"Distance", distance()},
        {"TravelTime", travel_time()},
        {"NumberOfItems", number_of_items()},
        {"ItemWeight", item_weight()},
        {"ItemProfit", item_profit()},
    };
}

std::ostream& Solution::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of cities:  " << optimizationtools::Ratio<CityId>(number_of_cities(), instance().number_of_cities()) << std::endl
            << "Distance:          " << distance() << std::endl
            << "Travel time:       " << travel_time() << std::endl
            << "Renting cost:      " << renting_cost() << std::endl
            << "Number of items:   " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Item weight:       " << optimizationtools::Ratio<Weight>(item_weight(), instance().capacity()) << std::endl
            << "Item profit:       " << item_profit() << std::endl
            << "Feasible:          " << feasible() << std::endl
            << "Objective value:   " << objective_value() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::setw(12) << "# items"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::setw(12) << "Eff."
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "-------"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "----"
            << std::endl;
        for (CityId city_id: city_ids_) {
            const SolutionCity& city = cities_[city_id];
            os
                << std::setw(12) << city_id
                << std::setw(12) << city.number_of_items
                << std::setw(12) << city.weight
                << std::setw(12) << city.profit
                << std::setw(12) << city.profit / city.weight
                << std::endl;
        }
    }

    if (verbosity_level >= 3) {
        os << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "City"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id: item_ids_) {
            os
                << std::setw(12) << item_id
                << std::endl;
        }
    }

    return os;
}

void Solution::write(
        const std::string& certificate_path) const
{
    if (certificate_path.empty())
        return;
    std::ofstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    std::string separator = "[";
    for (CityId city_id: city_ids_) {
        file << separator << city_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
    separator = "[";
    for (ItemId item_id: item_ids_) {
        file << separator << item_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
}

void Solution::write_csv(
        const std::string& output_path) const
{
    (void)output_path;
    //if (output_path.empty())
    //    return;
    //std::ofstream file(output_path);
    //if (!file.good()) {
    //    throw std::runtime_error(
    //            "Unable to open file \"" + output_path + "\".");
    //}

    //file << "City,Distance,Weight,Speed" << std::endl;
    //Distance distance = 0;
    //Weight weight = 0;
    //CityId city_id_prev = -1;
    //for (CityPos city_pos = 0;
    //        city_pos < instance().number_of_cities();
    //        ++city_pos) {
    //    CityId city_id = this->city_id(city_pos);
    //    const City& city = instance().city(city_id);
    //    if (city_id_prev != -1)
    //        distance += instance().distances().distance(city_id_prev, city_id);
    //    for (ItemId item_id: city.item_ids)
    //        if (contains(item_id))
    //            weight += instance().item(item_id).weight;
    //    double speed = instance().maximum_speed()
    //        - (double)(weight * (instance().maximum_speed() - instance().minimum_speed())) / instance().capacity();
    //    file
    //        << city_id << ","
    //        << distance << ","
    //        << weight << ","
    //        << speed << std::endl;
    //    city_id_prev = city_id;
    //}
}
