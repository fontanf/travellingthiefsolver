#pragma once

#include "travellingthiefsolver/travellingthief/instance.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Create an instance from a file. */
    InstanceBuilder(
            std::string instance_path,
            std::string format);

    /** Add cities. */
    void add_cities(CityId number_of_cities);

    /** Set the coordinates of a city. */
    void set_xy(
            CityId city_id,
            double x,
            double y,
            double z = -1);

    /** Set the distance between two cities. */
    inline void set_distance(
            CityId city_id_1,
            CityId city_id_2,
            Distance distance)
    {
        instance_.set_distance(city_id_1, city_id_2, distance);
    }

    /** Set the minimum speed. */
    void set_minimum_speed(double minimum_speed) { instance_.speed_min_ = minimum_speed; }

    /** Set the maximum speed. */
    void set_maximum_speed(double maximum_speed) { instance_.speed_max_ = maximum_speed; }

    /** Set the renting ratio. */
    void set_renting_ratio(double renting_ratio) { instance_.renting_ratio_ = renting_ratio; }

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
