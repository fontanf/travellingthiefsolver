#pragma once

#include "travellingthiefsolver/thieforienteering/solution.hpp"

#include "travellingthiefsolver/thieforienteering/algorithm_formatter.hpp"

#include "travellingthiefsolver/packingwhiletravelling/utils.hpp"

#include "localsearchsolver/sequencing.hpp"
#include "localsearchsolver/best_first_local_search.hpp"
//#include "localsearchsolver/genetic_local_search.hpp"

namespace travellingthiefsolver
{
namespace thieforienteering
{

/**
 * Local search algorithm for the travelling thief problem.
 *
 * The implementation is inspired by:
 * - "Investigation of the Traveling Thief Problem" (Wuijts, 2018)
 *   https://studenttheses.uu.nl/handle/20.500.12932/29179
 * - "Investigation of the traveling thief problem" (Wuijts et Thierens, 2019)
 *   https://doi.org/10.1145/3321707.3321766
 */
const Output local_search(
        const Instance& instance,
        const Parameters& parameters = {});

template <typename Distances>
const Output local_search(
        const Distances& distances,
        const Instance& instance,
        const Parameters& parameters = {});

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Distances>
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

        parameters.add_remove = true;
        parameters.replace = true;

        parameters.shift_change_mode_block_maximum_length = 1;
        parameters.shift_change_mode_maximum_mode_diff = -1;
        parameters.mode_swap = false;
        parameters.swap_with_modes = false;
        parameters.increment_decrement_modes = true;

        parameters.force_add = true;

        //parameters.order_crossover_weight = 1.0;
        //parameters.cycle_crossover_weight = 1.0;
        //parameters.maximal_preservative_crossover_weight = 1.0;

        return parameters;
    }

    /**
     * Global cost:
     * - Overweight
     * - Overtime
     * - Profit
     */
    using GlobalCost = std::tuple<Weight, Time, Profit>;

    struct SequenceData
    {
        localsearchsolver::sequencing::ElementId element_id_last = -1;
        Weight weight = 0;
        Profit profit = 0;
        Profit remaining_profit;
        Time time = 0;
        Time time_full = 0;
    };

    SequencingScheme(
            const Instance& instance,
            const Distances& distances,
            const std::vector<std::vector<travellingthiefsolver::packingwhiletravelling::CityState>>& city_states):
        instance_(instance),
        distances_(distances),
        city_states_(city_states)
    {
        total_profit_ = 0;
        for (ItemId item_id = 0;
                item_id < instance.number_of_items();
                ++item_id) {
            const Item& item = instance.item(item_id);
            total_profit_ += item.profit;
        }
    }

    inline localsearchsolver::sequencing::ElementPos number_of_elements() const { return instance_.number_of_cities() - 2; }

    inline localsearchsolver::sequencing::Mode number_of_modes(
            localsearchsolver::sequencing::ElementId element_id) const
    {
        return city_states_[element_id + 1].size();
    }

    inline SequenceData empty_sequence_data(
            localsearchsolver::sequencing::SequenceId) const
    {
        SequenceData sequence_data;
        sequence_data.remaining_profit = total_profit_;
        return sequence_data;
    }

    inline GlobalCost global_cost(const SequenceData& sequence_data) const
    {
        return {
            std::max((Weight)0, sequence_data.weight - instance_.capacity()),
            std::max((Time)0, sequence_data.time_full - instance_.time_limit()),
            -sequence_data.profit,
        };
    }

    inline GlobalCost bound(const SequenceData& sequence_data) const
    {
        return {
            std::max((Weight)0, sequence_data.weight - instance_.capacity()),
            std::max((Time)0, sequence_data.time_full - instance_.time_limit()),
            -(sequence_data.profit + sequence_data.remaining_profit),
        };
    }

