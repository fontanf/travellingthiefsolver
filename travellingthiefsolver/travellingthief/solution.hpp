#pragma once

#include "travellingthiefsolver/travellingthief/instance.hpp"

#include "optimizationtools//utils/utils.hpp"
#include "optimizationtools//utils/output.hpp"

#include <string>
#include <iomanip>

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
     * Constructors and destructor
     */

    /** Create an empty solution. */
    Solution(const Instance& instance);

    /** Create a solution from a certificate file. */
    template <typename Distances>
    Solution(
            const Distances& distances,
            const Instance& instance,
            const std::string& certificate_path);

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
    inline Profit objective_value() const { return item_profit() - renting_cost(); }

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
    template <typename Distances>
    void add_city(
            const Distances& distances,
            CityId city_id);

    /**
     * Add an item to the solution.
     *
     * The item must be located in the last city of the solution.
     */
    template <typename Distances>
    void add_item(
            const Distances& distances,
            ItemId item_id);

    /*
     * Export.
     */

    /** Print the instance. */
    std::ostream& format(
            std::ostream& os,
            int verbosity_level = 1) const;

    /** Export solution characteristics to a JSON structure. */
    nlohmann::json to_json() const;

    /** Write the solution to a file. */
    void write(
            const std::string& certificate_path) const;

    void write_csv(
            const std::string& output_path) const;

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

inline optimizationtools::ObjectiveDirection objective_direction()
{
    return optimizationtools::ObjectiveDirection::Maximize;
}

/**
 * Output structure for a set covering problem.
 */
struct Output: optimizationtools::Output
{
    /** Constructor. */
    Output(
            const Instance& instance):
        solution(instance),
        bound(instance.total_item_profit()) { }


    /** Solution. */
    Solution solution;

    /** Bound. */
    Profit bound = std::numeric_limits<Profit>::infinity();

    /** Elapsed time. */
    double time = 0.0;


    std::string solution_value() const
    {
        return optimizationtools::solution_value(
            objective_direction(),
            solution.feasible(),
            solution.objective_value());
    }

    double absolute_optimality_gap() const
    {
        return optimizationtools::absolute_optimality_gap(
                objective_direction(),
                solution.feasible(),
                solution.objective_value(),
                bound);
    }

    double relative_optimality_gap() const
    {
       return optimizationtools::relative_optimality_gap(
            objective_direction(),
            solution.feasible(),
            solution.objective_value(),
            bound);
    }

    virtual nlohmann::json to_json() const
    {
        return nlohmann::json {
            {"Solution", solution.to_json()},
            {"Value", solution_value()},
            {"Bound", bound},
            {"AbsoluteOptimalityGap", absolute_optimality_gap()},
            {"RelativeOptimalityGap", relative_optimality_gap()},
            {"Time", time}
        };
    }

    virtual int format_width() const { return 30; }

    virtual void format(std::ostream& os) const
    {
        int width = format_width();
        os
            << std::setw(width) << std::left << "Value: " << solution_value() << std::endl
            << std::setw(width) << std::left << "Bound: " << bound << std::endl
            << std::setw(width) << std::left << "Absolute optimality gap: " << absolute_optimality_gap() << std::endl
            << std::setw(width) << std::left << "Relative optimality gap (%): " << relative_optimality_gap() * 100 << std::endl
            << std::setw(width) << std::left << "Time (s): " << time << std::endl
            ;
    }
};

using NewSolutionCallback = std::function<void(const Output&, const std::string&)>;

struct Parameters: optimizationtools::Parameters
{
    /** Callback function called when a new best solution is found. */
    NewSolutionCallback new_solution_callback = [](const Output&, const std::string&) { };


    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = optimizationtools::Parameters::to_json();
        return json;
    }

    virtual int format_width() const override { return 23; }

    virtual void format(std::ostream& os) const override
    {
        optimizationtools::Parameters::format(os);
    }
};

template <typename Distances>
Solution::Solution(
        const Distances& distances,
        const Instance& instance,
        const std::string& certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    char sep;
    file >> sep;

    std::vector<CityId> city_ids;
    CityId city_id = -1;
    for (CityId city_pos = 0;
            city_pos < instance.number_of_cities();
            ++city_pos) {
        file >> city_id >> sep;
        city_id--;
        city_ids.push_back(city_id);
    }

    ItemId item_id;
    CityId city_pos = 0;
    file >> sep;
    for (;;) {
        file >> item_id >> sep;
        if (file.eof())
            break;
        item_id--;
        const Item& item = instance.item(item_id);
        while (item.city_id != city_ids[city_pos]) {
            city_pos++;
            add_city(distances, city_ids[city_pos]);
        }
        add_item(distances, item_id);
    }
}

template <typename Distances>
void Solution::add_city(
        const Distances& distances,
        CityId city_id)
{
    // Check that the city has not already been visited.
    if (cities_[city_id].position >= 0) {
        throw std::runtime_error(
                "travellingthiefsolver::Solution::add_city(CityId)."
                "City " + std::to_string(city_id) + " is already in the solution.");
    }

    CityId city_id_prev = city_ids_.back();
    cities_[city_id].position = city_ids_.size();
    distance_cur_ += distances.distance(city_id_prev, city_id);
    distance_ = distance_cur_ + distances.distance(city_id, 0);
    travel_time_cur_ += instance().duration(distances, city_id_prev, city_id, item_weight_);
    travel_time_ = travel_time_cur_ + instance().duration(distances, city_id, 0, item_weight_);
    city_ids_.push_back(city_id);
}

template <typename Distances>
void Solution::add_item(
        const Distances& distances,
        ItemId item_id)
{
    const Item& item = instance().item(item_id);

    // Check that the item is available at the last visited city.
    if (item.city_id != city_ids_.back()) {
        throw std::runtime_error("");  // TODO
    }

    // Check that the item has not already been selected.
    if (items_is_selected_[item_id]) {
        throw std::runtime_error("");  // TODO
    }

    items_is_selected_[item_id] = 1;
    item_ids_.push_back(item_id);
    item_profit_ += item.profit;
    item_weight_ += item.weight;
    CityId city_id = city_ids_.back();
    travel_time_ = travel_time_cur_ + instance().duration(
            distances,
            city_id,
            0,
            item_weight_);

    cities_[city_id].number_of_items++;
    cities_[city_id].profit += item.profit;
    cities_[city_id].weight += item.weight;
}

}
}
