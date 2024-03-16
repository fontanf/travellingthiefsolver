#include "travellingthiefsolver/travellingwhilepacking/solution.hpp"

using namespace travellingthiefsolver::travellingwhilepacking;

Solution::Solution(
        const Instance& instance):
    instance_(&instance),
    cities_is_visited_(instance.number_of_cities(), false)
{
    // Add initial city.
    city_ids_.push_back(0);
    cities_is_visited_[0] = true;
    item_weight_ += instance.city(0).weight;
}

Solution::Solution(
        const Instance& instance,
        const std::string& certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    // TODO
}

bool Solution::feasible() const
{
    return (number_of_cities() == instance().number_of_cities());
}

nlohmann::json Solution::to_json() const
{
    return nlohmann::json {
        {"NumberOfCities", number_of_cities()},
        {"Distance", distance()},
        {"TravelTime", travel_time()},
        {"RentingCost", renting_cost()},
    };
}

std::ostream& Solution::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of cities:    " << optimizationtools::Ratio<CityId>(number_of_cities(), instance().number_of_cities()) << std::endl
            << "Distance:            " << distance() << std::endl
            << "Travel time:         " << travel_time() << std::endl
            << "Feasible:            " << feasible() << std::endl
            << "Renting cost:        " << renting_cost() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::endl
            << std::setw(12) << "----"
            << std::endl;
        for (CityId city_id: city_ids_) {
            os
                << std::setw(12) << city_id
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
}
