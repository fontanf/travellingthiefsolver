/**
 * Thief orienterring problem.
 *
 * Input:
 * - n cities and an n×n symmetric matrix d containing the distances between
 *   each pair of cities
 * - m items; for each item i = 1..m
 *   - a profit pᵢ
 *   - a weight wᵢ
 *   - a location nᵢ
 * - A time limit T
 * - A capacity c
 * - A maximum speed vmax and a minimum speed vmin such that the time to travel
 *   from city location_id_1 to city location_id_2 is equal to:
 *       d(location_id_1, location_id_2) / (vmax - w * (vmax - vmin) / c)
 *   with w the sum of the weights of the currently collected items
 * Problem:
 * - find a route from city 1 to city n and a set of items to collect such
 *   that:
 *   - an item can only be collected in the city in which it is located
 *   - the total weight of the collected items does not exceed the capacity
 *   - the total time of the route does not exceed the time limit
 * Objective:
 * - maximize the profit of collected items
 *
 */

#pragma once

#include "travellingthiefsolver/packingwhiletravelling/utils.hpp"

#include "optimizationtools/utils/info.hpp"
#include "optimizationtools/utils/utils.hpp"

#include <random>

namespace travellingthiefsolver
{
namespace thieforienteering
{

using CityId = int64_t;
using CityPos = int64_t;
using CityStateId = packingwhiletravelling::CityStateId;
using Distance = int64_t;
using Time = double;
using ItemId = int64_t;
using Profit = double;
using Weight = int64_t;
using Counter = int64_t;
using Seed = int64_t;

/**
 * Structure for an item.
 */
struct Item
{
    /** City. */
    CityId city_id;

    /** Weight of the item. */
    Weight weight;

    /** Profit of the item. */
    Profit profit;
};

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

    /** Items available at the city. */
    std::vector<ItemId> item_ids;
};

/**
 * Instance class for a 'thieforienteering' problem.
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
        if (city_id_1 > city_id_2) {
            distances_[city_id_1][city_id_2] = distance;
        } else {
            distances_[city_id_2][city_id_1] = distance;
        }
    }

    /** Set the minimum speed. */
    void set_minimum_speed(double minimum_speed) { speed_min_ = minimum_speed; }

    /** Set the maximum speed. */
    void set_maximum_speed(double maximum_speed) { speed_max_ = maximum_speed; }

    /** Set the time limit. */
    void set_time_limit(double time_limit) { time_limit_ = time_limit; }

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { capacity_ = capacity; }

    /** Add an item. */
    void add_item(
            CityId city_id,
            Weight weight,
            Profit profit);

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

    inline const std::vector<std::vector<Distance>>& distances() const { return distances_; }

    /** Get the speed for a given weight. */
    inline double speed(
            Weight weight) const;

    /** Get the duration between two cities. */
    inline Time duration(
            CityId city_id_1,
            CityId city_id_2,
            Weight weight) const;

    /** Get the number of items. */
    inline ItemId number_of_items() const { return items_.size(); }

    /** Get an item. */
    inline const Item& item(ItemId item_id) const { return items_[item_id]; }

    /** Get the capacity of the knapsack. */
    inline Weight capacity() const { return capacity_; }

    /** Get the minimum speed. */
    inline double minimum_speed() const { return speed_min_; }

    /** Get the maximum speed. */
    inline double maximum_speed() const { return speed_max_; }

    /** Get the time limit. */
    inline double time_limit() const { return time_limit_; }

    /** Get the edge weight type. */
    inline std::string edge_weight_type() const { return edge_weight_type_; }

    /** Get the node coord type. */
    inline std::string node_coord_type() const { return node_coord_type_; }

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

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
    std::vector<std::vector<Distance>> distances_;

    /** Minimum speed. */
    double speed_min_ = -1;

    /** Maximum speed. */
    double speed_max_ = -1;

    /** Time limit. */
    Time time_limit_ = -1;

    /** Items. */
    std::vector<Item> items_;

    /** Capacity of the knapsack. */
    Weight capacity_ = -1;

    /** Profit sum. */
    Profit profit_sum_ = 0;

    /** Weight sum. */
    Weight weight_sum_ = 0;

    /**
     * Edge weight type.
     *
     * See http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf
     */
    std::string edge_weight_type_ = "EXPLICIT";

    /**
     * Node coord type.
     *
     * See http://comopt.ifi.uni-heidelberg.de/software/TSPLIB95/tsp95.pdf
     */
    std::string node_coord_type_ = "TWOD_COORDS";

};

void init_display(
        const Instance& instance,
        optimizationtools::Info& info);

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline Distance Instance::distance(
        CityId city_id_1,
        CityId city_id_2) const
{
    if (city_id_1 > city_id_2) {
        return distances_[city_id_1][city_id_2];
    } else {
        return distances_[city_id_2][city_id_1];
    }
}

inline double Instance::speed(
        Weight weight) const
{
    if (weight > capacity())
        return speed_min_;
    return speed_max_ - (double)(weight * (speed_max_ - speed_min_)) / capacity();
}

inline Time Instance::duration(
        CityId city_id_1,
        CityId city_id_2,
        Weight weight) const
{
    return (double)distance(city_id_1, city_id_2) / speed(weight);
}

}
}

