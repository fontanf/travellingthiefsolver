#include "travellingthiefsolver/travelling_thief/algorithms/large_neighborhood_search.hpp"

#include "travellingthiefsolver/travelling_thief/utils.hpp"
#include "travellingthiefsolver/packing_while_travelling/utils.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/efficient_local_search.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

using namespace travellingthiefsolver::travelling_thief;

namespace
{

Solution large_neighborhood_search_initial_solution(
        const Instance& instance,
        std::mt19937_64& generator,
        const LargeNeighborhoodSearchParameters& parameters,
        std::string& lkh_candidate_file_content)
{
    auto tsp_instance = create_tsp_instance(instance);

    std::vector<Solution> svc_solutions;
    for (Counter tsp_iteration = 0;
            tsp_iteration < 2;
            ++tsp_iteration) {

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
                instance,
                tsp_instance,
                generator,
                parameters,
                lkh_candidate_file_content);

        for (Counter svc_iteration = 0;
                svc_iteration < 2;
                ++svc_iteration) {

            // Create packing_while_travelling instance.
            auto pwt_instance = create_pwt_instance(
                    instance,
                    tsp_solutions[svc_iteration]);

            travellingthiefsolver::packing_while_travelling::SequentialValueCorrectionParameters svc_parameters;
            svc_parameters.timer = parameters.timer;
            svc_parameters.verbosity_level = 0;
            auto svc_output = travellingthiefsolver::packing_while_travelling::sequential_value_correction(
                    pwt_instance,
                    svc_parameters);

            // Retrieve solution.
            Solution svc_solution = retrieve_solution(
                    instance,
                    tsp_solutions[svc_iteration],
                    svc_output.solution);

            svc_solutions.push_back(svc_solution);
        }
    }

    // Retrieve the best SVC solution.
    std::sort(
            svc_solutions.begin(),
            svc_solutions.end(),
            [](
                const Solution& solution_1,
                const Solution& solution_2) -> bool
            {
                return solution_1.objective() < solution_2.objective();
            });
    Solution svc_solution = svc_solutions.back();
    svc_solutions.pop_back();

    // Create packing_while_travelling instance.
    auto pwt_instance = create_pwt_instance(
            instance,
            svc_solution);

    travellingthiefsolver::packing_while_travelling::EfficientLocalSearchParameters pwtels_parameters;
    pwtels_parameters.info = optimizationtools::Info(parameters.info, false, "");
    auto pwtels_output = travellingthiefsolver::packing_while_travelling::efficient_local_search(
            pwt_instance,
            pwtels_parameters);

    // Retrieve solution.
    Solution pwtels_solution = retrieve_solution(
            instance,
            svc_solution,
            pwtels_output.solution);

    return pwtels_solution;
}

struct TwoOptMove
{
    CityId city_id_1;
    CityId city_id_2;
    Distance distance_delta;
};

}

