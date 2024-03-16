#pragma once

#include "travellingthiefsolver/travellingwhilepacking/instance.hpp"

#include "optimizationtools//utils/utils.hpp"
#include "optimizationtools//utils/output.hpp"

#include <string>
#include <iomanip>

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

    /** Get the objective value of the solution. */
    inline Time objective_value() const { return travel_time(); }

    /*
     * Setters
     */

    /** Append a city at the end of the solution. */
    template <typename Distances>
    void add_city(
            const Distances& distances,
            CityId city_id);

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& format(
            std::ostream& os,
            int verbosity_level = 1) const;

    /** Write the solution to a file. */
    void write(
            const std::string& certificate_path) const;

    /** Export solution characteristics to a JSON structure. */
    nlohmann::json to_json() const;

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

inline optimizationtools::ObjectiveDirection objective_direction()
{
    return optimizationtools::ObjectiveDirection::Minimize;
}

/**
 * Output structure for a set covering problem.
 */
struct Output: optimizationtools::Output
{
    /** Constructor. */
    Output(const Instance& instance): solution(instance) { }


    /** Solution. */
    Solution solution;

    /** Bound. */
    Profit bound = -std::numeric_limits<Profit>::infinity();

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
void Solution::add_city(
        const Distances& distances,
        CityId city_id)
{
    // Check that the city has not already been visited.
    if (cities_is_visited_[city_id]) {
        throw std::runtime_error("");  // TODO
    }

    CityId city_id_prev = city_ids_.back();
    city_ids_.push_back(city_id);
    cities_is_visited_[city_id] = true;
    distance_cur_ += distances.distance(city_id_prev, city_id);
    distance_ = distance_cur_ + distances.distance(city_id, 0);
    travel_time_cur_ += instance().duration(distances, city_id_prev, city_id, item_weight_);
    item_weight_ += instance().city(city_id).weight;
    travel_time_ = travel_time_cur_ + instance().duration(distances, city_id, 0, item_weight_);
}

}
}
