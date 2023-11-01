#include "travellingthiefsolver/travellingthief/instance.hpp"

#include "optimizationtools/utils/utils.hpp"

using namespace travellingthiefsolver::travellingthief;

void Instance::compute_distances() const
{
    if (!distances_.empty())
        return;
    init_distances();

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
    } else {
        throw std::invalid_argument(
                "EDGE_WEIGHT_TYPE \""
                + edge_weight_type_
                + "\" not implemented.");
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
            << "Renting ratio:     " << renting_ratio() << std::endl
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

void travellingthiefsolver::travellingthief::init_display(
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
        << "Travelling thief problem" << std::endl
        << std::endl
        << "Instance" << std::endl
        << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
