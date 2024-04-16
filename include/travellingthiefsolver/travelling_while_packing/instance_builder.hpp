#pragma once

#include "travellingthiefsolver/travelling_while_packing/instance.hpp"

namespace travellingthiefsolver
{
namespace travelling_while_packing
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Read an instance from a file. */
    void read(
            std::string instance_path,
            std::string format);

    /** Add cities. */
    void add_cities(CityId number_of_cities);

    /** Set the distance between two cities. */
    inline void set_distances(
            const std::shared_ptr<const travelingsalesmansolver::Distances>& distances)
    {
        instance_.distances_ = distances;
    }

    /** Set the minimum speed. */
    void set_minimum_speed(double minimum_speed) { instance_.speed_min_ = minimum_speed; }

    /** Set the maximum speed. */
    void set_maximum_speed(double maximum_speed) { instance_.speed_max_ = maximum_speed; }

    /** Set the renting ratio. */
    void set_renting_ratio(double renting_ratio) { instance_.renting_ratio_ = renting_ratio; }

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { instance_.capacity_ = capacity; }

    /** Set the weight of a city. */
    void set_weight(
            CityId city_id,
            Weight weight);

    /*
     * Build
     */

    /** Build. */
    Instance build();

private:

    /*
     * Private methods
     */

    /*
     * Read input file
     */

    /** Read an instance from a file in 'polyakovskiy2014' format. */
    void read_polyakovskiy2014(std::ifstream& file);

    /*
     * Private attributes
     */

    /** Instance. */
    Instance instance_;

};

}
}
