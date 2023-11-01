#include "travellingthiefsolver/travellingthief/instance_builder.hpp"

using namespace travellingthiefsolver::travellingthief;

void InstanceBuilder::add_cities(CityId number_of_cities)
{
    instance_.cities_.insert(
            instance_.cities_.end(),
            number_of_cities,
            City());
}

void InstanceBuilder::set_xy(
        CityId city_id,
        double x,
        double y,
        double z)
{
    instance_.cities_[city_id].x = x;
    instance_.cities_[city_id].y = y;
    instance_.cities_[city_id].z = z;
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

    instance_.profit_sum_ += profit;
    instance_.weight_sum_ += weight;
}

InstanceBuilder::InstanceBuilder(
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
    std::string edge_weight_format;
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
            CityId number_of_cities = std::stol(line.back());
            add_cities(number_of_cities);
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
        } else if (tmp.rfind("EDGE_WEIGHT_TYPE", 0) == 0) {
            instance_.edge_weight_type_ = line.back();
        } else if (tmp.rfind("EDGE_WEIGHT_FORMAT", 0) == 0) {
            edge_weight_format = line.back();
        } else if (tmp.rfind("NODE_COORD_TYPE", 0) == 0) {
            instance_.node_coord_type_ = line.back();
        } else if (tmp.rfind("EDGE_WEIGHT_SECTION", 0) == 0) {
            instance_.init_distances();
            if (edge_weight_format == "UPPER_ROW") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < instance_.number_of_cities() - 1;
                        ++city_id_1) {
                    for (CityId city_id_2 = city_id_1 + 1;
                            city_id_2 < instance_.number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "LOWER_ROW") {
                Distance d;
                for (CityId city_id_1 = 1;
                        city_id_1 < instance_.number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = 0;
                            city_id_2 < city_id_1;
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "UPPER_DIAG_ROW") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < instance_.number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = city_id_1;
                            city_id_2 < instance_.number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "LOWER_DIAG_ROW") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < instance_.number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = 0;
                            city_id_2 <= city_id_1;
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "FULL_MATRIX") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < instance_.number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = 0;
                            city_id_2 < instance_.number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else {
                throw std::invalid_argument(
                        "EDGE_WEIGHT_FORMAT \""
                        + edge_weight_format
                        + "\" not implemented.");
            }
        } else if (tmp.rfind("NODE_COORD_SECTION", 0) == 0) {
            if (instance_.node_coord_type_ == "TWOD_COORDS") {
                CityId tmp;
                double x, y;
                for (CityId city_id = 0;
                        city_id < instance_.number_of_cities();
                        ++city_id) {
                    file >> tmp >> x >> y;
                    set_xy(city_id, x, y);
                }
            } else if (instance_.node_coord_type_ == "THREED_COORDS") {
                CityId tmp;
                double x, y, z;
                for (CityId city_id = 0;
                        city_id < instance_.number_of_cities();
                        ++city_id) {
                    file >> tmp >> x >> y >> z;
                    set_xy(city_id, x, y, z);
                }
            }
        } else if (tmp.rfind("DISPLAY_DATA_SECTION", 0) == 0) {
            CityId tmp;
            double x, y;
            for (CityId city_id = 0;
                    city_id < instance_.number_of_cities();
                    ++city_id) {
                file >> tmp >> x >> y;
                set_xy(city_id, x, y);
            }
        } else if (tmp.rfind("ITEMS SECTION", 0) == 0) {
            ItemId tmp = -1;
            Profit profit = -1;
            Weight weight = -1;
            CityId city = -1;
            for (ItemId item_id = 0; item_id < number_of_items; ++item_id) {
                file >> tmp >> profit >> weight >> city;
                add_item(city - 1, weight, profit);
            }
        } else if (tmp.rfind("EOF", 0) == 0) {
            break;
        } else {
            throw std::invalid_argument(
                    "ENTRY \""
                    + line[0]
                    + "\" not implemented.");
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Build /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Instance InstanceBuilder::build()
{
    return std::move(instance_);
}
