#pragma once

#include "travellingthiefsolver/packing_while_travelling/instance.hpp"

#include "nlohmann/json.hpp"

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

/**
 * Solution class for a 'packing_while_travelling' problem.
 */
class Solution
{

public:

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
    inline Profit objective_value() const { return item_profit() - renting_cost(); }

    /**
     * Return 'true' iff the solution is feasible.
     * - all cities have been visited
     * - the total weight of the items is smaller than the capacity of the
     *   knapsack
     */
    bool feasible() const;

    /*
     * Export
     */

    /** Write the solution to a file. */
    void write(
            const std::string& certificate_path) const;

    /** Export solution characteristics to a JSON structure. */
    nlohmann::json to_json() const;

    /** Print the instance. */
    std::ostream& format(
            std::ostream& os,
            int verbosity_level = 1) const;

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
     * Array indexed by items indicating whether or not they are selected in
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

}
}
