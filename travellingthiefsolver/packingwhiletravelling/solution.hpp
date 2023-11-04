#pragma once

#include "travellingthiefsolver/packingwhiletravelling/instance.hpp"

#include <string>

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

/**
 * Solution class for a 'packingwhiletravelling' problem.
 */
class Solution
{

public:

    /*
     * Constructors and destructor.
     */

    void update(const Solution& solution);

    /*
     * Getters
     */

    /** Get the instance. */
    inline const Instance& instance() const { return *instance_; }

    /** Get the number of items selected in the solution. */
    inline ItemId number_of_items() const { return item_ids_.size(); }

    /** Return 'true' iff the solution contains the item. */
    inline bool contains(ItemId item_id) const { return items_is_selected_[item_id]; }

    /** Get the profit of the items selected in the solution. */
    inline Profit item_profit() const { return item_profit_; }

    /** Get the weight of the items selected in the solution. */
    inline Weight item_weight() const { return item_weight_; }

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

    bool is_strictly_better_than(const Solution& solution) const;

    /*
     * Export.
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the solution to a file. */
    void write(std::string certificate_path) const;

private:

    /*
     * Private methods
     */

    Solution() { }

    /*
     * Private attributes
     */

    /** Instance. */
    const Instance* instance_;

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

    friend class SolutionBuilder;

};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Output structure for a 'packingwhiletravelling' problem.
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

