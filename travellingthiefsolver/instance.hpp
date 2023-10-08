#pragma once

#include "packingwhiletravellingsolver/utils.hpp"

#include "optimizationtools/utils/info.hpp"
#include "optimizationtools/utils/utils.hpp"

#include <random>

namespace travellingthiefsolver
{

using CityId = int64_t;
using CityPos = int64_t;
using CityStateId = packingwhiletravellingsolver::CityStateId;
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

    void compute_distances() const;

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

    /** Get the renting ratio. */
    inline double renting_ratio() const { return renting_ratio_; }

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

    /** Create an instance manually. */
    Instance() { }

    inline void init_distances() const
    {
        distances_ = std::vector<std::vector<Distance>>(number_of_cities());
        for (CityId city_id = 0;
                city_id < number_of_cities();
                ++city_id) {
            distances_[city_id] = std::vector<Distance>(
                    city_id,
                    std::numeric_limits<Distance>::max());
        }
    }

    /** Set the distance between two cities. */
    inline void set_distance(
            CityId city_id_1,
            CityId city_id_2,
            Distance distance) const
    {
        if (city_id_1 > city_id_2) {
            distances_[city_id_1][city_id_2] = distance;
        } else {
            distances_[city_id_2][city_id_1] = distance;
        }
    }

    /*
     * Private attributes
     */

    /** Cities. */
    std::vector<City> cities_;

    /** Distances. */
    mutable std::vector<std::vector<Distance>> distances_;

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

    friend class InstanceBuilder;

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
    if (!distances_.empty()) {
        if (city_id_1 > city_id_2) {
            return distances_[city_id_1][city_id_2];
        } else {
            return distances_[city_id_2][city_id_1];
        }
    } else if (edge_weight_type_ == "EUC_2D") {
        double xd = x(city_id_2) - x(city_id_1);
        double yd = y(city_id_2) - y(city_id_1);
        return std::round(std::sqrt(xd * xd + yd * yd));
    } else if (edge_weight_type_ == "CEIL_2D") {
        double xd = x(city_id_2) - x(city_id_1);
        double yd = y(city_id_2) - y(city_id_1);
        return std::ceil(std::sqrt(xd * xd + yd * yd));
    } else if (edge_weight_type_ == "ATT") {
        double xd = x(city_id_1) - x(city_id_2);
        double yd = y(city_id_1) - y(city_id_2);
        double rij = sqrt((xd * xd + yd * yd) / 10.0);
        int tij = std::round(rij);
        return (tij < rij)? tij + 1: tij;
    } else {
        throw std::invalid_argument(
                "Unknown edge weight type \"" + edge_weight_type_ + "\".");
        return -1;
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

