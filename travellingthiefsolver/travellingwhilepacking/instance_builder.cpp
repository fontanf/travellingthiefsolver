#include "travellingthiefsolver/travellingwhilepacking/instance_builder.hpp"

#include "travelingsalesmansolver/distances/distances_builder.hpp"

#include "optimizationtools/utils/utils.hpp"

using namespace travellingthiefsolver::travellingwhilepacking;

void InstanceBuilder::add_cities(CityId number_of_cities)
{
    instance_.cities_.insert(
            instance_.cities_.end(),
            number_of_cities,
            City());
}

void InstanceBuilder::set_weight(
        CityId city_id,
        Weight weight)
{
    instance_.cities_[city_id].weight = weight;
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
    travelingsalesmansolver::DistancesBuilder distances_builder;

    std::string tmp;
    std::vector<std::string> line;
    while (getline(file, tmp)) {
        line = optimizationtools::split(tmp);
        if (line.size() == 0) {
        } else if (distances_builder.read_tsplib(file, tmp, line)) {
        } else if (tmp.rfind("NAME", 0) == 0) {
        } else if (tmp.rfind("PROBLEM NAME", 0) == 0) {
        } else if (tmp.rfind("COMMENT", 0) == 0) {
        } else if (tmp.rfind("TYPE", 0) == 0) {
        } else if (tmp.rfind("KNAPSACK DATA TYPE", 0) == 0) {
        } else if (tmp.rfind("DISPLAY_DATA_TYPE", 0) == 0) {
        } else if (tmp.rfind("DIMENSION", 0) == 0) {
            CityId number_of_cities = std::stol(line.back());
            add_cities(number_of_cities);
            distances_builder.set_number_of_vertices(number_of_cities);
        } else if (tmp.rfind("CAPACITY OF KNAPSACK", 0) == 0) {
            set_capacity(std::stol(line.back()));
        } else if (tmp.rfind("MIN SPEED", 0) == 0) {
            set_minimum_speed(std::stod(line.back()));
        } else if (tmp.rfind("MAX SPEED", 0) == 0) {
            set_maximum_speed(std::stod(line.back()));
        } else if (tmp.rfind("RENTING RATIO", 0) == 0) {
            set_renting_ratio(std::stod(line.back()));
        } else if (tmp.rfind("WEIGHTS SECTION", 0) == 0) {
            CityId city = -1;
            Weight weight = -1;
            for (CityId city_id = 0;
                    city_id < instance_.number_of_cities();
                    ++city_id) {
                file >> tmp >> weight;
                set_weight(city, weight);
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

    set_distances(std::shared_ptr<const travelingsalesmansolver::Distances>(
                new travelingsalesmansolver::Distances(distances_builder.build())));
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Build /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Instance InstanceBuilder::build()
{
    // Compute total weight.
    for (CityId city_id = 0;
            city_id < instance_.number_of_cities();
            ++city_id) {
        instance_.total_weight_ += instance_.city(city_id).weight;
    }

    return std::move(instance_);
}
