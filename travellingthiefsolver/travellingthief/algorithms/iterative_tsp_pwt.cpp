#include "travellingthiefsolver/travellingthief/algorithms/iterative_tsp_pwt.hpp"

#include "travellingthiefsolver/travellingthief/algorithm_formatter.hpp"
#include "travellingthiefsolver/travellingthief/utils.hpp"
#include "travellingthiefsolver/packingwhiletravelling/instance_builder.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/efficient_local_search.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

using namespace travellingthiefsolver::travellingthief;

const IterativeTspPwtOutput travellingthiefsolver::travellingthief::iterative_tsp_pwt(
        const Instance& instance,
        std::mt19937_64& generator,
        const IterativeTspPwtParameters& parameters)
{
    IterativeTspPwtOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Iterative TSP PWT");
    algorithm_formatter.print_header();

    // Create travelingsalesman instance.
    auto tsp_instance = create_tsp_instance(instance);

    std::string lkh_candidate_file_content = "";
    std::vector<travelingsalesmansolver::LkhCandidate> lkh_candidates;

    std::vector<Solution> svc_solutions;

    for (Counter i = 0; i < 5; ++i) {

        auto tsp_begin = std::chrono::steady_clock::now();

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
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

            // Create packingwhiletravelling instance.
            auto pwt_instance = create_pwt_instance(
                    instance,
                    tsp_solution);

            packingwhiletravelling::SequentialValueCorrectionParameters svc_parameters;
            svc_parameters.verbosity_level = 0;
            auto svc_output = packingwhiletravelling::sequential_value_correction(
                    pwt_instance,
                    svc_parameters);

            // Retrieve solution.
            Solution svc_solution = retrieve_solution(
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
                return solution_1.objective_value() > solution_2.objective_value();
            });
    while (svc_solutions.size() > 3)
        svc_solutions.pop_back();

    auto pwt_begin = std::chrono::steady_clock::now();

    for (const Solution& svc_solution: svc_solutions) {

        // Create packingwhiletravelling instance.
        auto pwt_instance = create_pwt_instance(
                instance,
                svc_solution);

        packingwhiletravelling::EfficientLocalSearchParameters els_parameters;
        auto els_output = packingwhiletravelling::efficient_local_search(
                pwt_instance,
                els_parameters);

        // Retrieve solution.
        Solution els_solution = retrieve_solution(
                instance,
                svc_solution,
                els_output.solution);

        output.number_of_els_calls++;

        // Update output.
        algorithm_formatter.update_solution(els_solution, "initial els");

    }

    auto pwt_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> pwt_time_span
        = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
    output.pwt_time += pwt_time_span.count();


    for (Counter number_of_iterations = 0;
            !parameters.timer.needs_to_end();
            ++number_of_iterations) {

        auto tsp_begin = std::chrono::steady_clock::now();

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
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

            // Create packingwhiletravelling instance.
            auto pwt_instance = create_pwt_instance(
                    instance,
                    tsp_solution);

            packingwhiletravelling::SequentialValueCorrectionParameters svc_parameters;
            svc_parameters.verbosity_level = 0;
            auto svc_output = packingwhiletravelling::sequential_value_correction(
                    pwt_instance,
                    svc_parameters);

            // Retrieve solution.
            Solution svc_solution = retrieve_solution(
                    instance,
                    tsp_solution,
                    svc_output.solution);

            output.number_of_svc_calls++;

            // Update output.
            std::stringstream ss;
            ss << "iteration " << number_of_iterations << " (svc)";
            algorithm_formatter.update_solution(svc_solution, ss.str());

            if (svc_solution.objective_value()
                    > svc_solutions.back().objective_value()) {

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
                while (svc_solutions.size() > 3)
                    svc_solutions.pop_back();

                packingwhiletravelling::EfficientLocalSearchParameters els_parameters;
                auto els_output = packingwhiletravelling::efficient_local_search(
                        pwt_instance,
                        els_parameters);

                // Retrieve solution.
                Solution els_solution = retrieve_solution(
                        instance,
                        tsp_solution,
                        els_output.solution);

                output.number_of_els_calls++;

                // Update output.
                std::stringstream ss;
                ss << "iteration " << number_of_iterations << " (els)";
                algorithm_formatter.update_solution(els_solution, ss.str());

            }
        }

        auto pwt_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> pwt_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
        output.pwt_time += pwt_time_span.count();

    }

    algorithm_formatter.end();
    return output;
}
