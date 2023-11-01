#include "travellingthiefsolver/packingwhiletravelling/algorithms/efficient_local_search.hpp"

#include "travellingthiefsolver/packingwhiletravelling/utils.hpp"

#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"

using namespace travellingthiefsolver::packingwhiletravelling;

void EfficientLocalSearchOutput::print_statistics(
        optimizationtools::Info& info) const
{
    if (info.verbosity_level() >= 1) {
        info.os()
            << "Number of iterations:         " << number_of_iterations << std::endl
            ;
    }
    info.add_to_json("Algorithm", "NumberOfIterations", number_of_iterations);
}

namespace
{

/**
 * Structure for a solution of the efficient local search algorithm.
 */
struct EfficientLocalSearchSolution
{
    /** States of each city. */
    std::vector<CityStateId> city_states;

    /** Profit of the solution. */
    Profit profit = 0;

    /** Weight of the solution. */
    Weight weight = 0;

    /** Travel time of the solution. */
    Time time = 0;

    /** Cumulative weight for each city. */
    std::vector<Weight> cumulative_weights;

    /** Cumulative profit for each city. */
    std::vector<Profit> cumulative_profits;

    /** Cumulative time for each city. */
    std::vector<Time> cumulative_times;

    /** Objective value of the solution. */
    Profit objective;
};

EfficientLocalSearchSolution init_solution(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        const std::vector<CityStateId>& solution_city_states)
{
    EfficientLocalSearchSolution solution;
    solution.city_states = solution_city_states;
    CityStateId city_state_id = solution_city_states[0];
    const CityState& city_state = city_states[0][city_state_id];
    solution.profit = city_state.total_profit;
    solution.weight = instance.city(0).weight + city_state.total_weight;
    solution.cumulative_weights = std::vector<Weight> (instance.number_of_cities(), 0);
    solution.cumulative_profits = std::vector<Profit>(instance.number_of_cities(), 0);
    solution.cumulative_times = std::vector<Time>(instance.number_of_cities(), 0);
    solution.time = 0;
    for (CityId city_id = 1;
            city_id < instance.number_of_cities();
            ++city_id) {
        CityStateId city_state_id = solution_city_states[city_id];
        const CityState& city_state = city_states[city_id][city_state_id];
        solution.time += instance.duration(city_id, solution.weight);
        solution.weight += instance.city(city_id).weight
            + city_state.total_weight;
        solution.profit += city_state.total_profit;
        solution.cumulative_weights[city_id] = solution.weight;
        solution.cumulative_times[city_id] = solution.time;
    }
    solution.time += instance.duration(0, solution.weight);
    solution.objective = solution.profit
        - instance.renting_ratio() * solution.time;
    return solution;
}

Profit evaluate_move(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        const EfficientLocalSearchSolution& solution,
        CityId city_id,
        CityStateId city_state_id)
{
    Profit profit_new = solution.profit
        - city_states[city_id][solution.city_states[city_id]].total_profit
        + city_states[city_id][city_state_id].total_profit;
    Weight weight_new = solution.weight
        - city_states[city_id][solution.city_states[city_id]].total_weight
        + city_states[city_id][city_state_id].total_weight;
    if (weight_new > instance.capacity())
        return -std::numeric_limits<Profit>::infinity();
    Weight weight_diff =
        - city_states[city_id][solution.city_states[city_id]].total_weight
        + city_states[city_id][city_state_id].total_weight;
    Time time_cur = 0;

    if (city_state_id > solution.city_states[city_id]) {
        time_cur += instance.duration(0, solution.weight + weight_diff);
        for (CityId city_id_2 = instance.number_of_cities() - 1;
                city_id_2 > city_id;
                --city_id_2) {
            Weight weight_cur = solution.cumulative_weights[city_id_2 - 1] + weight_diff;
            time_cur += instance.duration(city_id_2, weight_cur);
            Profit bound = profit_new
                - instance.renting_ratio()
                * (solution.cumulative_times[city_id_2 - 1] + time_cur);
            if (bound < solution.objective)
                return -std::numeric_limits<Profit>::infinity();
        }
        time_cur += solution.cumulative_times[city_id];
    } else {
        time_cur += instance.duration(0, solution.weight + weight_diff);
        for (CityId city_id_2 = instance.number_of_cities() - 1;
                city_id_2 > city_id;
                --city_id_2) {
            Weight weight_cur = solution.cumulative_weights[city_id_2 - 1] + weight_diff;
            time_cur += instance.duration(city_id_2, weight_cur);
            double speed_cur = instance.speed(solution.cumulative_weights[city_id_2 - 1]);
            double speed_new = instance.speed(weight_cur);
            double unitary_spared_time = 1.0 / speed_cur - 1.0 / speed_new;
            Profit bound = profit_new
                - instance.renting_ratio()
                * (solution.cumulative_times[city_id_2 - 1]
                        + time_cur
                        - unitary_spared_time
                        * (instance.city(city_id_2 - 1).distance_from_start
                            - instance.city(city_id).distance_from_start));
            if (bound < solution.objective)
                return -std::numeric_limits<Profit>::infinity();
        }
        time_cur += solution.cumulative_times[city_id];
    }

    return profit_new - instance.renting_ratio() * time_cur;
}

void apply_move(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        EfficientLocalSearchSolution& solution,
        CityId city_id,
        CityStateId city_state_id)
{
    solution.weight = solution.cumulative_weights[city_id - 1];
    solution.profit = solution.cumulative_profits[city_id - 1];
    solution.time = solution.cumulative_times[city_id];

    solution.weight += instance.city(city_id).weight
        + city_states[city_id][city_state_id].total_weight;
    solution.profit += city_states[city_id][city_state_id].total_profit;
    solution.cumulative_weights[city_id] = solution.weight;
    solution.cumulative_profits[city_id] = solution.profit;
    solution.cumulative_times[city_id] = solution.time;

    for (CityId city_id_2 = city_id + 1;
            city_id_2 < instance.number_of_cities();
            ++city_id_2) {
        CityStateId city_state_id = solution.city_states[city_id_2];
        solution.time += instance.duration(city_id_2, solution.weight);
        solution.weight += instance.city(city_id_2).weight
            + city_states[city_id_2][city_state_id].total_weight;
        solution.profit += city_states[city_id_2][city_state_id].total_profit;
        solution.cumulative_weights[city_id_2] = solution.weight;
        solution.cumulative_profits[city_id_2] = solution.profit;
        solution.cumulative_times[city_id_2] = solution.time;
    }
    solution.time += instance.duration(0, solution.weight);
    solution.objective = solution.profit
        - instance.renting_ratio() * solution.time;

    //std::cout << "solution" << std::endl;
    //std::cout << "profit " << profit_new << " " << solution.profit << std::endl;
    //std::cout << "time " << time_cur << " " << solution.time << std::endl;
    //std::cout << "objective " << objective_new << " " << solution.objective << std::endl;

    //std::cout << "city_id " << city_id
    //    << " " << solution.city_states[city_id]
    //    << " -> " << city_state_id
    //    << " wei " << solution.weight << " / " << instance.capacity()
    //    << " pro " << solution.profit
    //    << " time " << solution.time
    //    << " obj " << solution.objective
    //    << std::endl;

    solution.city_states[city_id] = city_state_id;
}

Solution efficient_local_search_initial_solution(
        const Instance& instance,
        EfficientLocalSearchOptionalParameters parameters)
{
    if (parameters.initial_solution != nullptr) {
        std::vector<uint8_t> items(instance.number_of_items());
        for (ItemId item_id = 0;
                item_id < instance.number_of_items();
                ++item_id) {
            ItemId orig_item_id = instance.unreduction_info().unreduction_operations[item_id];
            if (parameters.initial_solution->contains(orig_item_id))
                items[item_id] = 1;
        }
        return Solution(instance, items);
    } else {
        auto svc_output = travellingthiefsolver::packingwhiletravelling::sequential_value_correction(
                instance);
        return svc_output.solution;
    }
}

}

