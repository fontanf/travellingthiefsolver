#pragma once

#include "travellingthiefsolver/solution.hpp"

#include "packingwhiletravellingsolver/solution.hpp"
#include "packingwhiletravellingsolver/instance_builder.hpp"
#include "travellingwhilepackingsolver/solution.hpp"
#include "travelingsalesmansolver/solution.hpp"

namespace travellingthiefsolver
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

Solution retrieve_solution(
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution);

Solution retrieve_solution_2(
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution);

std::vector<Solution> solve_tsp_lkh(
        const Instance& instance,
        const travelingsalesmansolver::Instance& tsp_instance,
        std::mt19937_64& generator,
        std::string& lkh_candidate_file_content);

/*
 * PWT
 */

packingwhiletravellingsolver::Instance create_pwt_instance(
        const Instance& instance,
        const Solution& solution_pred);

Solution retrieve_solution(
        const Instance& instance,
        const Solution& solution_pred,
        const packingwhiletravellingsolver::Solution& pwt_solution);

/*
 * TWP
 */

travellingwhilepackingsolver::Instance create_twp_instance(
        const Instance& instance,
        const Solution& solution_pred);

travellingwhilepackingsolver::Solution create_twp_solution(
        const travellingwhilepackingsolver::Instance& twp_instance,
        const Solution& solution);

Solution retrieve_solution(
        const Instance& instance,
        const Solution& solution_pred,
        const travellingwhilepackingsolver::Solution& twp_solution);

}
