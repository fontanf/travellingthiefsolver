#pragma once

#include "travellingthiefsolver/travelling_thief/solution.hpp"

#include "travellingthiefsolver/packing_while_travelling/solution.hpp"
#include "travellingthiefsolver/packing_while_travelling/instance_builder.hpp"
#include "travellingthiefsolver/travelling_while_packing/solution.hpp"
#include "travelingsalesmansolver/solution.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

namespace travellingthiefsolver
{
namespace travelling_thief
{

inline Profit slightly_greater(Profit objective)
{
    if (objective > 0)
        return objective * (1 + 1e-8);
    if (objective < 0)
        return objective * (1 - 1e-8);
    return 1e-8;
}

/*
 * TSP
 */

travelingsalesmansolver::Instance create_tsp_instance(
        const Instance& instance);

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution);

template <typename Distances>
Solution retrieve_solution_2(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution);

template <typename Distances>
std::vector<Solution> solve_tsp_lkh(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Instance& tsp_instance,
        std::mt19937_64& generator,
        const Parameters& parameters,
        std::string& lkh_candidate_file_content);

/*
 * PWT
 */

template <typename Distances>
packing_while_travelling::Instance create_pwt_instance(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred);

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred,
        const packing_while_travelling::Solution& pwt_solution);

/*
 * TWP
 */

travelling_while_packing::Instance create_twp_instance(
        const Instance& instance,
        const Solution& solution_pred);

template <typename Distances>
travelling_while_packing::Solution create_twp_solution(
        const Distances& distances,
        const travelling_while_packing::Instance& twp_instance,
        const Solution& solution);

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred,
        const travelling_while_packing::Solution& twp_solution);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution)
{
    Solution solution(instance);
    for (CityId pos = 1; pos < instance.number_of_cities(); ++pos) {
        CityId city_id = tsp_solution.vertex_id(pos);
        solution.add_city(distances, city_id);
    }
    return solution;
}

template <typename Distances>
Solution retrieve_solution_2(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution)
{
    Solution solution(instance);
    for (CityId pos = instance.number_of_cities() - 1; pos > 0; --pos) {
        CityId city_id = tsp_solution.vertex_id(pos);
        solution.add_city(distances, city_id);
    }
    return solution;
}

template <typename Distances>
std::vector<Solution> solve_tsp_lkh(
        const Distances& distances,
        const Instance& instance,
        const travelingsalesmansolver::Instance& tsp_instance,
        std::mt19937_64& generator,
        const Parameters& parameters,
        std::string& lkh_candidate_file_content)
{
    std::uniform_int_distribution<Counter> d_seed(1, 1e8);
    travelingsalesmansolver::LkhParameters tsp_parameters;
    tsp_parameters.timer = parameters.timer;
    tsp_parameters.verbosity_level = 0;
    if (instance.number_of_cities() < 1e4) {
        tsp_parameters.candidate_set_type = "DELAUNAY";
    } else {
        tsp_parameters.candidate_set_type = "POPMUSIC";
    }
    tsp_parameters.initial_period = "100";
    tsp_parameters.runs = "1";
    tsp_parameters.max_trials = "1";
    tsp_parameters.seed = std::to_string(d_seed(generator));
    tsp_parameters.candidate_file_content = lkh_candidate_file_content;
    auto tsp_output = travelingsalesmansolver::lkh(
            tsp_instance,
            tsp_parameters);
    //std::cout << tsp_output.solution.distance() << std::endl;
    if (lkh_candidate_file_content.empty())
        lkh_candidate_file_content = tsp_output.candidate_file_content;

    return {
        retrieve_solution(distances, instance, tsp_output.solution),
        retrieve_solution_2(distances, instance, tsp_output.solution)};
}

template <typename Distances>
travellingthiefsolver::packing_while_travelling::Instance create_pwt_instance(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred)
{
    std::vector<CityId> cities_tsp2pwt(instance.number_of_cities(), -1);
    for (CityId pwt_city_id = 0;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id = solution_pred.city_id(pwt_city_id);
        cities_tsp2pwt[city_id] = pwt_city_id;
    }
    packing_while_travelling::InstanceBuilder pwt_instance_builder;
    pwt_instance_builder.add_cities(instance.number_of_cities());
    pwt_instance_builder.set_capacity(instance.capacity());
    pwt_instance_builder.set_minimum_speed(instance.minimum_speed());
    pwt_instance_builder.set_maximum_speed(instance.maximum_speed());
    pwt_instance_builder.set_renting_ratio(instance.renting_ratio());
    // Set distances.
    for (CityId pwt_city_id = 0;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id_1 = (pwt_city_id == 0)?
            solution_pred.city_id(instance.number_of_cities() - 1):
            solution_pred.city_id(pwt_city_id - 1);
        CityId city_id_2 = solution_pred.city_id(pwt_city_id);
        pwt_instance_builder.set_distance(
                pwt_city_id,
                distances.distance(city_id_1, city_id_2));
    }
    // Add items.
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        CityId pwt_city_id = cities_tsp2pwt[item.city_id];
        pwt_instance_builder.add_item(
                pwt_city_id,
                item.weight,
                item.profit);
    }
    return pwt_instance_builder.build();
}

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred,
        const packing_while_travelling::Solution& pwt_solution)
{
    Solution solution(instance);
    for (ItemId item_id: instance.city(0).item_ids) {
        if (pwt_solution.contains(item_id))
            solution.add_item(distances, item_id);
    }
    for (CityId pwt_city_id = 1;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id = solution_pred.city_id(pwt_city_id);
        //std::cout << "pwt_city_id " << pwt_city_id
        //    << " city_id " << city_id
        //    << std::endl;
        solution.add_city(distances, city_id);
        for (ItemId item_id: instance.city(city_id).item_ids) {
            if (pwt_solution.contains(item_id))
                solution.add_item(distances, item_id);
        }
    }
    return solution;
}

template <typename Distances>
travellingthiefsolver::travelling_while_packing::Solution create_twp_solution(
        const Distances& distances,
        const travelling_while_packing::Instance& twp_instance,
        const Solution& solution)
{
    travelling_while_packing::Solution twp_solution(twp_instance);
    for (CityPos city_pos = 1;
            city_pos < twp_instance.number_of_cities();
            ++city_pos) {
        CityId city_id = solution.city_id(city_pos);
        twp_solution.add_city(distances, city_id);
    }
    return twp_solution;
}

template <typename Distances>
Solution retrieve_solution(
        const Distances& distances,
        const Instance& instance,
        const Solution& solution_pred,
        const travelling_while_packing::Solution& twp_solution)
{
    Solution solution(instance);
    for (ItemId item_id: instance.city(0).item_ids) {
        if (solution_pred.contains(item_id))
            solution.add_item(distances, item_id);
    }
    for (CityId pos = 1;
            pos < instance.number_of_cities();
            ++pos) {
        CityId city_id = twp_solution.city_id(pos);
        solution.add_city(distances, city_id);
        for (ItemId item_id: instance.city(city_id).item_ids) {
            if (solution_pred.contains(item_id))
                solution.add_item(distances, item_id);
        }
    }
    //std::cout << "solution feasible " << solution.feasible()
    //    << " renting_cost " << solution.renting_cost()
    //    << " cost " << solution.objective()
    //    << std::endl;
    return solution;
}

}
}