Output travellingthiefsolver::travelling_thief::large_neighborhood_search(
        const Instance& instance,
        std::mt19937_64& generator,
        LargeNeighborhoodSearchParameters parameters)
{
    init_display(instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Large neighborhood search" << std::endl
        << std::endl;

    LargeNeighborhoodSearchOutput output(instance, parameters.info);

    auto city_states = packing_while_travelling::compute_city_states<Instance>(instance);
    std::string lkh_candidate_file_content = "";

    // Generate an initial solution.
    Solution solution = large_neighborhood_search_initial_solution(
            instance,
            generator,
            parameters,
            lkh_candidate_file_content);

    // Update output.
    algorithm_formatter.update_solution(solution, "initial solution");

    std::vector<travelingsalesmansolver::LkhCandidate> lkh_candidates
        = travelingsalesmansolver::read_candidates(lkh_candidate_file_content);

    for (output.number_of_iterations = 0;; ++output.number_of_iterations) {

        // Check end.
        if (parameters.info.needs_to_end())
            break;

        // Get the list of valid 2-opt moves.
        std::vector<TwoOptMove> two_opt_moves;
        for (CityId city_id_1 = 1;
                city_id_1 < instance.number_of_cities();
                ++city_id_1) {
            for (const auto& candidate: lkh_candidates[city_id_1].edges) {
                CityId city_id_2 = candidate.vertex_id;

                TwoOptMove move;
                move.city_id_1 = city_id_1;
                move.city_id_2 = city_id_2;

                CityPos city_pos_1 = solution.city(city_id_1).position;
                CityPos city_pos_2 = solution.city(city_id_2).position;
                if (city_pos_1 > city_pos_2)
                    continue;

                // We don't consider the case where city_id_2 is visited right
                // after city_id_1.
                if (city_pos_2 == city_pos_1 + 1)
                    continue;

                CityId city_id_a = solution.city_id(city_pos_1 + 1);
                CityId city_id_b = (city_pos_2 == instance.number_of_cities() - 1)?
                        0:
                        solution.city_id(city_pos_2 + 1);

                move.distance_delta
                    = instance.distance(city_id_1, city_id_2)
                    + instance.distance(city_id_a, city_id_b)
                    - instance.distance(city_id_1, city_id_a)
                    - instance.distance(city_id_2, city_id_b);

                two_opt_moves.push_back(move);
            }
        }

        // Sort 2-opt moves.
        std::shuffle(
                two_opt_moves.begin(),
                two_opt_moves.end(),
                generator);
        //std::sort(
        //        two_opt_moves.begin(),
        //        two_opt_moves.end(),
        //        [](
        //            const TwoOptMove& two_opt_move_1,
        //            const TwoOptMove& two_opt_move_2) -> bool
        //        {
        //            return two_opt_move_1.distance_delta
        //                < two_opt_move_2.distance_delta;
        //        });

        // Loop through 2-opt moves.
        bool improved = false;
        for (const TwoOptMove& two_opt_move: two_opt_moves) {

            // Check end.
            if (parameters.info.needs_to_end())
                break;

            CityPos city_pos_1 = solution.city(two_opt_move.city_id_1).position;
            CityPos city_pos_2 = solution.city(two_opt_move.city_id_2).position;
            std::cout << "city_id_1 " << two_opt_move.city_id_1
                << " city_id_2 " << two_opt_move.city_id_2
                << " city_pos_1 " << city_pos_1
                << " city_pos_2 " << city_pos_2
                << " delta " << two_opt_move.distance_delta
                << std::endl;

            // Build new solution.
            Solution solution_two_opt(instance);
            for (CityPos city_pos = 1;
                    city_pos <= city_pos_1;
                    ++city_pos) {
                CityId city_id = solution.city_id(city_pos);
                solution_two_opt.add_city(city_id);
                for (ItemId item_id: instance.city(city_id).item_ids)
                    if (solution.contains(item_id))
                        solution_two_opt.add_item(item_id);
            }
            for (CityPos city_pos = city_pos_2;
                    city_pos > city_pos_1;
                    --city_pos) {
                CityId city_id = solution.city_id(city_pos);
                solution_two_opt.add_city(city_id);
                for (ItemId item_id: instance.city(city_id).item_ids)
                    if (solution.contains(item_id))
                        solution_two_opt.add_item(item_id);
            }
            for (CityPos city_pos = city_pos_2 + 1;
                    city_pos < instance.number_of_cities();
                    ++city_pos) {
                CityId city_id = solution.city_id(city_pos);
                solution_two_opt.add_city(city_id);
                for (ItemId item_id: instance.city(city_id).item_ids)
                    if (solution.contains(item_id))
                        solution_two_opt.add_item(item_id);
            }
            std::cout << "solution_two_opt.objective() "
                << solution_two_opt.objective()
                << std::endl;

            // Build PWT instance.
            auto pwt_instance = create_pwt_instance(
                    instance,
                    solution_two_opt);

            // Build PWT intial solution.
            std::vector<uint8_t> pwt_initial_items(instance.number_of_items());
            for (ItemId item_id = 0;
                    item_id < instance.number_of_items();
                    ++item_id) {
                pwt_initial_items[item_id] = solution.contains(item_id);
            }
            packing_while_travelling::Solution pwt_initial_solution(
                    pwt_instance,
                    pwt_initial_items);

            // Solve PWT instance.
            packing_while_travelling::EfficientLocalSearchParameters pwtels_parameters;
            pwtels_parameters.info = optimizationtools::Info(parameters.info, false, "");
            pwtels_parameters.initial_solution = &pwt_initial_solution;
            pwtels_parameters.minimum_improvement = 0;
            auto pwt_output = packing_while_travelling::efficient_local_search(
                    pwt_instance,
                    pwtels_parameters);

            // Retrieve TTP solution.
            Solution solution_new = retrieve_solution(
                    instance,
                    solution_two_opt,
                    pwt_output.solution);
            std::cout << "solution_new.objective() "
                << solution_new.objective()
                << std::endl;
            std::cout << "distance " << solution.distance()
                << " " << two_opt_move.distance_delta
                << " " << solution_new.distance()
                << std::endl;

            // Check if the new solution is better.
            if (solution.objective() < solution_new.objective()) {
                solution = solution_new;
                improved = true;

                // Update output.
                std::stringstream ss;
                ss << "iteration " << output.number_of_iterations;
                algorithm_formatter.update_solution(solution, ss.str());

                break;
            }
        }

        if (!improved)
            break;
    }

    algorithm_formatter.end();
    return output;
}
