#include "travellingthiefsolver/thieforienteering/solution.hpp"

using namespace travellingthiefsolver::thieforienteering;

Solution::Solution(
        const Instance& instance):
    instance_(&instance),
    cities_(instance.number_of_cities()),
    items_is_selected_(instance.number_of_items(), false)
{
    // Add initial city.
    city_ids_.push_back(0);
    cities_[0].in = true;
    cities_[instance.number_of_cities() - 1].in = true;
    distance_ = distance_cur_ + instance.distances().distance(
            0,
            instance_->number_of_cities() - 1);
    travel_time_ = travel_time_cur_ + instance.duration(
            0,
            instance.number_of_cities() - 1,
            item_weight_);
}

Solution::Solution(
        const Instance& instance,
        std::string certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    char sep;
    file >> sep;

    std::vector<CityId> city_ids;
    CityId city_id = -1;
    for (CityId city_pos = 0;
            city_pos < instance.number_of_cities();
            ++city_pos) {
        file >> city_id >> sep;
        city_id--;
        city_ids.push_back(city_id);
    }

    ItemId item_id;
    CityId city_pos = 0;
    file >> sep;
    for (;;) {
        file >> item_id >> sep;
        if (file.eof())
            break;
        item_id--;
        const Item& item = instance.item(item_id);
        while (item.city_id != city_ids[city_pos]) {
            city_pos++;
            add_city(city_ids[city_pos]);
        }
        add_item(item_id);
    }
}

bool Solution::feasible() const
{
    return (item_weight_ <= instance().capacity())
        && (travel_time() <= instance().time_limit());
}

void Solution::add_city(CityId city_id)
{
    // Check that the city has not already been visited.
    if (cities_[city_id].in) {
        throw std::runtime_error(
                "thieforienteering::Solution::add_city(CityId)."
                "City " + std::to_string(city_id) + " is already in the solution.");
    }

    CityId city_id_prev = city_ids_.back();
    city_ids_.push_back(city_id);
    cities_[city_id].in = true;
    distance_cur_ += instance().distances().distance(city_id_prev, city_id);
    distance_ = distance_cur_ + instance().distances().distance(
            city_id,
            instance_->number_of_cities() - 1);
    travel_time_cur_ += instance().duration(city_id_prev, city_id, item_weight_);
    travel_time_ = travel_time_cur_ + instance().duration(
            city_id,
            instance().number_of_cities() - 1,
            item_weight_);
}

void Solution::add_item(ItemId item_id)
{
    const Item& item = instance().item(item_id);

    // Check that the item is available at the last visited city.
    if (item.city_id != city_ids_.back()) {
        throw std::runtime_error("");  // TODO
    }

    // Check that the item has not already been selected.
    if (items_is_selected_[item_id]) {
        throw std::runtime_error("");  // TODO
    }

    items_is_selected_[item_id] = 1;
    item_ids_.push_back(item_id);
    item_profit_ += item.profit;
    item_weight_ += item.weight;
    CityId city_id = city_ids_.back();
    travel_time_ = travel_time_cur_ + instance().duration(
            city_id,
            instance().number_of_cities() - 1,
            item_weight_);

    cities_[city_id].number_of_items++;
    cities_[city_id].profit += item.profit;
    cities_[city_id].weight += item.weight;
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
            << "Travel time:       " << optimizationtools::Ratio<Time>(travel_time(), instance().time_limit()) << std::endl
            << "Number of items:   " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Item weight:       " << optimizationtools::Ratio<Weight>(item_weight(), instance().capacity()) << std::endl
            << "Item profit:       " << item_profit() << std::endl
            << "Feasible:          " << feasible() << std::endl
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
    if (output_path.empty())
        return;
    std::ofstream file(output_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + output_path + "\".");
    }

    file << "City,Distance,Weight,Speed" << std::endl;
    Distance distance = 0;
    Weight weight = 0;
    CityId city_id_prev = -1;
    for (CityPos city_pos = 0;
            city_pos < number_of_cities() - 1;
            ++city_pos) {
        CityId city_id = this->city_id(city_pos);
        const City& city = instance().city(city_id);
        if (city_id_prev != -1)
            distance += instance().distances().distance(city_id_prev, city_id);
        for (ItemId item_id: city.item_ids)
            if (contains(item_id))
                weight += instance().item(item_id).weight;
        double speed = instance().maximum_speed()
            - (double)(weight * (instance().maximum_speed() - instance().minimum_speed())) / instance().capacity();
        file
            << city_id << ","
            << distance << ","
            << weight << ","
            << speed << std::endl;
        city_id_prev = city_id;
    }
}
