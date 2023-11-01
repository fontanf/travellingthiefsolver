#include "travellingthiefsolver/thieforienteering/instance.hpp"

#include "optimizationtools/utils/utils.hpp"

using namespace travellingthiefsolver::thieforienteering;

Instance::Instance(CityId number_of_cities):
    cities_(number_of_cities)
{
    distances_ = std::vector<std::vector<Distance>>(number_of_cities);
    for (CityId city_id = 0;
            city_id < number_of_cities;
            ++city_id) {
        distances_[city_id] = std::vector<Distance>(
                city_id,
                std::numeric_limits<Distance>::max());
    }
}

void Instance::set_xy(
        CityId city_id,
        double x,
        double y,
        double z)
{
    cities_[city_id].x = x;
    cities_[city_id].y = y;
    cities_[city_id].z = z;
}

void Instance::add_item(
        CityId city_id,
        Weight weight,
        Profit profit)
{
    ItemId item_id = items_.size();
    cities_[city_id].item_ids.push_back(item_id);

    Item item;
    item.city_id = city_id;
    item.profit = profit;
    item.weight = weight;
    items_.push_back(item);

    profit_sum_ += profit;
    weight_sum_ += weight;
}

Instance::Instance(
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

void Instance::read_polyakovskiy2014(std::ifstream& file)
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
            CityId n = std::stol(line.back());
            cities_ = std::vector<City>(n);
            distances_ = std::vector<std::vector<Distance>>(n);
            for (CityId city_id = 0;
                    city_id < n;
                    ++city_id) {
                distances_[city_id] = std::vector<Distance>(
                        city_id,
                        std::numeric_limits<Distance>::max());
            }
        } else if (tmp.rfind("NUMBER OF ITEMS", 0) == 0) {
            number_of_items = std::stol(line.back());
        } else if (tmp.rfind("CAPACITY OF KNAPSACK", 0) == 0) {
            set_capacity(std::stol(line.back()));
        } else if (tmp.rfind("MAX TIME", 0) == 0) {
            set_time_limit(std::stol(line.back()));
        } else if (tmp.rfind("MIN SPEED", 0) == 0) {
            set_minimum_speed(std::stod(line.back()));
        } else if (tmp.rfind("MAX SPEED", 0) == 0) {
            set_maximum_speed(std::stod(line.back()));
        } else if (tmp.rfind("EDGE_WEIGHT_TYPE", 0) == 0) {
            edge_weight_type_ = line.back();
        } else if (tmp.rfind("EDGE_WEIGHT_FORMAT", 0) == 0) {
            edge_weight_format = line.back();
        } else if (tmp.rfind("NODE_COORD_TYPE", 0) == 0) {
            node_coord_type_ = line.back();
        } else if (tmp.rfind("EDGE_WEIGHT_SECTION", 0) == 0) {
            if (edge_weight_format == "UPPER_ROW") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < number_of_cities() - 1;
                        ++city_id_1) {
                    for (CityId city_id_2 = city_id_1 + 1;
                            city_id_2 < number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "LOWER_ROW") {
                Distance d;
                for (CityId city_id_1 = 1;
                        city_id_1 < number_of_cities();
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
                        city_id_1 < number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = city_id_1;
                            city_id_2 < number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else if (edge_weight_format == "LOWER_DIAG_ROW") {
                Distance d;
                for (CityId city_id_1 = 0;
                        city_id_1 < number_of_cities();
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
                        city_id_1 < number_of_cities();
                        ++city_id_1) {
                    for (CityId city_id_2 = 0;
                            city_id_2 < number_of_cities();
                            ++city_id_2) {
                        file >> d;
                        set_distance(city_id_1, city_id_2, d);
                    }
                }
            } else {
                std::cerr << "\033[31m" << "ERROR, EDGE_WEIGHT_FORMAT \"" << edge_weight_format << "\" not implemented." << "\033[0m" << std::endl;
            }
        } else if (tmp.rfind("NODE_COORD_SECTION", 0) == 0) {
            if (node_coord_type_ == "TWOD_COORDS") {
                CityId tmp;
                double x, y;
                for (CityId city_id = 0;
                        city_id < number_of_cities();
                        ++city_id) {
                    file >> tmp >> x >> y;
                    set_xy(city_id, x, y);
                }
            } else if (node_coord_type_ == "THREED_COORDS") {
                CityId tmp;
                double x, y, z;
                for (CityId city_id = 0;
                        city_id < number_of_cities();
                        ++city_id) {
                    file >> tmp >> x >> y >> z;
                    set_xy(city_id, x, y, z);
                }
            }
        } else if (tmp.rfind("DISPLAY_DATA_SECTION", 0) == 0) {
            CityId tmp;
            double x, y;
            for (CityId city_id = 0;
                    city_id < number_of_cities();
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
            std::cerr << "\033[31m" << "ERROR, ENTRY \"" << line[0] << "\" not implemented." << "\033[0m" << std::endl;
        }
    }

    // Compute distances.
    if (edge_weight_type_ == "EUC_2D") {
        for (CityId city_id_1 = 0;
                city_id_1 < number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < number_of_cities();
                    ++city_id_2) {
                Distance xd = x(city_id_2) - x(city_id_1);
                Distance yd = y(city_id_2) - y(city_id_1);
                Distance d = std::round(std::sqrt(xd * xd + yd * yd));
                set_distance(city_id_1, city_id_2, d);
            }
        }
    } else if (edge_weight_type_ == "CEIL_2D") {
        for (CityId city_id_1 = 0;
                city_id_1 < number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < number_of_cities();
                    ++city_id_2) {
                Distance xd = x(city_id_2) - x(city_id_1);
                Distance yd = y(city_id_2) - y(city_id_1);
                Distance d = std::ceil(std::sqrt(xd * xd + yd * yd));
                set_distance(city_id_1, city_id_2, d);
            }
        }
    } else if (edge_weight_type_ == "GEO") {
        std::vector<double> latitudes(number_of_cities(), 0);
        std::vector<double> longitudes(number_of_cities(), 0);
        for (CityId city_id = 0;
                city_id < number_of_cities();
                ++city_id) {
            double pi = 3.141592;
            int deg_x = std::round(x(city_id));
            double min_x = x(city_id) - deg_x;
            latitudes[city_id] = pi * (deg_x + 5.0 * min_x / 3.0) / 180.0;
            int deg_y = std::round(y(city_id));
            double min_y = y(city_id) - deg_y;
            longitudes[city_id] = pi * (deg_y + 5.0 * min_y / 3.0) / 180.0;
        }
        double rrr = 6378.388;
        for (CityId city_id_1 = 0;
                city_id_1 < number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < number_of_cities();
                    ++city_id_2) {
                double q1 = cos(longitudes[city_id_1] - longitudes[city_id_2]);
                double q2 = cos(latitudes[city_id_1] - latitudes[city_id_2]);
                double q3 = cos(latitudes[city_id_1] + latitudes[city_id_2]);
                Distance d = (Distance)(rrr * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
                set_distance(city_id_1, city_id_2, d);
            }
        }
    } else if (edge_weight_type_ == "ATT") {
        for (CityId city_id_1 = 0;
                city_id_1 < number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < number_of_cities();
                    ++city_id_2) {
                double xd = x(city_id_1) - x(city_id_2);
                double yd = y(city_id_1) - y(city_id_2);
                double rij = sqrt((xd * xd + yd * yd) / 10.0);
                int tij = std::round(rij);
                Distance d = (tij < rij)? tij + 1: tij;
                set_distance(city_id_1, city_id_2, d);
            }
        }
    } else if (edge_weight_type_ == "EXPLICIT") {
    } else {
        std::cerr << "\033[31m" << "ERROR, EDGE_WEIGHT_TYPE \"" << edge_weight_type_ << "\" not implemented." << "\033[0m" << std::endl;
    }
}

