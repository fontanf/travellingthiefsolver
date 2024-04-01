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

#include "travellingthiefsolver/packing_while_travelling/utils.hpp"

#include "travelingsalesmansolver/distances/distances.hpp"

#include <memory>

namespace travellingthiefsolver
{
namespace thief_orienteering
{

using CityId = int64_t;
using CityPos = int64_t;
using CityStateId = packing_while_travelling::CityStateId;
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
    /** Items available at the city. */
    std::vector<ItemId> item_ids;
};

/**
 * Instance class for a 'thief_orienteering' problem.
 */
class Instance
{

public:

    /*
     * Constructors and destructor
     */

    /*
     * Getters
     */

    /** Get the number of cities. */
    inline CityId number_of_cities() const { return cities_.size(); }

    /** Get a city. */
    inline const City& city(CityId city_id) const { return cities_[city_id]; }

    /** Get the speed for a given weight. */
    inline double speed(
            Weight weight) const;

    /** Get the duration between two cities. */
    template <typename Distances>
    inline Time duration(
            const Distances& distances,
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

    /** Get distances. */
    const travelingsalesmansolver::Distances& distances() const { return *distances_; }

    /** Get the shared pointer to the distances class. */
    const std::shared_ptr<const travelingsalesmansolver::Distances>& distances_ptr() const { return distances_; }

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& format(
            std::ostream& os,
            int verbosity_level = 1) const;

private:

    /*
     * Private methods
     */

    /** Create an instance manually. */
    Instance() { }

    /*
     * Private attributes
     */

    /** Cities. */
    std::vector<City> cities_;

    /** Distances. */
    std::shared_ptr<const travelingsalesmansolver::Distances> distances_;

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

    friend class InstanceBuilder;

};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline double Instance::speed(
        Weight weight) const
{
    if (weight > capacity())
        return speed_min_;
    return speed_max_ - (double)(weight * (speed_max_ - speed_min_)) / capacity();
}

template <typename Distances>
inline Time Instance::duration(
        const Distances& distances,
        CityId city_id_1,
        CityId city_id_2,
        Weight weight) const
{
    return (double)distances.distance(city_id_1, city_id_2) / speed(weight);
}

}
}
