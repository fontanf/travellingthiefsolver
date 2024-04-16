#pragma once

#include "travellingthiefsolver/travelling_thief/solution.hpp"

#include "travellingthiefsolver/travelling_thief/algorithm_formatter.hpp"
#include "travellingthiefsolver/travelling_thief/utils.hpp"
#include "travellingthiefsolver/travelling_thief/algorithms/efficient_local_search.hpp"
#include "travellingthiefsolver/packing_while_travelling/instance_builder.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packing_while_travelling/algorithms/efficient_local_search.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

namespace travellingthiefsolver
{
namespace travelling_thief
{

struct IterativeTspPwtTtpParameters: Parameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;
};

struct IterativeTspPwtTtpOutput: Output
{
    IterativeTspPwtTtpOutput(
            const Instance& instance):
        Output(instance) { }


    /** Number of SVC calls. */
    Counter number_of_svc_calls = 0;

    /** Number of efficient local search calls. */
    Counter number_of_els_calls = 0;

    /** Time spent in TSP. */
    double tsp_time = 0.0;

    /** Time spent in PWT. */
    double pwt_time = 0.0;

    /** Time spent in TTP. */
    double ttp_time = 0.0;
};

const IterativeTspPwtTtpOutput iterative_tsp_pwt_ttp(
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtTtpParameters& parameters = {});

template <typename Distances>
const IterativeTspPwtTtpOutput iterative_tsp_pwt_ttp(
        const Distances& distances,
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtTtpParameters& parameters = {});

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Distances>
const IterativeTspPwtTtpOutput iterative_tsp_pwt_ttp(
        const Distances& distances,
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtTtpParameters& parameters)
{
    IterativeTspPwtTtpOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Iterative TSP PWT TTP");
    algorithm_formatter.print_header();

    // Create travelingsalesman instance.
    auto tsp_instance = create_tsp_instance(instance);

    std::string lkh_candidate_file_content = "";
    std::vector<travelingsalesmansolver::LkhCandidate> lkh_candidates;

    std::vector<Solution> svc_solutions;
    Counter svc_solutions_size = 10;

    for (Counter i = 0; i < 5; ++i) {

        auto tsp_begin = std::chrono::steady_clock::now();

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
                distances,
                instance,
                tsp_instance,
                generator,
                parameters,
                lkh_candidate_file_content);

        auto tsp_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> tsp_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(tsp_end - tsp_begin);
        output.tsp_time += tsp_time_span.count();

        auto pwt_begin = std::chrono::steady_clock::now();

        for (const Solution& tsp_solution: tsp_solutions) {

            // Create packing_while_travelling instance.
            auto pwt_instance = create_pwt_instance(
                    distances,
                    instance,
                    tsp_solution);

            packing_while_travelling::SequentialValueCorrectionParameters svc_parameters;
            svc_parameters.timer = parameters.timer;
            svc_parameters.verbosity_level = 0;
            auto svc_output = packing_while_travelling::sequential_value_correction(
                    pwt_instance,
                    svc_parameters);

            // Retrieve solution.
            Solution svc_solution = retrieve_solution(
                    distances,
                    instance,
                    tsp_solution,
                    svc_output.solution);

            output.number_of_svc_calls++;

            // Update output.
            std::stringstream ss;
            ss << "initial svc " << i;
            algorithm_formatter.update_solution(svc_solution, ss.str());

            svc_solutions.push_back(svc_solution);
        }

        auto pwt_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> pwt_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
        output.pwt_time += pwt_time_span.count();

    }

    std::sort(
            svc_solutions.begin(),
            svc_solutions.end(),
            [](
                const Solution& solution_1,
                const Solution& solution_2) -> bool
            {
            return solution_1.objective_value()
            > solution_2.objective_value();
            });
    while ((Counter)svc_solutions.size() > svc_solutions_size)
        svc_solutions.pop_back();

    for (const Solution& svc_solution: svc_solutions) {

        auto pwt_begin = std::chrono::steady_clock::now();

        // Create packing_while_travelling instance.
        auto pwt_instance = create_pwt_instance(
                distances,
                instance,
                svc_solution);

        packing_while_travelling::EfficientLocalSearchParameters els_parameters;
        els_parameters.timer = parameters.timer;
        els_parameters.verbosity_level = 0;
        auto els_output = packing_while_travelling::efficient_local_search(
                pwt_instance,
                els_parameters);

        // Retrieve solution.
        Solution els_solution = retrieve_solution(
                distances,
                instance,
                svc_solution,
                els_output.solution);
        //std::cout << "pwtels " << els_solution.objective() << std::endl;

        output.number_of_els_calls++;

        // Update output.
        algorithm_formatter.update_solution(els_solution, "initial els");

        auto pwt_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> pwt_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
        output.pwt_time += pwt_time_span.count();

        auto ttp_begin = std::chrono::steady_clock::now();

        // Run TTP-ELS.
        EfficientLocalSearchParameters ttpels_parameters;
        ttpels_parameters.timer = parameters.timer;
        ttpels_parameters.verbosity_level = 0;
        ttpels_parameters.initial_solution = &els_solution;
        ttpels_parameters.lkh_candidate_file_content = lkh_candidate_file_content;
        auto ttpels_output = efficient_local_search(
                instance,
                generator,
                ttpels_parameters);
        //std::cout << "ttpels " << ttpels_output.solution.objective() << std::endl;
        // Update output.
        {
            algorithm_formatter.update_solution(ttpels_output.solution, "initial ttpels");
        }

        auto ttp_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> ttp_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(ttp_end - ttp_begin);
        output.ttp_time += ttp_time_span.count();

    }

