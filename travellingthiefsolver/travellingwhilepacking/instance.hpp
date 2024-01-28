#pragma once

#include "travelingsalesmansolver/distances.hpp"

#include <memory>

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

    /*
     * Getters
     */

    /** Get the number of cities. */
    inline CityId number_of_cities() const { return cities_.size(); }

    /** Get a city. */
    inline const City& city(CityId city_id) const { return cities_[city_id]; }

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

    /** Total weight. */
    Weight total_weight_ = 0;

    /** Capacity of the knapsack. */
    Weight capacity_ = -1;

    friend class InstanceBuilder;

};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Inlined methods ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
    return (double)distances().distance(city_id_1, city_id_2) / speed;
}

}
}
