#pragma once

#include "optimizationtools/utils/info.hpp"
#include "optimizationtools/utils/utils.hpp"

#include <random>

namespace travellingthiefsolver
{
namespace travellingwhilepacking
{

using CityId = int64_t;
using CityPos = int64_t;
using Distance = int64_t;
using Time = double;
using Weight = int64_t;
using Profit = double;
using Counter = int64_t;
using Seed = int64_t;

/**
 * Structure for a city.
 */
struct City
{
    /** x-coordinate. */
    double x;

    /** y-coordinate. */
    double y;

    /** z-coordinate. */
    double z;

    /** Weight of the items retrieve at this city. */
    Weight weight;
};

/**
 * Instance class for a 'travellingwhilepacking' problem.
 */
class Instance
{

public:

    /*
     * Constructors and destructor
     */

    /** Create an instance from a file. */
    Instance(
            std::string instance_path,
            std::string format);

    /** Create an instance manually. */
    Instance(CityId number_of_cities = 0);

    /** Create an instance manually. */
    Instance(const std::vector<std::vector<Distance>>& distances);

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
            Distance distance);

    /** Set the minimum speed. */
    void set_minimum_speed(double minimum_speed) { speed_min_ = minimum_speed; }

    /** Set the maximum speed. */
    void set_maximum_speed(double maximum_speed) { speed_max_ = maximum_speed; }

    /** Set the renting ratio. */
    void set_renting_ratio(double renting_ratio) { renting_ratio_ = renting_ratio; }

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { capacity_ = capacity; }

    /** Set the weight of a city. */
    void set_weight(
            CityId city_id,
            Weight weight);

    /*
     * Getters
     */

    /** Get the number of cities. */
    inline CityId number_of_cities() const { return cities_.size(); }

    /** Get a city. */
    inline const City& city(CityId city_id) const { return cities_[city_id]; }

    /** Get the x-coordinate of a city. */
    inline double x(CityId city_id) const { return cities_[city_id].x; }

    /** Get the y-coordinate of a city. */
    inline double y(CityId city_id) const { return cities_[city_id].y; }

    /** Get the distance between two cities. */
    inline Distance distance(
            CityId city_id_1,
            CityId city_id_2) const;

    /** Get the duration between two cities. */
    inline Time duration(
            CityId city_id_1,
            CityId city_id_2,
            Weight weight) const;

    /** Get the total weight of the cities. */
    inline Weight total_weight() const { return total_weight_; }

    /** Get the capacity of the knapsack. */
    inline Weight capacity() const { return capacity_; }

    /** Get the renting ratio. */
    inline double renting_ratio() const { return renting_ratio_; }

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the instance to a file. */
    void write(std::string instance_path) const;

private:

    /*
     * Private methods
     */

    /** Read an instance from a file in 'polyakovskiy2014' format. */
    void read_polyakovskiy2014(std::ifstream& file);

    /*
     * Private attributes
     */

    /** Cities. */
    std::vector<City> cities_;

    /** Distances. */
    std::vector<std::vector<Distance>> distances_0_;

    const std::vector<std::vector<Distance>>* distances_;

    /** Minimum speed. */
    double speed_min_ = -1;

    /** Maximum speed. */
    double speed_max_ = -1;

    /** Renting ratio. */
    double renting_ratio_ = 1.0;

    /** Total weight. */
    Weight total_weight_ = 0;

    /** Capacity of the knapsack. */
    Weight capacity_ = -1;

};

void init_display(
        const Instance& instance,
        optimizationtools::Info& info);

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline void Instance::set_distance(
        CityId city_id_1,
        CityId city_id_2,
        Distance distance)
{
    if (city_id_1 > city_id_2) {
        distances_0_[city_id_1][city_id_2] = distance;
    } else {
        distances_0_[city_id_2][city_id_1] = distance;
    }
}

inline Distance Instance::distance(
        CityId city_id_1,
        CityId city_id_2) const
{
    if (city_id_1 > city_id_2) {
        return (*distances_)[city_id_1][city_id_2];
    } else {
        return (*distances_)[city_id_2][city_id_1];
    }
}

inline Time Instance::duration(
        CityId city_id_1,
        CityId city_id_2,
        Weight weight) const
{
    //if (weight > capacity()) {
    //    throw std::runtime_error(
    //            "Instance::duration. weight: " + std::to_string(weight)
    //            + "; capacity: " + std::to_string(capacity()) + ".");
    //}
    double speed = speed_max_ - (double)(weight * (speed_max_ - speed_min_)) / capacity();
    return (double)distance(city_id_1, city_id_2) / speed;
}

}
}

