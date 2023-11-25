#pragma once

#include "travellingthiefsolver/thieforienteering/instance.hpp"

namespace travellingthiefsolver
{
namespace thieforienteering
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Create an instance from a file. */
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

    /** Set the time limit. */
    void set_time_limit(double time_limit) { instance_.time_limit_ = time_limit; }

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { instance_.capacity_ = capacity; }

    /** Add an item. */
    void add_item(
            CityId city_id,
            Weight weight,
            Profit profit);

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
