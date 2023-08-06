#include "travellingthiefsolver/algorithms/iterative_tsp_pwt_ttp.hpp"

#include "travellingthiefsolver/utils.hpp"
#include "travellingthiefsolver/algorithms/efficient_local_search.hpp"
#include "packingwhiletravellingsolver/instance_builder.hpp"
#include "packingwhiletravellingsolver/algorithms/sequential_value_correction.hpp"
#include "packingwhiletravellingsolver/algorithms/efficient_local_search.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

using namespace travellingthiefsolver;

void IterativeTspPwtTtpOutput::print_statistics(
        optimizationtools::Info& info) const
{
    if (info.verbosity_level() >= 1) {
        info.os()
            << "Number of SVC calls:          " << number_of_svc_calls << std::endl
            << "Number of ELS calls:          " << number_of_els_calls << std::endl
            << "TSP time:                     " << tsp_time << std::endl
            << "PWT time:                     " << pwt_time << std::endl
            << "TTP time:                     " << ttp_time << std::endl
            ;
    }
    info.add_to_json("Algorithm", "NumberOfSvcCalls", number_of_svc_calls);
    info.add_to_json("Algorithm", "NumberOfElsCalls", number_of_els_calls);
    info.add_to_json("Algorithm", "TspTime", tsp_time);
    info.add_to_json("Algorithm", "PwtTime", pwt_time);
    info.add_to_json("Algorithm", "TtpTime", ttp_time);
}