    for (Counter number_of_iterations = 0;
            !parameters.timer.needs_to_end();
            ++number_of_iterations) {

        auto tsp_begin = std::chrono::steady_clock::now();

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
                distances,
                instance,
                tsp_instance,
                generator,
                parameters,
                lkh_candidate_file_content);

        auto tsp_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> tsp_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(tsp_end - tsp_begin);
        output.tsp_time += tsp_time_span.count();

        for (const Solution& tsp_solution: tsp_solutions) {

            auto pwt_begin = std::chrono::steady_clock::now();

            // Create packing_while_travelling instance.
            auto pwt_instance = create_pwt_instance(
                    distances,
                    instance,
                    tsp_solution);

            packing_while_travelling::SequentialValueCorrectionParameters svc_parameters;
            svc_parameters.timer = parameters.timer;
            svc_parameters.verbosity_level = 0;
            auto svc_output = packing_while_travelling::sequential_value_correction(
                    pwt_instance,
                    svc_parameters);

            // Retrieve solution.
            Solution svc_solution = retrieve_solution(
                    distances,
                    instance,
                    tsp_solution,
                    svc_output.solution);

            output.number_of_svc_calls++;

            // Update output.
            std::stringstream ss;
            ss << "iteration " << number_of_iterations << " (svc)";
            algorithm_formatter.update_solution(svc_solution, ss.str());

            auto pwt_end = std::chrono::steady_clock::now();
            std::chrono::duration<double> pwt_time_span
                = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
            output.pwt_time += pwt_time_span.count();

            if (svc_solution.objective_value()
                    > svc_solutions.back().objective_value()) {

                auto pwt_begin = std::chrono::steady_clock::now();

                svc_solutions.push_back(svc_solution);
                std::sort(
                        svc_solutions.begin(),
                        svc_solutions.end(),
                        [](
                            const Solution& solution_1,
                            const Solution& solution_2) -> bool
                        {
                        return solution_1.objective_value()
                        > solution_2.objective_value();
                        });
                while ((Counter)svc_solutions.size() > svc_solutions_size)
                    svc_solutions.pop_back();

                packing_while_travelling::EfficientLocalSearchParameters els_parameters;
                els_parameters.timer = parameters.timer;
                els_parameters.verbosity_level = 0;
                auto els_output = packing_while_travelling::efficient_local_search(
                        pwt_instance,
                        els_parameters);

                // Retrieve solution.
                Solution els_solution = retrieve_solution(
                        distances,
                        instance,
                        tsp_solution,
                        els_output.solution);
                //std::cout << "pwtels " << els_solution.objective() << std::endl;

                output.number_of_els_calls++;

                // Update output.
                std::stringstream ss;
                ss << "iteration " << number_of_iterations << " (els)";
                algorithm_formatter.update_solution(els_solution, ss.str());

                auto pwt_end = std::chrono::steady_clock::now();
                std::chrono::duration<double> pwt_time_span
                    = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
                output.pwt_time += pwt_time_span.count();

                auto ttp_begin = std::chrono::steady_clock::now();

                // Run TTP-ELS.
                EfficientLocalSearchParameters ttpels_parameters;
                ttpels_parameters.timer = parameters.timer;
                ttpels_parameters.verbosity_level = 0;
                ttpels_parameters.initial_solution = &els_solution;
                ttpels_parameters.lkh_candidate_file_content = lkh_candidate_file_content;
                auto ttpels_output = efficient_local_search(
                        instance,
                        generator,
                        ttpels_parameters);
                //std::cout << "ttpels " << ttpels_output.solution.objective() << std::endl;
                // Update output.
                {
                    std::stringstream ss;
                    ss << "iteration " << number_of_iterations << " (ttpels)";
                    algorithm_formatter.update_solution(ttpels_output.solution, ss.str());
                }

                auto ttp_end = std::chrono::steady_clock::now();
                std::chrono::duration<double> ttp_time_span
                    = std::chrono::duration_cast<std::chrono::duration<double>>(ttp_end - ttp_begin);
                output.ttp_time += ttp_time_span.count();

            }
        }

    }

    algorithm_formatter.end();
    return output;
}

}
}