    inline void append(
            SequenceData& sequence_data,
            localsearchsolver::sequencing::ElementId element_id,
            localsearchsolver::sequencing::Mode mode) const
    {
        // Update time_cur.
        sequence_data.time += instance_.duration(
                distances_,
                sequence_data.element_id_last + 1,
                element_id + 1,
                sequence_data.weight);
        // Update weight
        sequence_data.weight += city_states_[element_id + 1][mode].total_weight;
        // Update profit
        sequence_data.profit += city_states_[element_id + 1][mode].total_profit;
        // Update remaining_profit.
        sequence_data.remaining_profit -= city_states_[element_id + 1].back().total_profit;
        // Update time_full.
        sequence_data.time_full = sequence_data.time
            + instance_.duration(
                    distances_,
                    element_id + 1,
                    instance_.number_of_cities() - 1,
                    sequence_data.weight);
        // Update j_last.
        sequence_data.element_id_last = element_id;
    }

private:

    /** Instance. */
    const Instance& instance_;

    /** Distances. */
    const Distances& distances_;

    /** City states. */
    const std::vector<std::vector<travellingthiefsolver::packingwhiletravelling::CityState>>& city_states_;

    Profit total_profit_ = 0;

};

template <typename Distances>
const Output local_search(
        const Distances& distances,
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(distances, instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Local search");
    algorithm_formatter.print_header();

    auto city_states = packingwhiletravelling::compute_city_states<Instance>(instance);

    //for (CityId city_id = 0; city_id < instance.number_of_cities(); ++city_id) {
    //    std::cout << "city_id " << city_id << std::endl;
    //    for (const CityState& state: city_states[city_id]) {
    //        std::cout << "weight " << state.total_weight
    //            << " profit " << state.total_profit
    //            << " eff " << (double)state.total_profit / state.total_weight
    //            << " items";
    //        for (ItemId item_id: state.item_ids)
    //            std::cout << " " << item_id;
    //        std::cout << std::endl;
    //    }
    //}

    using LocalScheme = localsearchsolver::sequencing::LocalScheme<SequencingScheme<Distances>>;
    SequencingScheme<Distances> sequencing_scheme(instance, distances, city_states);
    localsearchsolver::sequencing::Parameters sequencing_parameters
        = sequencing_scheme.sequencing_parameters();
    LocalScheme local_scheme(sequencing_scheme, sequencing_parameters);

    localsearchsolver::BestFirstLocalSearchParameters<LocalScheme> bfls_parameters;
    bfls_parameters.verbosity_level = 0;
    bfls_parameters.timer = parameters.timer;
    //bfls_parameters.maximum_number_of_nodes = 100;
    bfls_parameters.new_solution_callback
        = [&instance, &distances, &city_states, &algorithm_formatter](
                const localsearchsolver::Output<LocalScheme>& ls_output)
        {
            Solution solution(distances, instance);
            for (auto se: ls_output.solution_pool.best().sequences[0].elements) {
                CityId city_id = se.element_id + 1;
                solution.add_city(distances, city_id);
                for (ItemId item_id: city_states[city_id][se.mode].item_ids) {
                    solution.add_item(distances, item_id);
                }
            }
            algorithm_formatter.update_solution(solution, "");
        };
    best_first_local_search(local_scheme, bfls_parameters);

    /*
    localsearchsolver::GeneticLocalSearchParameters<LocalScheme> gls_parameters;
    //gls_parameters.info.set_verbosity_level(1);
    gls_parameters.info.set_time_limit(info.remaining_time());
    //gls_parameters.maximum_number_of_iterations = 100;
    gls_parameters.number_of_threads = 6;
    gls_parameters.new_solution_callback
        = [&instance, &city_states, &info, &output](
                const LocalScheme::Solution& solution)
        {
            Solution sol(instance);
            for (auto se: solution.sequences[0].elements) {
                CityId city_id = se.element_id + 1;
                sol.add_city(city_id);
                for (ItemId item_id: city_states[city_id][se.mode].item_ids) {
                    sol.add_item(item_id);
                }
            }
            std::stringstream ss;
            algorithm_formatter.update_solution(sol, ss, info);
        };
    genetic_local_search(local_scheme, gls_parameters);
    */

    algorithm_formatter.end();
    return output;
}

}
}
