#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

using CityId = int64_t;
using CityPos = int64_t;
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
    /** Distance from the previous city. */
    Distance distance = 0;

    /**
     * Default weight added to the knapsack at the city.
     *
     * This weight comes from reduction operations.
     */
    Weight weight = 0;

    /** Items available at the city. */
    std::vector<ItemId> item_ids;

    /*
     * Computed attributes
     */

    /** Distance from the first city. */
    Distance distance_from_start;

    /** Distance to the last city (including return to the first city). */
    Distance distance_to_end;

    /** City weight from the first city. */
    Weight weight_from_start;
};

/**
 * Instance class for a 'packingwhiletravelling' problem.
 */
class Instance
{

public:

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

    /** Get the duration between its previous city and a city. */
    inline Time duration(
            CityId city_id,
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

    /** Get the renting ratio. */
    inline double renting_ratio() const { return renting_ratio_; }

    /** Get the distance. */
    inline Distance distance() const { return distance_; }

    /** Get the total weight of the cities. */
    inline Weight city_weight() const { return city_weight_; }

    /** Get the total weight of the cities and the items. */
    inline Weight total_weight() const { return total_weight_; }

    /** Get the total profit of the items. */
    inline Weight total_item_profit() const { return total_item_profit_; }

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& format(
            std::ostream& os,
            int verbosity_level = 1) const;

    /** Write the instance to a file. */
    void write(
            const std::string& instance_path) const;

private:

    /*
     * Private methods
     */

    /** Create an instance manually. */
    Instance() { }

    /*
     * Reduction
     */

    /** Remove unprofitable items. */
    bool reduce_unprofitable_items();

    /** Remove compulsory items. */
    bool reduce_compulsory_items();

    /**
     * Reduce using the algorithm from:
     * - "The Packing While Traveling Problem" (Polyakovskiy and Neumann, 2017)
     *   https://doi.org/10.1016/j.ejor.2016.09.035
     */
    bool reduce_polyakovskiy2017(
            Counter maximum_number_of_rounds);

    /*
     * Private attributes
     */

    /** Cities. */
    std::vector<City> cities_;

    /** Minimum speed. */
    double speed_min_ = -1;

    /** Maximum speed. */
    double speed_max_ = -1;

    /** Renting ratio. */
    double renting_ratio_ = 1.0;

    /** Items. */
    std::vector<Item> items_;

    /** Distance. */
    Distance distance_ = 0;

    /** Capacity of the knapsack. */
    Weight capacity_ = -1;

    /** Weight sum. */
    Weight total_weight_ = 0;

    /** Weight of the cities. */
    Weight city_weight_ = 0;

    /** Total item profit. */
    Profit total_item_profit_ = 0;

    friend class InstanceBuilder;

};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline double Instance::speed(
        Weight weight) const
{
    return speed_max_ - (double)(weight * (speed_max_ - speed_min_)) / capacity();
}

inline Time Instance::duration(
        CityId city_id,
        Weight weight) const
{
    return (double)city(city_id).distance / speed(weight);
}

}
}
