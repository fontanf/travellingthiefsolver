#pragma once

#include "packingwhiletravellingsolver/instance.hpp"

namespace packingwhiletravellingsolver
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Add cities. */
    void add_cities(CityId number_of_cities);

    /** Add a city. */
    void add_city(
            CityId city_id,
            Weight weight,
            Distance distance);

    /** Set the default weight of a city. */
    void add_weight(
            CityId city_id,
            Weight weight)
    {
        instance_.cities_[city_id].weight += weight;
    }

    /** Set the distance between a city and the next city. */
    void set_distance(
            CityId city_id,
            Distance distance);

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


    /** Read an instance from a file. */
    void read(
            std::string instance_path,
            std::string format);

    /*
     * Build
     */

    /** Build. */
    Instance build();

private:

    /*
     * Private methods
     */

    /** Compute distance_from_start and distance_to_end for each city. */
    void compute_distances_start_end();

    /** Compute the sum of all the weights (items and cities). */
    void compute_weight_sum();

    /** Compute weight_from_start for each city. */
    void compute_weight_from_start();

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
