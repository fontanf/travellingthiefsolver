#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

struct EfficientLocalSearchOptionalParameters
{
    /** Initial solution. */
    const Solution* initial_solution = nullptr;

    /** LKH candidate file content. */
    std::string lkh_candidate_file_content;

    /** Enable change-city-state neighborhood. */
    int neighborhood_change_city_state = -1;

    /** Enable two-opt neighborhood. */
    int neighborhood_two_opt = 1;

    /** Enable two-opt-change-city-states neighborhood. */
    int neighborhood_two_opt_change_city_states = 1;

    /** Enable shift neighborhood. */
    int neighborhood_shift = -1;

    /** Enable shift-change-city-state neighborhood. */
    int neighborhood_shift_change_city_state = -1;

    /** Enable shift-change-city-state-2 neighborhood. */
    int neighborhood_shift_change_city_state_2 = 1;

    /** Enable change-two-city-states neighborhood. */
    int neighborhood_change_two_city_states = 0;

    /** Info structure. */
    optimizationtools::Info info = optimizationtools::Info();
};

struct EfficientLocalSearchOutput: Output
{
    EfficientLocalSearchOutput(
            const Instance& instance,
            optimizationtools::Info& info):
        Output(instance, info) { }

    void print_statistics(
            optimizationtools::Info& info) const override;

    Counter number_of_local_search_calls = 0;
    Counter number_of_iterations = 0;
    Counter number_of_improvements = 0;
    Counter number_of_change_city_state_improvements = 0;
    Counter number_of_two_opt_improvements = 0;
    Counter number_of_two_opt_change_city_states_improvements = 0;
    Counter number_of_shift_improvements = 0;
    Counter number_of_shift_change_city_state_improvements = 0;
    Counter number_of_change_two_city_states_improvements = 0;
};

EfficientLocalSearchOutput efficient_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        EfficientLocalSearchOptionalParameters parameters = {});

EfficientLocalSearchOutput efficient_genetic_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        EfficientLocalSearchOptionalParameters parameters = {});

}
}

