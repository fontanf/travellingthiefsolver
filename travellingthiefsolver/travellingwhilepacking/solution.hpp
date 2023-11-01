#pragma once

#include "travellingthiefsolver/travellingwhilepacking/instance.hpp"

#include <string>

namespace travellingthiefsolver
{
namespace travellingwhilepacking
{

/**
 * Solution class for a 'travellingwhilepacking' problem.
 */
class Solution
{

public:

    /*
     * Constructors and destructor
     */

    /** Create an empty solution. */
    Solution(const Instance& instance);

    /** Create a solution from a certificate file. */
    Solution(
            const Instance& instance,
            std::string certificate_path);

    /*
     * Getters
     */

    /** Get the instance. */
    inline const Instance& instance() const { return *instance_; }

    /** Get the number of cities in the solution. */
    inline CityId number_of_cities() const { return city_ids_.size(); }

    /** Get the city at a given position. */
    inline CityId city_id(CityPos pos) const { return city_ids_[pos]; }

    /** Get the weight of the items selected in the solution. */
    inline Weight item_weight() const { return item_weight_; }

    /** Get the full distance of the solution. */
    inline Distance distance() const { return distance_; }

    /** Get the full travel time of the solution. */
    inline Time travel_time() const { return travel_time_; }

    /** Get the renting cost. */
    inline Profit renting_cost() const { return instance().renting_ratio() * travel_time_; }

    /**
     * Return 'true' iff the solution is feasible.
     * - all cities have been visited
     * - the total weight of the items is smaller than the capacity of the
     *   knapsack
     */
    bool feasible() const;

    /*
     * Setters
     */

    /** Append a city at the end of the solution. */
    void add_city(CityId city_id);

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the solution to a file. */
    void write(std::string certificate_path) const;

private:

    /*
     * Private attributes.
     */

    /** Instance. */
    const Instance* instance_;

    /** List of visited cities. */
    std::vector<CityId> city_ids_;

    /**
     * Array indexed by cities indicating whether of not they have been
     * visited.
     */
    std::vector<uint8_t> cities_is_visited_;

    /** Weight. */
    Weight item_weight_ = 0;

    /**
     * Distance.
     *
     * This distance doesn't take into account the return to the depot.
     */
    Distance distance_cur_ = 0;

    /**
     * Full distance.
     *
     * This distance takes into account the return to the depot.
     */
    Distance distance_ = 0;

    /**
     * Travel time.
     *
     * This time doesn't take into account the return to the depot.
     */
    Time travel_time_cur_ = 0;

    /**
     * Full travel time.
     *
     * This time takes into account the return to the depot.
     */
    Time travel_time_ = 0;

};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Output structure for a 'travellingwhilepacking' problem.
 */
struct Output
{
    /** Constructor. */
    Output(
            const Instance& instance,
            optimizationtools::Info& info);

    /** Solution. */
    Solution solution;

    /** Bound. */
    Profit bound = -std::numeric_limits<Profit>::infinity();

    /** Elapsed time. */
    double time = -1;

    /** Print current state. */
    void print(
            optimizationtools::Info& info,
            const std::stringstream& s) const;

    /** Update the solution. */
    void update_solution(
            const Solution& solution_new,
            const std::stringstream& s,
            optimizationtools::Info& info);

    /** Update the bound. */
    void update_bound(
            Profit bound_new,
            const std::stringstream& s,
            optimizationtools::Info& info);

    /** Print the algorithm statistics. */
    virtual void print_statistics(
            optimizationtools::Info& info) const { (void)info; }

    /** Method to call at the end of the algorithm. */
    Output& algorithm_end(optimizationtools::Info& info);
};

}
}