IterativeTspPwtTtpOutput travellingthiefsolver::iterative_tsp_pwt_ttp(
        const Instance& instance,
        std::mt19937_64& generator,
        IterativeTspPwtTtpOptionalParameters parameters)
{
    init_display(instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Iterative TSP PWT TTP" << std::endl
        << std::endl;

    IterativeTspPwtTtpOutput output(instance, parameters.info);

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
                instance,
                tsp_instance,
                generator,
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

            packingwhiletravellingsolver::SequentialValueCorrectionOptionalParameters svc_parameters;
            //pwt_parameters.parameters.info.set_verbosity_level(1);
            auto svc_output = packingwhiletravellingsolver::sequential_value_correction(
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
            output.update_solution(svc_solution, ss, parameters.info);

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
                return solution_1.objective() > solution_2.objective();
            });
    while ((Counter)svc_solutions.size() > svc_solutions_size)
        svc_solutions.pop_back();

    for (const Solution& svc_solution: svc_solutions) {

        auto pwt_begin = std::chrono::steady_clock::now();

        // Create packingwhiletravelling instance.
        auto pwt_instance = create_pwt_instance(
                instance,
                svc_solution);

        packingwhiletravellingsolver::EfficientLocalSearchOptionalParameters els_parameters;
        auto els_output = packingwhiletravellingsolver::efficient_local_search(
                pwt_instance,
                els_parameters);

        // Retrieve solution.
        Solution els_solution = retrieve_solution(
                instance,
                svc_solution,
                els_output.solution);
        //std::cout << "pwtels " << els_solution.objective() << std::endl;

        output.number_of_els_calls++;

        // Update output.
        std::stringstream ss;
        ss << "initial els";
        output.update_solution(els_solution, ss, parameters.info);

        auto pwt_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> pwt_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
        output.pwt_time += pwt_time_span.count();

        auto ttp_begin = std::chrono::steady_clock::now();

        // Run TTP-ELS.
        EfficientLocalSearchOptionalParameters ttpels_parameters;
        ttpels_parameters.initial_solution = &els_solution;
        ttpels_parameters.lkh_candidate_file_content = lkh_candidate_file_content;
        //ttpels_parameters.info.set_verbosity_level(1);
        auto ttpels_output = efficient_local_search(
                instance,
                generator,
                ttpels_parameters);
        //std::cout << "ttpels " << ttpels_output.solution.objective() << std::endl;
        // Update output.
        {
            std::stringstream ss;
            ss << "initial ttpels";
            output.update_solution(ttpels_output.solution, ss, parameters.info);
        }

        auto ttp_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> ttp_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(ttp_end - ttp_begin);
        output.ttp_time += ttp_time_span.count();

    }

    for (Counter number_of_iterations = 0;
            !parameters.info.needs_to_end();
            ++number_of_iterations) {

        auto tsp_begin = std::chrono::steady_clock::now();

        // Solve travelingsaleman instance.
        std::vector<Solution> tsp_solutions = solve_tsp_lkh(
                instance,
                tsp_instance,
                generator,
                lkh_candidate_file_content);

        auto tsp_end = std::chrono::steady_clock::now();
        std::chrono::duration<double> tsp_time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(tsp_end - tsp_begin);
        output.tsp_time += tsp_time_span.count();

        for (const Solution& tsp_solution: tsp_solutions) {

            auto pwt_begin = std::chrono::steady_clock::now();

            // Create packingwhiletravelling instance.
            auto pwt_instance = create_pwt_instance(
                    instance,
                    tsp_solution);

            packingwhiletravellingsolver::SequentialValueCorrectionOptionalParameters svc_parameters;
            //pwt_parameters.parameters.info.set_verbosity_level(1);
            auto svc_output = packingwhiletravellingsolver::sequential_value_correction(
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
            output.update_solution(svc_solution, ss, parameters.info);

            auto pwt_end = std::chrono::steady_clock::now();
            std::chrono::duration<double> pwt_time_span
                = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
            output.pwt_time += pwt_time_span.count();

            if (svc_solution.objective() > svc_solutions.back().objective()) {

                auto pwt_begin = std::chrono::steady_clock::now();

                svc_solutions.push_back(svc_solution);
                std::sort(
                        svc_solutions.begin(),
                        svc_solutions.end(),
                        [](
                            const Solution& solution_1,
                            const Solution& solution_2) -> bool
                        {
                            return solution_1.objective() > solution_2.objective();
                        });
                while ((Counter)svc_solutions.size() > svc_solutions_size)
                    svc_solutions.pop_back();

                packingwhiletravellingsolver::EfficientLocalSearchOptionalParameters els_parameters;
                auto els_output = packingwhiletravellingsolver::efficient_local_search(
                        pwt_instance,
                        els_parameters);

                // Retrieve solution.
                Solution els_solution = retrieve_solution(
                        instance,
                        tsp_solution,
                        els_output.solution);
                //std::cout << "pwtels " << els_solution.objective() << std::endl;

                output.number_of_els_calls++;

                // Update output.
                std::stringstream ss;
                ss << "iteration " << number_of_iterations << " (els)";
                output.update_solution(els_solution, ss, parameters.info);

                auto pwt_end = std::chrono::steady_clock::now();
                std::chrono::duration<double> pwt_time_span
                    = std::chrono::duration_cast<std::chrono::duration<double>>(pwt_end - pwt_begin);
                output.pwt_time += pwt_time_span.count();

                auto ttp_begin = std::chrono::steady_clock::now();

                // Run TTP-ELS.
                EfficientLocalSearchOptionalParameters ttpels_parameters;
                ttpels_parameters.initial_solution = &els_solution;
                ttpels_parameters.lkh_candidate_file_content = lkh_candidate_file_content;
                //ttpels_parameters.info.set_verbosity_level(1);
                auto ttpels_output = efficient_local_search(
                        instance,
                        generator,
                        ttpels_parameters);
                //std::cout << "ttpels " << ttpels_output.solution.objective() << std::endl;
                // Update output.
                {
                    std::stringstream ss;
                    ss << "iteration " << number_of_iterations << " (ttpels)";
                    output.update_solution(ttpels_output.solution, ss, parameters.info);
                }

                auto ttp_end = std::chrono::steady_clock::now();
                std::chrono::duration<double> ttp_time_span
                    = std::chrono::duration_cast<std::chrono::duration<double>>(ttp_end - ttp_begin);
                output.ttp_time += ttp_time_span.count();

            }
        }

    }


    output.algorithm_end(parameters.info);
    return output;
}
