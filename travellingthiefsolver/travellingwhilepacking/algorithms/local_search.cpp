#include "travellingthiefsolver/travellingwhilepacking/algorithms/local_search.hpp"

#include "localsearchsolver/sequencing.hpp"
#include "localsearchsolver/best_first_local_search.hpp"
#include "localsearchsolver/genetic_local_search.hpp"

using namespace travellingthiefsolver::travellingwhilepacking;

class SequencingScheme
{

public:

    static localsearchsolver::sequencing::Parameters sequencing_parameters()
    {
        localsearchsolver::sequencing::Parameters parameters;

        parameters.shift_block_maximum_length = 7;
        parameters.swap_block_maximum_length = 5;
        parameters.reverse = true;
        parameters.shift_reverse_block_maximum_length = 6;

        parameters.double_bridge_number_of_perturbations = 10;

        parameters.order_crossover_weight = 1.0;

        return parameters;
    }

    /**
     * Global cost:
     * - Profit
     */
    using GlobalCost = std::tuple<Profit>;

    struct SequenceData
    {
        localsearchsolver::sequencing::ElementId element_id_last = -1;
        Weight weight = 0;
        Time time = 0;
        Time time_full = 0;
    };

    SequencingScheme(const Instance& instance): instance_(instance) { }

    inline localsearchsolver::sequencing::ElementPos number_of_elements() const { return instance_.number_of_cities() - 1; }

    inline SequenceData empty_sequence_data(localsearchsolver::sequencing::SequenceId) const
    {
        SequenceData sequence_data;
        sequence_data.weight = instance_.city(0).weight;
        return sequence_data;
    }

    inline GlobalCost global_cost(const SequenceData& sequence_data) const
    {
        return {
            sequence_data.time_full,
        };
    }

    inline GlobalCost bound(const SequenceData& sequence_data) const
    {
        return {
            sequence_data.time_full,
        };
    }

    inline void append(
            SequenceData& sequence_data,
            localsearchsolver::sequencing::ElementId element_id) const
    {
        // Update time_cur.
        sequence_data.time += instance_.duration(
                sequence_data.element_id_last + 1,
                element_id + 1,
                sequence_data.weight);
        // Update weight
        sequence_data.weight += instance_.city(element_id + 1).weight;
        // Update time_full.
        sequence_data.time_full = sequence_data.time
            + instance_.duration(element_id + 1, 0, sequence_data.weight);
        // Update j_last.
        sequence_data.element_id_last = element_id;
    }

private:

    /** Instance. */
    const Instance& instance_;

};

Output travellingthiefsolver::travellingwhilepacking::local_search(
        const Instance& instance,
        optimizationtools::Info info)
{
    init_display(instance, info);
    info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Local search" << std::endl
        << std::endl;

    Output output(instance, info);

    using LocalScheme = localsearchsolver::sequencing::LocalScheme<SequencingScheme>;
    SequencingScheme sequencing_scheme(instance);
    localsearchsolver::sequencing::Parameters sequencing_parameters
        = sequencing_scheme.sequencing_parameters();
    LocalScheme local_scheme(sequencing_scheme, sequencing_parameters);

    /*
    localsearchsolver::BestFirstLocalSearchOptionalParameters<LocalScheme> bfls_parameters;
    bfls_parameters.info.set_verbosity_level(1);
    bfls_parameters.info.set_time_limit(info.remaining_time());
    bfls_parameters.maximum_number_of_nodes = 100;
    bfls_parameters.new_solution_callback
        = [&instance, &info, &output](
                const LocalScheme::Solution& solution)
        {
            Solution sol(instance);
            for (auto se: solution.sequences[0].elements) {
                CityId city_id = se.element_id + 1;
                sol.add_city(city_id);
            }
            std::stringstream ss;
            output.update_solution(sol, ss, info);
        };
    best_first_local_search(local_scheme, bfls_parameters);
    */

    localsearchsolver::GeneticLocalSearchOptionalParameters<LocalScheme> gls_parameters;
    gls_parameters.info.set_verbosity_level(1);
    gls_parameters.info.set_time_limit(info.remaining_time());
    gls_parameters.maximum_number_of_iterations = 100;
    gls_parameters.number_of_threads = 6;
    gls_parameters.new_solution_callback
        = [&instance, &info, &output](
                const LocalScheme::Solution& solution)
        {
            Solution sol(instance);
            for (auto se: solution.sequences[0].elements) {
                CityId city_id = se.element_id + 1;
                sol.add_city(city_id);
            }
            std::stringstream ss;
            output.update_solution(sol, ss, info);
        };
    genetic_local_search(local_scheme, gls_parameters);

    return output.algorithm_end(info);
}
