#include "packingwhiletravellingsolver/instance_builder.hpp"

using namespace packingwhiletravellingsolver;

void InstanceBuilder::add_cities(CityId number_of_cities)
{
    instance_.cities_.insert(instance_.cities_.end(), number_of_cities, City());
}

void InstanceBuilder::set_distance(
        CityId city_id,
        Distance distance)
{
    instance_.cities_[city_id].distance = distance;
}

void InstanceBuilder::add_item(
        CityId city_id,
        Weight weight,
        Profit profit)
{
    ItemId item_id = instance_.items_.size();
    instance_.cities_[city_id].item_ids.push_back(item_id);

    Item item;
    item.city_id = city_id;
    item.profit = profit;
    item.weight = weight;
    instance_.items_.push_back(item);
}

void InstanceBuilder::read(
        std::string instance_path,
        std::string format)
{
    std::ifstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }
    if (format == ""
            || format == "default"
            || format == "polyakovskiy2014") {
        read_polyakovskiy2014(file);
    } else {
        throw std::invalid_argument(
                "Unknown instance format \"" + format + "\".");
    }
    file.close();
}

void InstanceBuilder::read_polyakovskiy2014(std::ifstream& file)
{
    std::string tmp;
    std::vector<std::string> line;
    ItemId number_of_items = -1;
    std::string edge_weight_type;
    std::string edge_weight_format;
    std::string node_coord_type = "TWOD_COORDS";
    while (getline(file, tmp)) {
        line = optimizationtools::split(tmp);
        if (line.size() == 0) {
        } else if (tmp.rfind("NAME", 0) == 0) {
        } else if (tmp.rfind("PROBLEM NAME", 0) == 0) {
        } else if (tmp.rfind("COMMENT", 0) == 0) {
        } else if (tmp.rfind("TYPE", 0) == 0) {
        } else if (tmp.rfind("KNAPSACK DATA TYPE", 0) == 0) {
        } else if (tmp.rfind("DISPLAY_DATA_TYPE", 0) == 0) {
        } else if (tmp.rfind("DIMENSION", 0) == 0) {
            CityId n = std::stol(line.back());
            instance_.cities_ = std::vector<City>(n);
        } else if (tmp.rfind("NUMBER OF ITEMS", 0) == 0) {
            number_of_items = std::stol(line.back());
        } else if (tmp.rfind("CAPACITY OF KNAPSACK", 0) == 0) {
            set_capacity(std::stol(line.back()));
        } else if (tmp.rfind("MIN SPEED", 0) == 0) {
            set_minimum_speed(std::stod(line.back()));
        } else if (tmp.rfind("MAX SPEED", 0) == 0) {
            set_maximum_speed(std::stod(line.back()));
        } else if (tmp.rfind("RENTING RATIO", 0) == 0) {
            set_renting_ratio(std::stod(line.back()));
        } else if (tmp.rfind("EDGE_WEIGHT_SECTION", 0) == 0) {
            CityId tmp;
            Distance distance;
            for (CityId city_id = 0;
                    city_id < instance_.number_of_cities();
                    ++city_id) {
                file >> tmp >> distance;
                set_distance(city_id, distance);
            }
        } else if (tmp.rfind("ITEMS SECTION", 0) == 0) {
            ItemId tmp = -1;
            Profit profit = -1;
            Weight weight = -1;
            CityId city_id = -1;
            for (ItemId item_id = 0; item_id < number_of_items; ++item_id) {
                file >> tmp >> profit >> weight >> city_id;
                add_item(city_id - 1, weight, profit);
            }
        } else if (tmp.rfind("EOF", 0) == 0) {
            break;
        } else {
            std::cerr << "\033[31m" << "ERROR, ENTRY \"" << line[0] << "\" not implemented." << "\033[0m" << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Build /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void InstanceBuilder::compute_distances_start_end()
{
    // Total distance.
    instance_.distance_ = 0;
    for (CityId city_id = 0;
            city_id < instance_.number_of_cities();
            ++city_id) {
        instance_.distance_ += instance_.city(city_id).distance;
    }
    instance_.distance_ += instance_.city(0).distance;

    // Distance from start.
    instance_.cities_[0].distance_from_start = 1;
    for (CityId city_id = 1;
            city_id < instance_.number_of_cities();
            ++city_id) {
        instance_.cities_[city_id].distance_from_start
            = instance_.city(city_id - 1).distance_from_start
            + instance_.city(city_id).distance;
    }

    // Distance to end.
    instance_.cities_[instance_.number_of_cities() - 1].distance_to_end
        = instance_.city(0).distance;
    for (CityId city_id = instance_.number_of_cities() - 2;
            city_id >= 0;
            --city_id) {
        instance_.cities_[city_id].distance_to_end
            = instance_.city(city_id + 1).distance_to_end
            + instance_.city(city_id + 1).distance;
    }
}

void InstanceBuilder::compute_weight_from_start()
{
    instance_.cities_[0].weight_from_start = instance_.city(0).weight;
    for (CityId city_id = 1;
            city_id < instance_.number_of_cities();
            ++city_id) {
        instance_.cities_[city_id].weight_from_start
            = instance_.city(city_id - 1).weight_from_start
            + instance_.city(city_id).weight;
    }

    // Compute city_weight_.
    instance_.city_weight_ = 0;
    for (CityId city_id = 1;
            city_id < instance_.number_of_cities();
            ++city_id) {
        instance_.city_weight_ += instance_.city(city_id).weight;
    }
}

void InstanceBuilder::compute_weight_sum()
{
    instance_.weight_sum_ = 0;
    for (CityId city_id = 0;
            city_id < instance_.number_of_cities();
            ++city_id) {
        const City& city = instance_.city(city_id);
        instance_.weight_sum_ += city.weight;
        for (ItemId item_id: city.item_ids)
            instance_.weight_sum_ += instance_.item(item_id).weight;
    }
}

Instance InstanceBuilder::build()
{
    compute_distances_start_end();
    compute_weight_sum();
    compute_weight_from_start();
    return std::move(instance_);
}