EfficientLocalSearchOutput travellingthiefsolver::packingwhiletravelling::efficient_local_search(
        const Instance& original_instance,
        EfficientLocalSearchOptionalParameters parameters)
{
    init_display(original_instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Efficient local search" << std::endl
        << std::endl;

    // Reduction.
    std::unique_ptr<Instance> reduced_instance = nullptr;
    if (parameters.reduction_parameters.reduce) {
        reduced_instance = std::unique_ptr<Instance>(
                new Instance(
                    original_instance.reduce(
                        parameters.reduction_parameters)));
        parameters.info.os()
            << "Reduced instance" << std::endl
            << "----------------" << std::endl;
        reduced_instance->print(parameters.info.os(), parameters.info.verbosity_level());
        parameters.info.os() << std::endl;
    }
    const Instance& instance = (reduced_instance == nullptr)? original_instance: *reduced_instance;

    EfficientLocalSearchOutput output(original_instance, parameters.info);

    Solution initial_solution = efficient_local_search_initial_solution(instance, parameters);

    auto city_states = packingwhiletravelling::compute_city_states<Instance>(instance);

    EfficientLocalSearchSolution solution = init_solution(
            instance,
            city_states,
            solution2states(instance, city_states, initial_solution));

    for (output.number_of_iterations = 0;
            !parameters.info.needs_to_end();
            ++output.number_of_iterations) {
        Profit objective_cur = solution.objective;

        //std::cout << "number_of_iterations " << output.number_of_iterations << std::endl;

        Counter number_of_improvements = 0;
        for (CityId city_id = instance.number_of_cities() - 1;
                city_id > 0;
                --city_id) {
            //std::cout << "city_id " << city_id << std::endl;
            bool city_improved = false;

            for (CityStateId city_state_offset = 0;
                    city_state_offset < (CityStateId)city_states[city_id].size();
                    ++city_state_offset) {
                CityStateId city_state_id
                    = (solution.city_states[city_id] + city_state_offset)
                    % (CityStateId)city_states[city_id].size();
                if (city_state_id == solution.city_states[city_id])
                    continue;

                Profit objective_new = evaluate_move(
                        instance,
                        city_states,
                        solution,
                        city_id,
                        city_state_id);

                if (solution.objective < objective_new) {
                    // Update current solution.
                    apply_move(
                            instance,
                            city_states,
                            solution,
                            city_id,
                            city_state_id);

                    if (!city_improved) {
                        city_improved = true;
                        number_of_improvements++;
                    }
                }
            }

        }

        //std::cout << number_of_improvements << std::endl;
        //std::cout << solution_objective << std::endl;
        if (number_of_improvements == 0)
            break;
        if (objective_cur > 0) {
            double improvement = (solution.objective - objective_cur) / objective_cur;
            //std::cout
            //    << "objective_cur " << objective_cur
            //    << " objective_new " << solution.objective
            //    << " improvement " << improvement << std::endl;
            if (improvement < parameters.minimum_improvement)
                break;
        }
    }

    std::vector<uint8_t> items(instance.number_of_items());
    for (CityId city_id = 0;
            city_id < instance.number_of_cities();
            ++city_id) {
        CityStateId city_state_id = solution.city_states[city_id];
        for (ItemId item_id: city_states[city_id][city_state_id].item_ids)
            items[item_id] = 1;
    }

    // Update output.
    std::stringstream ss;
    output.update_solution(Solution(instance, items), ss, parameters.info);

    output.algorithm_end(parameters.info);
    return output;
}