std::ostream& Instance::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of cities:  " << number_of_cities() << std::endl
            << "Number of items:   " << number_of_items() << std::endl
            << "Capacity:          " << capacity() << std::endl
            << "Minimum speed:     " << minimum_speed() << std::endl
            << "Maximum speed:     " << maximum_speed() << std::endl
            << "Time limit:        " << time_limit() << std::endl
            << "Profit sum:        " << profit_sum_ << std::endl
            << "Weight sum:        " << weight_sum_ << std::endl
            << "Weight ratio:      " << (double)weight_sum_ / capacity() << std::endl
            ;
    }

    if (verbose >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::setw(12) << "X"
            << std::setw(12) << "Y"
            << std::setw(12) << "Z"
            << std::setw(12) << "# items"
            << std::endl
            << std::setw(12) << "------"
            << std::setw(12) << "-"
            << std::setw(12) << "-"
            << std::setw(12) << "-"
            << std::setw(12) << "-------"
            << std::endl;
        for (CityId city_id = 0;
                city_id < number_of_cities();
                ++city_id) {
            const City& city = this->city(city_id);
            os
                << std::setw(12) << city_id
                << std::setw(12) << city.x
                << std::setw(12) << city.y
                << std::setw(12) << city.z
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

    if (verbose >= 3) {
        os << std::endl
            << std::setw(12) << "City 1"
            << std::setw(12) << "City 2"
            << std::setw(12) << "Distance"
            << std::endl
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "--------"
            << std::endl;
        for (CityId city_id_1 = 0;
                city_id_1 < number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < number_of_cities();
                    ++city_id_2) {
                os
                    << std::setw(12) << city_id_1
                    << std::setw(12) << city_id_2
                    << std::setw(12) << distance(city_id_1, city_id_2)
                    << std::endl;
            }
        }
    }

    return os;
}

void travellingthiefsolver::thieforienteering::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
        << "=====================================" << std::endl
        << "        TravellingThiefSolver        " << std::endl
        << "=====================================" << std::endl
        << std::endl
        << "Problem" << std::endl
        << "-------" << std::endl
        << "Thief orienteering problem" << std::endl
        << std::endl
        << "Instance" << std::endl
        << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
