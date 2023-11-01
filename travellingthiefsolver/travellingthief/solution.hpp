#pragma once

#include "travellingthiefsolver/travellingthief/instance.hpp"

#include <string>

namespace travellingthiefsolver
{
namespace travellingthief
{

/**
 * Structure for a city in a solution.
 */
struct SolutionCity
{
    /** Position of the city in the solution. */
    CityPos position = -1;

    /** Number of selected items. */
    ItemId number_of_items = 0;

    /** Weight of the selected items. */
    Weight weight = 0;

    /** Profit of the selected items. */
    Profit profit = 0;
};

/**
 * Solution class for a 'travellingthief' problem.
 */
class Solution
{

public:

    /*
     * Constructors and destructor.
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

    /** Get a city. */
    inline const SolutionCity& city(CityId city_id) const { return cities_[city_id]; }

    /** Get the number of items selected in the solution. */
    inline ItemId number_of_items() const { return item_ids_.size(); }

    /** Return 'true' iff the solution contains a given item. */
    inline bool contains(ItemId item_id) const { return items_is_selected_[item_id]; }

    /** Get the profit of the items selected in the solution. */
    inline Profit item_profit() const { return item_profit_; }

    /** Get the weight of the items selected in the solution. */
    inline Weight item_weight() const { return item_weight_; }

    /** Get the full distance of the solution. */
    inline Distance distance() const { return distance_; }

    /** Get the full travel time of the solution. */
    inline Time travel_time() const { return travel_time_; }

    /** Get the renting cost. */
    inline Profit renting_cost() const { return instance().renting_ratio() * travel_time_; }

    /** Get the objective value of the solution. */
    inline Profit objective() const { return item_profit() - renting_cost(); }

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

    /**
     * Add an item to the solution.
     *
     * The item must be located in the last city of the solution.
     */
    void add_item(ItemId item_id);

    /*
     * Export.
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the solution to a file. */
    void write(std::string certificate_path) const;

    void write_csv(std::string output_path) const;

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
    std::vector<SolutionCity> cities_;

    /** List of selected items. */
    std::vector<ItemId> item_ids_;

    /**
     * Array indexed by items indicating whether of not they are selected in
     * the solution.
     */
    std::vector<uint8_t> items_is_selected_;

    /** Profit from the selected items. */
    Profit item_profit_ = 0.0;

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
 * Output structure for a 'travellingthief' problem.
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
    Profit bound = std::numeric_limits<Profit>::infinity();

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

