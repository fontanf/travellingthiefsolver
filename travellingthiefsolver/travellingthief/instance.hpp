#pragma once

#include "travellingthiefsolver/packingwhiletravelling/utils.hpp"

#include "travelingsalesmansolver/distances/distances.hpp"

#include <memory>

namespace travellingthiefsolver
{
namespace travellingthief
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
 * Instance class for a 'travellingthief' problem.
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

    /** Get the renting ratio. */
    inline double renting_ratio() const { return renting_ratio_; }

    /** Get distances. */
    const travelingsalesmansolver::Distances& distances() const { return *distances_; }

    /** Get the shared pointer to the distances class. */
    const std::shared_ptr<const travelingsalesmansolver::Distances>& distances_ptr() const { return distances_; }

    /** Get the total weight of the items. */
    inline Weight total_item_weight() const { return total_item_weight_; }

    /** Get the total profit of the items. */
    inline Weight total_item_profit() const { return total_item_profit_; }

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

    /** Renting ratio. */
    double renting_ratio_ = 1.0;

    /** Items. */
    std::vector<Item> items_;

    /** Capacity of the knapsack. */
    Weight capacity_ = -1;

    /** Profit sum. */
    Profit total_item_profit_ = 0;

    /** Weight sum. */
    Weight total_item_weight_ = 0;

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
