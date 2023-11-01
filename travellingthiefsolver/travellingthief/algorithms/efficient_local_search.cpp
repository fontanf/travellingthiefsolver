#include "travellingthiefsolver/travellingthief/algorithms/efficient_local_search.hpp"

#include "travellingthiefsolver/travellingthief/utils.hpp"
#include "travellingthiefsolver/packingwhiletravelling/utils.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/sequential_value_correction.hpp"
#include "travellingthiefsolver/packingwhiletravelling/algorithms/efficient_local_search.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

#include "localsearchsolver/sequencing.hpp"
#include "localsearchsolver/multi_start_local_search.hpp"
#include "localsearchsolver/iterated_local_search.hpp"
#include "localsearchsolver/genetic_local_search.hpp"

using namespace travellingthiefsolver::travellingthief;

using CityState = travellingthiefsolver::packingwhiletravelling::CityState;
using CityStateId = travellingthiefsolver::packingwhiletravelling::CityStateId;

void EfficientLocalSearchOutput::print_statistics(
        optimizationtools::Info& info) const
{
    if (info.verbosity_level() >= 1) {
        info.os()
            << "Number of iterations:         " << number_of_iterations << std::endl
            << "Number of improvements:       " << number_of_improvements << std::endl
            << "# of CSS impr.:               " << number_of_change_city_state_improvements << std::endl
            << "# of 2O improvements:         " << number_of_two_opt_improvements << std::endl
            << "# of 2OCCS improvements:      " << number_of_two_opt_change_city_states_improvements << std::endl
            << "# of S improvements:          " << number_of_shift_improvements << std::endl
            << "# of SCCS improvements:       " << number_of_shift_change_city_state_improvements << std::endl
            << "# of CTCS improvements:       " << number_of_change_two_city_states_improvements << std::endl
            ;
    }
    info.add_to_json("Algorithm", "NumberOfIterations", number_of_iterations);
}

struct EfficientLocalSearchSolutionCity
{
    /** City state. */
    CityStateId city_state_id = 0;

    /** Position. */
    CityPos position = -1;
};

struct EfficientLocalSearchSolutionVisit
{
    /** City. */
    CityId city_id = 0;

    /** Distance from start. */
    Distance distance_from_start = 0;

    /** Time from start. */
    Time time_from_start = 0;

    /** Time to end. */
    Time time_to_end = 0;

    /** Weight from start. */
    Weight weight_from_start = 0;

    /** Profit from start. */
    Profit profit_from_start = 0;
};

/**
 * Structure for a solution of the efficient local search algorithm.
 */
class EfficientLocalSearchSolution
{

public:

    /** Constructor. */
    EfficientLocalSearchSolution(
            const Instance& instance,
            const std::vector<std::vector<CityState>>& city_states,
            const std::vector<CityId>& city_ids,
            const std::vector<CityStateId>& city_state_ids);

    /*
     * Getters.
     */

    /** Get the instance. */
    inline const Instance& instance() const { return *instance_; }

    /** Get city states. */
    inline const std::vector<std::vector<CityState>>& city_states() const { return *city_states_; }

    /** Get the number of visited cities. */
    CityId number_of_cities() const { return visits_.size(); }

    /** Get a visit. */
    const EfficientLocalSearchSolutionVisit& visit(CityPos city_pos) const { return visits_[city_pos]; }

    /** Get a city. */
    const EfficientLocalSearchSolutionCity& city(CityId city_id) const { return cities_[city_id]; }

    /** Get the profit of the items selected in the solution. */
    inline Profit profit() const { return profit_; }

    /** Get the weight of the items selected in the solution. */
    inline Weight weight() const { return weight_; }

    /** Get the overweight of the solution. */
    inline Weight overweight() const
    {
        return std::max(
                (Weight)0,
                weight() - instance().capacity());
    }

    /** Get the full distance of the solution. */
    inline Distance distance() const { return distance_; }

    /** Get the full travel time of the solution. */
    inline Time travel_time() const { return travel_time_; }

    /** Get the renting cost. */
    inline Profit renting_cost() const { return instance().renting_ratio() * travel_time_; }

    /** Get the objective value of the solution. */
    inline Profit objective() const { return profit() - renting_cost(); }

    /*
     * Export
     */

    Solution to_solution() const
    {
        // Retrieve solution.
        Solution solution(instance());
        for (CityId city_pos = 1;
                city_pos < instance().number_of_cities();
                ++city_pos) {
            CityId city_id = visit(city_pos).city_id;
            CityStateId city_state_id = city(city_id).city_state_id;
            solution.add_city(city_id);
            for (ItemId item_id: city_states()[city_id][city_state_id].item_ids)
                solution.add_item(item_id);
        }
        return solution;
    }


private:

    /*
     * Private attributes
     */

    /** Instance. */
    const Instance* instance_;

    /** City states. */
    const std::vector<std::vector<CityState>>* city_states_;

    /** Visits in the order they are performed. */
    std::vector<EfficientLocalSearchSolutionVisit> visits_;

    /** For each city, its position. */
    std::vector<EfficientLocalSearchSolutionCity> cities_;

    /** Profit of the solution. */
    Profit profit_ = 0;

    /** Weight of the solution. */
    Weight weight_ = 0;

    /** Total distance of the solution. */
    Time distance_ = 0;

    /** Travel time of the solution. */
    Time travel_time_ = 0;

    /** Objective value of the solution. */
    Profit objective_;

};

EfficientLocalSearchSolution::EfficientLocalSearchSolution(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        const std::vector<CityId>& city_ids,
        const std::vector<CityStateId>& city_state_ids):
    instance_(&instance),
    city_states_(&city_states),
    visits_(1),
    cities_(instance.number_of_cities())
{
    if (city_ids.size() == 1)
        return;

    CityId city_id_prev = 0;
    for (CityPos city_pos = 1;
            city_pos < (CityId)city_ids.size();
            ++city_pos) {
        CityId city_id = city_ids[city_pos];
        CityStateId city_state_id = city_state_ids[city_id];
        if (city_state_id >= (CityStateId)city_states[city_id].size()) {
            std::cout << "city_pos " << city_pos
                << " city_id " << city_id
                << " city_state_id " << city_state_id
                << " / " << city_states[city_id].size()
                << std::endl;
            throw std::runtime_error(
                    "city_state_id >= (CityStateId)city_states[city_id].size()");
        }
        const CityState& city_state = city_states[city_id][city_state_id];

        const EfficientLocalSearchSolutionVisit& visit_prev = this->visit(city_pos - 1);

        EfficientLocalSearchSolutionVisit visit;
        visit.city_id = city_id;

        Distance distance = instance.distance(city_id_prev, city_id);
        visit.distance_from_start = visit_prev.distance_from_start + distance;
        distance_ += distance;

        Time time = instance.duration(city_id_prev, city_id, visit_prev.weight_from_start);
        visit.time_from_start = visit_prev.time_from_start + time;
        travel_time_ += time;

        visit.weight_from_start = visit_prev.weight_from_start + city_state.total_weight;
        weight_ += city_state.total_weight;

        visit.profit_from_start = visit_prev.profit_from_start + city_state.total_profit;
        profit_ += city_state.total_profit;

        visits_.push_back(visit);

        cities_[city_id].position = city_pos;
        cities_[city_id].city_state_id = city_state_ids[city_id];

        city_id_prev = city_id;
    }

    distance_ += instance.distance(city_id_prev, 0);
    travel_time_ += instance.duration(city_id_prev, 0, weight_);

    // Compute remining_times.
    CityPos city_pos = city_ids.size() - 1;
    CityId city_id = city_ids[city_pos];
    visits_[city_pos].time_to_end
        = instance.duration(
                city_id,
                0,
                this->visit(city_pos).weight_from_start);
    for (CityId city_pos = (CityId)city_ids.size() - 2;
            city_pos >= 0;
            --city_pos) {
        CityId city_id = city_ids[city_pos];
        CityId city_id_next = city_ids[city_pos + 1];
        visits_[city_pos].time_to_end
            = visit(city_pos + 1).time_to_end
            + instance.duration(
                    city_id,
                    city_id_next,
                    visit(city_pos).weight_from_start);
    }

    objective_ = profit_ - instance.renting_ratio() * travel_time_;
}

class EfficientLocalScheme
{

public:

    enum class MoveType
    {
        ChangeCityState,
        TwoOpt,
        Shift,
        ShiftChangeCityState,
        ShiftChangeCityState2,
        ChangeTwoCtiyStates,
        TwoOptChangeCityStates,
    };

    /**
     * Structure for a move.
     */
    struct Move
    {
        /** Move type. */
        MoveType type;

        /** City. */
        CityId city_id = -1;

        /** City. */
        CityId city_id_2 = -1;

        /** New city state. */
        CityStateId city_state_id = -1;
    };

    /*
     * Constructors and destructor.
     */

    EfficientLocalScheme(
            const Instance& instance,
            const std::vector<std::vector<CityState>>& city_states,
            EfficientLocalSearchOptionalParameters& parameters,
            EfficientLocalSearchOutput& output,
            std::mt19937_64& generator);

    /*
     * Global cost.
     */

    /** Global cost: <Number of cities, Overcapacity, Profit>; */
    using GlobalCost = std::tuple<CityId, Weight, Profit>;

    inline CityId&       number_of_cities(GlobalCost& global_cost) const { return std::get<0>(global_cost); }
    inline Weight&           overcapacity(GlobalCost& global_cost) const { return std::get<1>(global_cost); }
    inline Profit&                 profit(GlobalCost& global_cost) const { return std::get<2>(global_cost); }
    inline CityId  number_of_cities(const GlobalCost& global_cost) const { return std::get<0>(global_cost); }
    inline Weight      overcapacity(const GlobalCost& global_cost) const { return std::get<1>(global_cost); }
    inline Profit            profit(const GlobalCost& global_cost) const { return std::get<2>(global_cost); }

    /*
     * Solution.
     */

    using Solution = EfficientLocalSearchSolution;

    inline GlobalCost global_cost(const Solution& solution) const
    {
        return {
            -solution.number_of_cities(),
            std::max((Weight)0, solution.weight() - instance_.capacity()),
            -solution.objective(),
        };
    }

    inline Solution empty_solution() const
    {
        std::vector<CityId> city_ids = {0};
        std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), -1);
        city_state_ids[0] = 0;
        return EfficientLocalSearchSolution(
                instance_,
                city_states_,
                city_ids,
                city_state_ids);
    }

    void generate_initial_solutions(
            std::mt19937_64& generator)
    {
        auto tsp_solutions = solve_tsp_lkh(
                instance_,
                tsp_instance_,
                generator,
                lkh_candidate_file_content_);

        for (const auto& tsp_solution: tsp_solutions) {

            // Create packingwhiletravelling instance.
            auto pwt_instance = create_pwt_instance(
                    instance_,
                    tsp_solution);

            travellingthiefsolver::packingwhiletravelling::EfficientLocalSearchOptionalParameters els_parameters;
            //pwt_parameters.parameters.info.set_verbosity_level(1);
            auto els_output = travellingthiefsolver::packingwhiletravelling::efficient_local_search(
                    pwt_instance,
                    els_parameters);

            // Retrieve solution.
            auto els_solution = retrieve_solution(
                    instance_,
                    tsp_solution,
                    els_output.solution);

            auto city_state_ids = solution2states(
                    instance_,
                    city_states_,
                    els_solution);
            std::vector<CityId> city_ids;
            for (CityPos city_pos = 0;
                    city_pos < instance_.number_of_cities();
                    ++city_pos) {
                CityId city_id = tsp_solution.city_id(city_pos);
                city_ids.push_back(city_id);
            }
            EfficientLocalSearchSolution ls_solution(
                    instance_,
                    city_states_,
                    city_ids,
                    city_state_ids);
            local_search(ls_solution, generator);
            std::stringstream ss;
            output_.update_solution(ls_solution.to_solution(), ss, parameters_.info);
            initial_solutions_.push_back(ls_solution);
        }
    }

    inline Solution initial_solution(
            Counter,
            std::mt19937_64& generator)
    {
        if (initial_solutions_.empty()) {
            generate_initial_solutions(generator);
        }

        Counter i_best = -1;
        for (Counter i = 0;
                i < (Counter)initial_solutions_.size();
                ++i) {
            if (i_best == -1
                    || initial_solutions_[i_best].objective()
                    < initial_solutions_[i].objective()) {
                i_best = i;
            }
        }
        Solution solution = initial_solutions_[i_best];
        initial_solutions_[i_best] = initial_solutions_.back();
        initial_solutions_.pop_back();
        return solution;
    }

    /*
     * Local search.
     */

    struct Perturbation;

    void local_search(
            Solution& solution,
            std::mt19937_64& generator,
            Perturbation = {});

    /*
     * Iterated local search
     */

    struct Perturbation
    {
        Perturbation(): global_cost(localsearchsolver::worst<GlobalCost>()) { }

        GlobalCost global_cost;
    };

    inline std::vector<Perturbation> perturbations(
            const Solution& solution,
            std::mt19937_64&)
    {
        std::vector<Perturbation> perturbations;
        for (Counter c = 0; c < 64; ++c) {
            Perturbation perturbation;
            perturbation.global_cost = global_cost(solution);
            perturbations.push_back(perturbation);
        }
        return perturbations;
    }

    void apply_perturbation(
            Solution& solution,
            const Perturbation&,
            std::mt19937_64& generator) const;

    /*
     * Genetic local search.
     */

    Solution cycle_crossover(
            const Solution& solution_parent_1,
            const Solution& solution_parent_2,
            std::mt19937_64& generator);

    Solution crossover(
            const Solution& solution_parent_1,
            const Solution& solution_parent_2,
            std::mt19937_64& generator);

    ItemId distance(
            const Solution& solution_1,
            const Solution& solution_2) const;

    inline void write(
            const Solution&,
            std::string) const
    {
    }

private:

    /*
     * Manipulate solutions.
     */

    /*
     * Evaluate moves.
     */

    struct EvaluateChangeCityStateOutput
    {
        Weight overweight;
        Profit objective;
    };

    EvaluateChangeCityStateOutput evaluate_change_city_state_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id,
            CityStateId city_state_id);

    Profit evaluate_two_opt_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id_1,
            CityId city_id_2);

    struct EvaluateTwoOptChangeCityStatesOutput
    {
        std::vector<CityStateId> city_state_ids;
        Profit objective;
    };

    EvaluateTwoOptChangeCityStatesOutput evaluate_two_opt_change_city_states_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id_1,
            CityId city_id_2);

    struct EvaluateShiftOutput
    {
        CityPos city_pos;
        Profit objective;
    };

    EvaluateShiftOutput evaluate_shift_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id);

    struct EvaluateShiftChangeCityStateOutput
    {
        CityPos city_pos;
        Weight overweight;
        Profit objective;
    };

    EvaluateShiftChangeCityStateOutput evaluate_shift_change_city_state_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id,
            CityStateId city_state_id);

    struct EvaluateShiftChangeCityState2Output
    {
        CityPos city_pos;
        CityStateId city_state_id;
        Weight overweight;
        Profit objective;
    };

    EvaluateShiftChangeCityState2Output evaluate_shift_change_city_state_2_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id);

    struct EvaluateChangeTwoCityStatesOutput
    {
        CityId city_id_2;
        CityStateId city_state_id_1;
        CityStateId city_state_id_2;
    };

    EvaluateChangeTwoCityStatesOutput evaluate_change_two_city_states_move(
            const EfficientLocalSearchSolution& solution,
            CityId city_id_1);

    /*
     * Private attributes.
     */

    /** Instance. */
    const Instance& instance_;

    /** City states. */
    const std::vector<std::vector<CityState>>& city_states_;

    /** TSP instance. */
    travelingsalesmansolver::Instance tsp_instance_;

    /** LKH candidate file content. */
    std::string lkh_candidate_file_content_;

    /** LKH candidates. */
    std::vector<travelingsalesmansolver::LkhCandidate> lkh_candidates_;

    /** Parameters. */
    EfficientLocalSearchOptionalParameters& parameters_;

    /** Output. */
    EfficientLocalSearchOutput& output_;

    /** Moves. */
    std::vector<std::vector<Move>> moves_;

    /** Initial solutions. */
    mutable std::vector<Solution> initial_solutions_;

};

EfficientLocalScheme::EfficientLocalScheme(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        EfficientLocalSearchOptionalParameters& parameters,
        EfficientLocalSearchOutput& output,
        std::mt19937_64& generator):
    instance_(instance),
    city_states_(city_states),
    tsp_instance_(create_tsp_instance(instance)),
    lkh_candidate_file_content_(parameters.lkh_candidate_file_content),
    parameters_(parameters),
    output_(output)
{
    if (parameters.lkh_candidate_file_content.empty())
        generate_initial_solutions(generator);
    lkh_candidates_ = travelingsalesmansolver::read_candidates(lkh_candidate_file_content_);

    // Generate moves.
    // Change city state.
    if (parameters.neighborhood_change_city_state != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_change_city_state)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            for (CityStateId city_state_id = 0;
                    city_state_id < (CityStateId)city_states[city_id].size();
                    ++city_state_id) {
                Move move;
                move.type = MoveType::ChangeCityState;
                move.city_id = city_id;
                move.city_state_id = city_state_id;
                moves_[parameters.neighborhood_change_city_state].push_back(move);
            }
        }
    }
    // Two-opt.
    if (parameters.neighborhood_two_opt != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_two_opt)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            for (const auto& candidate: lkh_candidates_[city_id].edges) {
                Move move;
                move.type = MoveType::TwoOpt;
                move.city_id = city_id;
                move.city_id_2 = candidate.vertex_id;
                moves_[parameters.neighborhood_two_opt].push_back(move);
            }
        }
    }
    // Two-opt-change-city-states.
    if (parameters.neighborhood_two_opt_change_city_states != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_two_opt_change_city_states)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            for (const auto& candidate: lkh_candidates_[city_id].edges) {
                Move move;
                move.type = MoveType::TwoOptChangeCityStates;
                move.city_id = city_id;
                move.city_id_2 = candidate.vertex_id;
                moves_[parameters.neighborhood_two_opt_change_city_states].push_back(move);
            }
        }
    }
    // Shift.
    if (parameters.neighborhood_shift != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_shift)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            Move move;
            move.type = MoveType::Shift;
            move.city_id = city_id;
            moves_[parameters.neighborhood_shift].push_back(move);
        }
    }
    // Shift change city state.
    if (parameters.neighborhood_shift_change_city_state != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_shift_change_city_state)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            for (CityStateId city_state_id = 0;
                    city_state_id < (CityId)city_states[city_id].size();
                    ++city_state_id) {
                Move move;
                move.type = MoveType::ShiftChangeCityState;
                move.city_id = city_id;
                move.city_state_id = city_state_id;
                moves_[parameters.neighborhood_shift_change_city_state].push_back(move);
            }
        }
    }
    // Shift change city state 2.
    if (parameters.neighborhood_shift_change_city_state_2 != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_shift_change_city_state_2)
            moves_.push_back({});
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            Move move;
            move.type = MoveType::ShiftChangeCityState2;
            move.city_id = city_id;
            moves_[parameters.neighborhood_shift_change_city_state_2].push_back(move);
        }
    }
    // Change two city states.
    if (parameters.neighborhood_change_two_city_states != -1) {
        while ((Counter)moves_.size() <= parameters.neighborhood_change_two_city_states)
            moves_.push_back({});
        std::vector<Move> moves_tmp;
        for (CityId city_id = 1;
                city_id < instance.number_of_cities();
                ++city_id) {
            Move move;
            move.type = MoveType::ChangeTwoCtiyStates;
            move.city_id = city_id;
            moves_[parameters.neighborhood_change_two_city_states].push_back(move);
        }
    }
}

void EfficientLocalScheme::local_search(
        Solution& solution,
        std::mt19937_64& generator,
        Perturbation)
{
    //std::cout << "local_search..." << std::endl;
    std::vector<CityId> city_ids;
    std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), -1);
    for (CityPos city_pos = 0;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        CityId city_id = solution.visit(city_pos).city_id;
        city_ids.push_back(city_id);
        city_state_ids[city_id] = solution.city(city_id).city_state_id;
    }

    Counter number_of_iterations = 0;
    for (number_of_iterations = 0;
            !parameters_.info.needs_to_end();
            ++number_of_iterations) {

        //std::cout << "number_of_iterations " << number_of_iterations
        //    << " overweight " << solution.overweight()
        //    << " profit " << solution.profit()
        //    << " distance " << solution.distance()
        //    << " travel_time " << solution.travel_time()
        //    << " objective " << solution.objective()
        //    << std::endl;

        Counter number_of_improvements = 0;

        for (std::vector<Move>& moves: moves_) {

            // Shuffle moves.
            std::shuffle(moves.begin(), moves.end(), generator);

            for (const Move& move: moves) {

                if (move.type == MoveType::ChangeCityState) {

                    auto output = evaluate_change_city_state_move(
                            solution,
                            move.city_id,
                            move.city_state_id);

                    if (solution.overweight() > output.overweight
                            || (solution.overweight() == output.overweight
                                && slightly_greater(solution.objective())
                                < output.objective)) {

                        // Update current solution.
                        city_state_ids[move.city_id] = move.city_state_id;
                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (std::fabs(solution.objective() - output.objective)
                                > std::fabs(solution.objective() * 1e-6)) {
                            std::cout << "ChangeCityState"
                                << " overweight_new " << output.overweight
                                << " objective_new " << output.objective
                                << " solution.objective() " << solution.objective()
                                << " diff " << std::fabs(solution.objective() - output.objective)
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_change_city_state_improvements++;
                    }

                } else if (move.type == MoveType::TwoOpt) {

                    Profit objective_new = evaluate_two_opt_move(
                            solution,
                            move.city_id,
                            move.city_id_2);

                    if (slightly_greater(solution.objective()) < objective_new) {
                        std::vector<CityId> city_ids_new;
                        CityPos city_pos_1 = solution.city(move.city_id).position;
                        CityPos city_pos_2 = solution.city(move.city_id_2).position;
                        for (CityPos city_pos = 0;
                                city_pos <= city_pos_1;
                                ++city_pos) {
                            city_ids_new.push_back(solution.visit(city_pos).city_id);
                        }
                        for (CityPos city_pos = city_pos_2;
                                city_pos > city_pos_1;
                                --city_pos) {
                            city_ids_new.push_back(solution.visit(city_pos).city_id);
                        }
                        for (CityPos city_pos = city_pos_2 + 1;
                                city_pos < instance_.number_of_cities();
                                ++city_pos) {
                            city_ids_new.push_back(solution.visit(city_pos).city_id);
                        }
                        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
                            throw std::runtime_error(
                                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
                        }
                        city_ids = city_ids_new;

                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (std::fabs(solution.objective() - objective_new)
                                > std::fabs(solution.objective() * 1e-9)) {
                            std::cout << "TwoOpt"
                                << " objective_new " << objective_new
                                << " solution.objective() " << solution.objective()
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_two_opt_improvements++;
                    }

                } else if (move.type == MoveType::TwoOptChangeCityStates) {

                    auto output = evaluate_two_opt_change_city_states_move(
                            solution,
                            move.city_id,
                            move.city_id_2);

                    if (slightly_greater(solution.objective()) < output.objective) {
                        std::vector<CityId> city_ids_new;
                        CityPos city_pos_1 = solution.city(move.city_id).position;
                        CityPos city_pos_2 = solution.city(move.city_id_2).position;
                        Profit profit = 0;
                        for (CityPos city_pos = 0;
                                city_pos <= city_pos_1;
                                ++city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            city_ids_new.push_back(city_id);
                            profit += city_states_[city_id][city_state_ids[city_id]].total_profit;
                        }
                        CityPos x = 0;
                        for (CityPos city_pos = city_pos_2;
                                city_pos > city_pos_1;
                                --city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            city_ids_new.push_back(city_id);
                            city_state_ids[city_id] = output.city_state_ids[x];
                            x++;
                        }
                        for (CityPos city_pos = city_pos_2 + 1;
                                city_pos < instance_.number_of_cities();
                                ++city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            city_ids_new.push_back(city_id);
                            profit += city_states_[city_id][city_state_ids[city_id]].total_profit;
                        }
                        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
                            throw std::runtime_error(
                                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
                        }
                        city_ids = city_ids_new;

                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (std::fabs(solution.objective() - output.objective)
                                > std::fabs(solution.objective() * 1e-7)) {
                            std::cout << "TwoOptChangeCityStates"
                                << " objective_new " << output.objective
                                << " solution.objective() " << solution.objective()
                                << " solution.profit() " << solution.profit()
                                << " solution.travel_time() " << solution.travel_time()
                                << " profit " << profit
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_two_opt_change_city_states_improvements++;
                    }

                } else if (move.type == MoveType::Shift) {

                    auto output = evaluate_shift_move(
                            solution,
                            move.city_id);

                    if (slightly_greater(solution.objective()) < output.objective) {
                        // Update current solution.
                        std::vector<CityId> city_ids_new;
                        for (CityPos city_pos = 0;
                                city_pos < instance_.number_of_cities();
                                ++city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            if (city_id == move.city_id)
                                continue;
                            city_ids_new.push_back(city_id);
                            if ((CityPos)city_ids_new.size() == output.city_pos)
                                city_ids_new.push_back(move.city_id);
                        }
                        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
                            throw std::runtime_error(
                                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
                        }

                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids_new,
                                city_state_ids);
                        city_ids = city_ids_new;

                        if (std::fabs(solution.objective() - output.objective)
                                > std::fabs(solution.objective() * 1e-8)) {
                            std::cout << "Shift"
                                << " objective_new " << output.objective
                                << " solution.objective() " << solution.objective()
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_shift_improvements++;
                    }

                } else if (move.type == MoveType::ShiftChangeCityState) {

                    auto output = evaluate_shift_change_city_state_move(
                            solution,
                            move.city_id,
                            move.city_state_id);

                    if (solution.overweight() > output.overweight
                            || (solution.overweight() == output.overweight
                                && slightly_greater(solution.objective()) < output.objective)) {
                        // Update current solution.
                        city_state_ids[move.city_id] = move.city_state_id;
                        std::vector<CityId> city_ids_new;
                        for (CityPos city_pos = 0;
                                city_pos < instance_.number_of_cities();
                                ++city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            if (city_id == move.city_id)
                                continue;
                            city_ids_new.push_back(city_id);
                            if ((CityPos)city_ids_new.size() == output.city_pos)
                                city_ids_new.push_back(move.city_id);
                        }
                        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
                            throw std::runtime_error(
                                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
                        }
                        city_ids = city_ids_new;

                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (std::fabs(solution.objective() - output.objective)
                                > std::fabs(solution.objective() * 1e-8)) {
                            std::cout << "ShiftChangeCityState"
                                << " objective_new " << output.objective
                                << " solution.objective() " << solution.objective()
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_shift_change_city_state_improvements++;
                    }

                } else if (move.type == MoveType::ShiftChangeCityState2) {

                    auto output = evaluate_shift_change_city_state_2_move(
                            solution,
                            move.city_id);

                    if (solution.overweight() > output.overweight
                            || (solution.overweight() == output.overweight
                                && slightly_greater(solution.objective()) < output.objective)) {

                        //if (solution.city(move.city_id).position
                        //        != std::get<0>(t)) {
                        //    std::cout << "CityId " << move.city_id
                        //        << " city_pos " << solution.city(move.city_id).position
                        //        << " -> " << std::get<0>(t)
                        //        << " city_state_id " << solution.city(move.city_id).city_state_id
                        //        << " -> " << std::get<1>(t)
                        //        << std::endl;
                        //}

                        // Update current solution.
                        city_state_ids[move.city_id] = output.city_state_id;
                        std::vector<CityId> city_ids_new;
                        for (CityPos city_pos = 0;
                                city_pos < instance_.number_of_cities();
                                ++city_pos) {
                            CityId city_id = solution.visit(city_pos).city_id;
                            if (city_id == move.city_id)
                                continue;
                            city_ids_new.push_back(city_id);
                            if ((CityPos)city_ids_new.size() == output.city_pos)
                                city_ids_new.push_back(move.city_id);
                        }
                        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
                            throw std::runtime_error(
                                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
                        }
                        city_ids = city_ids_new;

                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (std::fabs(solution.objective() - output.objective)
                                > std::fabs(solution.objective() * 1e-8)) {
                            std::cout << "ShiftChangeCityState2"
                                << " objective_new " << output.objective
                                << " solution.objective() " << solution.objective()
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() != objective_new");
                        }

                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_shift_change_city_state_improvements++;
                    }


                } else if (move.type == MoveType::ChangeTwoCtiyStates) {

                    auto output = evaluate_change_two_city_states_move(
                            solution,
                            move.city_id);

                    if (output.city_id_2 != -1) {
                        // Update current solution.
                        city_state_ids[move.city_id] = output.city_state_id_1;
                        city_state_ids[output.city_id_2] = output.city_state_id_2;
                        Profit objective_old = solution.objective();
                        //std::cout << "solution.objective() " << solution.objective() << std::endl;
                        solution = EfficientLocalSearchSolution(
                                instance_,
                                city_states_,
                                city_ids,
                                city_state_ids);

                        if (solution.objective() <= objective_old) {
                            std::cout << "ChangeTwoCtiyStates"
                                << " travel_time " << solution.travel_time()
                                << " objective_old " << objective_old
                                << " solution.objective() " << solution.objective()
                                << std::endl;
                            //throw std::runtime_error(
                            //        "solution.objective() <= objective_old");
                        }

                        //std::cout << "solution.objective() " << solution.objective() << std::endl;
                        number_of_improvements++;
                        output_.number_of_improvements++;
                        output_.number_of_change_two_city_states_improvements++;
                    }

                }
            }
            if (number_of_improvements > 0)
                break;
        }

        //std::cout << number_of_improvements << std::endl;
        //std::cout << solution_objective << std::endl;
        if (number_of_improvements == 0)
            break;
    }

    output_.number_of_local_search_calls++;
    output_.number_of_iterations += number_of_iterations;
    //std::cout << "local_search end" << std::endl;
}

EfficientLocalScheme::EvaluateChangeCityStateOutput EfficientLocalScheme::evaluate_change_city_state_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id,
        CityStateId city_state_id)
{
    CityStateId city_state_id_cur = solution.city(city_id).city_state_id;
    if (city_state_id == city_state_id_cur) {
        return EvaluateChangeCityStateOutput{
            std::numeric_limits<Weight>::max(),
            -std::numeric_limits<Profit>::infinity()};
    }
    Profit profit_new = solution.profit()
        - city_states_[city_id][city_state_id_cur].total_profit
        + city_states_[city_id][city_state_id].total_profit;
    Weight weight_new = solution.weight()
        - city_states_[city_id][city_state_id_cur].total_weight
        + city_states_[city_id][city_state_id].total_weight;
    Weight overweight_new = std::max(
            (Weight)0,
            weight_new - instance_.capacity());
    if (solution.overweight() < overweight_new) {
        return EvaluateChangeCityStateOutput{
            std::numeric_limits<Weight>::max(),
            -std::numeric_limits<Profit>::infinity()};
    }
    CityPos city_pos = solution.city(city_id).position;
    Weight weight_diff =
        - city_states_[city_id][city_state_id_cur].total_weight
        + city_states_[city_id][city_state_id].total_weight;
    Time time_cur = 0;

    if (city_state_id > city_state_id_cur) {
        CityId city_id_2_next = 0;
        for (CityId city_pos_2 = instance_.number_of_cities() - 1;
                city_pos_2 >= city_pos;
                --city_pos_2) {
            CityId city_id_2 = solution.visit(city_pos_2).city_id;
            Weight weight_cur = solution.visit(city_pos_2).weight_from_start
                + weight_diff;
            time_cur += instance_.duration(
                    city_id_2,
                    city_id_2_next,
                    weight_cur);
            Profit bound = profit_new
                - instance_.renting_ratio()
                * (solution.visit(city_pos_2).time_from_start + time_cur);
            if (bound < solution.objective()) {
                return EvaluateChangeCityStateOutput{
                    std::numeric_limits<Weight>::max(),
                    -std::numeric_limits<Profit>::infinity()};
            }
            city_id_2_next = city_id_2;
        }
        time_cur += solution.visit(city_pos).time_from_start;
    } else {
        CityId city_id_2_next = 0;
        for (CityId city_pos_2 = instance_.number_of_cities() - 1;
                city_pos_2 >= city_pos;
                --city_pos_2) {
            CityId city_id_2 = solution.visit(city_pos_2).city_id;
            Weight weight_cur = solution.visit(city_pos_2).weight_from_start
                + weight_diff;
            time_cur += instance_.duration(
                    city_id_2,
                    city_id_2_next,
                    weight_cur);
            //std::cout << "city_id " << city_id_2
            //    << " city_id_next " << city_id_2_next
            //    << " weight " << weight_cur
            //    << " time " << instance.duration(city_id_2, city_id_2_next, weight_cur)
            //    << std::endl;
            double speed_cur = instance_.speed(solution.visit(city_pos_2).weight_from_start);
            double speed_new = instance_.speed(weight_cur);
            double unitary_spared_time = 1.0 / speed_cur - 1.0 / speed_new;
            Profit bound = profit_new
                - instance_.renting_ratio()
                * (solution.visit(city_pos_2).time_from_start
                        + time_cur
                        - unitary_spared_time
                        * (solution.visit(city_pos_2).distance_from_start
                            - solution.visit(city_pos).distance_from_start));
            if (bound < solution.objective()) {
                return EvaluateChangeCityStateOutput{
                    std::numeric_limits<Weight>::max(),
                    -std::numeric_limits<Profit>::infinity()};
            }
            city_id_2_next = city_id_2;
        }
        time_cur += solution.visit(city_pos).time_from_start;
    }

    //std::cout << "profit_new " << profit_new << " time_cur " << time_cur << std::endl;
    return EvaluateChangeCityStateOutput{
        overweight_new,
        profit_new - instance_.renting_ratio() * time_cur};
}

Profit EfficientLocalScheme::evaluate_two_opt_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id_1,
        CityId city_id_2)
{
    // If city_id_1 is visited before city_id_2:
    // 0 - ... - X - 1 - A - B - C - 2 - Y - ... - 0
    // =>
    // 0 - ... - X - 1 - 2 - C - B - A - Y - ... - 0
    //
    // If city_id_1 is visited after city_id_2.
    // 0 - ... - X - 2 - A - B - C - 1 - Y - ... - 0
    // =>
    // 0 - ... - X - C - B - A - 2 - 1 - Y - ... - 0
    // This is equivalent to TwoOpt(X, C)
    // Therefore, we don't consider this case.

    CityPos city_pos_1 = solution.city(city_id_1).position;
    CityPos city_pos_2 = solution.city(city_id_2).position;
    if (city_pos_1 > city_pos_2)
        return -std::numeric_limits<Profit>::infinity();

    // We don't consider the case where city_id_2 is visited right after
    // city_id_1.
    if (city_pos_2 == city_pos_1 + 1)
        return -std::numeric_limits<Profit>::infinity();

    // Segment 0 - ... - X - 1
    //std::cout << "time_from_start " << solution.visit(city_pos_1).time_from_start << std::endl;
    Time time_cur = solution.visit(city_pos_1).time_from_start;

    // Segment Y - ... - 0
    if (city_pos_2 != instance_.number_of_cities() - 1)
        time_cur += solution.visit(city_pos_2 + 1).time_to_end;
    //std::cout << "time_to_end " << solution.visit(city_pos_2 + 1).time_to_end << std::endl;

    // Segment A - Y
    {
        CityId city_id_3 = solution.visit(city_pos_1 + 1).city_id;
        CityId city_id_4 = (city_pos_2 == instance_.number_of_cities() - 1)?
            0:
            solution.visit(city_pos_2 + 1).city_id;
        time_cur += instance_.duration(
                city_id_3,
                city_id_4,
                solution.visit(city_pos_2).weight_from_start);
        //std::cout << "AY city_id_1 " << city_id_3
        //    << " city_id_2 " << city_id_4
        //    << " time " << instance.duration(city_id_3, city_id_4, solution.visit(city_pos_2).weight_from_start)
        //    << std::endl;
    }

    // Segment
    // A - B - C - 2
    // =>
    // 2 - C - B - A
    Weight weight_cur = solution.visit(city_pos_2).weight_from_start;

    Distance remaining_distance = (city_pos_2 == instance_.number_of_cities() - 1)?
        solution.distance():
        solution.visit(city_pos_2 + 1).distance_from_start;
    remaining_distance -= solution.visit(city_pos_1).distance_from_start;
    {
        // A
        CityId city_id_3 = solution.visit(city_pos_1 + 1).city_id;
        // Y
        CityId city_id_4 = (city_pos_2 == instance_.number_of_cities() - 1)?
            0:
            solution.visit(city_pos_2 + 1).city_id;
        // Two-opt 1 - A => 1 - 2.
        remaining_distance -= instance_.distance(city_id_1, city_id_3);
        remaining_distance += instance_.distance(city_id_1, city_id_2);
        // Two-opt 2 - Y => A - Y.
        remaining_distance -= instance_.distance(city_id_2, city_id_4);
        remaining_distance += instance_.distance(city_id_3, city_id_4);
    }

    double speed_1 = instance_.speed(solution.visit(city_pos_1).weight_from_start);
    for (CityPos city_pos_4 = city_pos_1 + 1;
            city_pos_4 < city_pos_2;
            ++city_pos_4) {
        Profit bound = solution.profit()
            - instance_.renting_ratio() * time_cur
            - instance_.renting_ratio() * remaining_distance / speed_1
            ;
        if (bound < solution.objective())
            return -std::numeric_limits<Profit>::infinity();
        CityId city_id_4 = solution.visit(city_pos_4).city_id;
        CityId city_id_3 = solution.visit(city_pos_4 + 1).city_id;
        CityStateId city_state_id_4 = solution.city(city_id_4).city_state_id;
        weight_cur -= city_states_[city_id_4][city_state_id_4].total_weight;
        time_cur += instance_.duration(
                city_id_3,
                city_id_4,
                weight_cur);
        //std::cout << "t " << instance.duration(
        //        city_id_3,
        //        city_id_4,
        //        weight_cur)
        //    << std::endl;
        remaining_distance -= instance_.distance(city_id_3, city_id_4);
    }

    // Segment 1 - 2
    time_cur += instance_.duration(
            city_id_1,
            city_id_2,
            solution.visit(city_pos_1).weight_from_start);
    //std::cout << "12 "
    //    << " city_id_1 " << city_id_1
    //    << " city_id_2 " << city_id_2
    //    << " time " << instance.duration(city_id_1, city_id_2, solution.visit(city_pos_1).weight_from_start)
    //    << std::endl;

    return solution.profit() - instance_.renting_ratio() * time_cur;
}

struct TwoOptChangeCityStatesStruct
{
    CityPos city_pos;
    CityPos city_pos_3;
    CityId city_id;
    CityStateId city_state_id;
    Weight weight;
    double efficiency;
};

EfficientLocalScheme::EvaluateTwoOptChangeCityStatesOutput EfficientLocalScheme::evaluate_two_opt_change_city_states_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id_1,
        CityId city_id_2)
{
    CityPos city_pos_1 = solution.city(city_id_1).position;
    CityPos city_pos_2 = solution.city(city_id_2).position;
    if (city_pos_1 > city_pos_2) {
        return EvaluateTwoOptChangeCityStatesOutput{
            {},
            -std::numeric_limits<Profit>::infinity()};
    }

    // We don't consider the case where city_id_2 is visited right after
    // city_id_1.
    if (city_pos_2 == city_pos_1 + 1) {
        return EvaluateTwoOptChangeCityStatesOutput{
            {},
            -std::numeric_limits<Profit>::infinity()};
    }

    Profit profit_new = solution.profit()
        - solution.visit(city_pos_2).profit_from_start
        + solution.visit(city_pos_1).profit_from_start;
    //std::cout << "profit_new " << profit_new << std::endl;

    // Segment 0 - ... - X - 1
    //std::cout << "time_from_start " << solution.visit(city_pos_1).time_from_start << std::endl;
    Time time_cur = solution.visit(city_pos_1).time_from_start;

    // Segment 1 - 2
    time_cur += instance_.duration(
            city_id_1,
            city_id_2,
            solution.visit(city_pos_1).weight_from_start);
    //std::cout << "12 "
    //    << " city_id_1 " << city_id_1
    //    << " city_id_2 " << city_id_2
    //    << " time " << solution.visit(city_pos_1 + 1).time_from_start
    //    << " -> " << time_cur
    //    << std::endl;

    // Segment
    // A - B - C - 2
    // =>
    // 2 - C - B - A

    std::vector<TwoOptChangeCityStatesStruct> states;
    CityPos city_pos = city_pos_1 + 1;
    for (CityPos city_pos_3 = city_pos_2;
            city_pos_3 > city_pos_1;
            --city_pos_3) {
        CityId city_id_3 = solution.visit(city_pos_3).city_id;
        for (CityStateId city_state_id_3 = 1;
                city_state_id_3 < (CityStateId)city_states_[city_id_3].size();
                ++city_state_id_3) {
            const CityState& city_state = city_states_[city_id_3][city_state_id_3];
            TwoOptChangeCityStatesStruct s;
            s.city_pos = city_pos;
            s.city_pos_3 = city_pos_3;
            s.city_id = city_id_3;
            s.city_state_id = city_state_id_3;
            s.weight = city_state.total_weight;
            s.efficiency = city_state.total_profit / city_state.total_weight;
            states.push_back(s);
        }
        city_pos++;
    }
    std::sort(
            states.begin(),
            states.end(),
            [](
                const TwoOptChangeCityStatesStruct& s1,
                const TwoOptChangeCityStatesStruct& s2) -> bool
            {
                if (s1.efficiency != s2.efficiency)
                    return s1.efficiency > s2.efficiency;
                return s1.city_pos > s2.city_pos;
            });

    std::vector<Weight> weights_cur(city_pos_2 - city_pos_1, solution.visit(city_pos_1).weight_from_start);
    std::vector<CityStateId> city_state_ids_cur(city_pos_2 - city_pos_1, 0);
    for (const auto& s: states) {
        CityPos pos = s.city_pos - (city_pos_1 + 1);
        CityStateId city_state_id_cur = city_state_ids_cur[pos];
        if (s.city_state_id < city_state_ids_cur[pos])
            continue;
        bool ok = true;
        for (CityPos p = pos;
                p < (CityPos)weights_cur.size();
                ++p) {
            if (weights_cur[p]
                    - city_states_[s.city_id][city_state_id_cur].total_weight
                    + s.weight
                    > solution.visit(city_pos_1 + 1 + p).weight_from_start) {
                ok = false;
                break;
            }
        }
        if (!ok)
            continue;
        //std::cout << "city_pos " << s.city_pos
        //    << " city_id " << s.city_id
        //    << " city_state_id " << s.city_state_id
        //    << " weight " << s.weight
        //    << " weight_cur " << weights_cur[pos]
        //    << " / " << solution.visit(s.city_pos).weight_from_start
        //    << " eff " << s.efficiency
        //    << std::endl;
        for (CityPos p = pos;
                p < (CityPos)weights_cur.size();
                ++p) {
            weights_cur[p] += s.weight
                - city_states_[s.city_id][city_state_id_cur].total_weight;
        }
        city_state_ids_cur[pos] = s.city_state_id;
    }

    Weight weight_cur = solution.visit(city_pos_1).weight_from_start;
    std::vector<CityStateId> city_state_ids_new;
    city_pos = city_pos_1 + 1;
    for (CityPos city_pos_4 = city_pos_2 - 1;
            city_pos_4 > city_pos_1;
            --city_pos_4) {
        CityId city_id_4 = solution.visit(city_pos_4).city_id;
        CityId city_id_3 = solution.visit(city_pos_4 + 1).city_id;
        CityStateId city_state_id_3 = city_state_ids_cur[city_state_ids_new.size()];
        city_state_ids_new.push_back(city_state_id_3);
        weight_cur += city_states_[city_id_3][city_state_id_3].total_weight;
        profit_new += city_states_[city_id_3][city_state_id_3].total_profit;
        time_cur += instance_.duration(
                city_id_3,
                city_id_4,
                weight_cur);
        //std::cout
        //    << "city_pos_1 " << city_pos_1
        //    << " city_pos " << city_pos
        //    << " city_pos_2 " << city_pos_2
        //    << " city_id " << city_id_3
        //    << " city_state_id " << city_state_id_3
        //    << " w1 " << solution.visit(city_pos_1).weight_from_start
        //    << " w2 " << solution.visit(city_pos_2).weight_from_start
        //    << " w " << solution.visit(city_pos).weight_from_start
        //    << " -> " << weight_cur
        //    << " p " << profit_new
        //    << " t " << solution.visit(city_pos + 1).time_from_start
        //    << " -> " << time_cur
        //    << std::endl;
        city_pos++;
    }

    // Segment A - Y
    {
        CityId city_id_3 = solution.visit(city_pos_1 + 1).city_id;
        CityId city_id_4 = (city_pos_2 == instance_.number_of_cities() - 1)?
            0:
            solution.visit(city_pos_2 + 1).city_id;
        CityStateId city_state_id_3 = city_state_ids_cur[city_state_ids_new.size()];
        city_state_ids_new.push_back(city_state_id_3);
        weight_cur += city_states_[city_id_3][city_state_id_3].total_weight;
        profit_new += city_states_[city_id_3][city_state_id_3].total_profit;
        time_cur += instance_.duration(
                city_id_3,
                city_id_4,
                weight_cur);
        //std::cout
        //    << "AY city_pos_1 " << city_pos_1
        //    << " city_pos " << city_pos
        //    << " city_pos_2 " << city_pos_2
        //    << " w1 " << solution.visit(city_pos_1).weight_from_start
        //    << " w " << solution.visit(city_pos).weight_from_start
        //    << " -> " << weight_cur
        //    << " w2 " << solution.visit(city_pos_2).weight_from_start
        //    << " p_new " << profit_new
        //    << " t " << solution.visit(city_pos + 1).time_from_start
        //    << " -> " << time_cur
        //    << std::endl;
    }

    // Segment Y - ... - 0
    if (city_pos_2 != instance_.number_of_cities() - 1) {
        if (weight_cur == solution.visit(city_pos_2 + 1).weight_from_start) {
            time_cur += solution.visit(city_pos_2 + 1).time_to_end;
        } else {
            Weight weight_diff =
                - solution.visit(city_pos_2).weight_from_start
                + weight_cur;
            //std::cout << "weight_diff " << weight_diff << std::endl;
            CityId city_id_4 = 0;
            for (CityId city_pos_3 = instance_.number_of_cities() - 1;
                    city_pos_3 > city_pos_2;
                    --city_pos_3) {
                CityId city_id_3 = solution.visit(city_pos_3).city_id;
                Weight weight_cur = solution.visit(city_pos_3).weight_from_start
                    + weight_diff;
                time_cur += instance_.duration(
                        city_id_3,
                        city_id_4,
                        weight_cur);
                city_id_4 = city_id_3;
            }
        }
    }
    //std::cout << "time_to_end " << solution.visit(city_pos_2 + 1).time_to_end << std::endl;
    //std::cout << "profit_new " << profit_new
    //    << " time_cur " << time_cur
    //    << std::endl;

    Profit objective_new = profit_new - instance_.renting_ratio() * time_cur;
    //std::cout
    //    << "city_pos_1 " << city_pos_1
    //    << " city_pos_2 " << city_pos_2
    //    << std::endl
    //    << " solution.profit() " << solution.profit()
    //    << " profit_new " << profit_new
    //    << std::endl
    //    << " solution.travel_time() " << solution.travel_time()
    //    << " time_cur " << time_cur
    //    << std::endl
    //    << " solution.objective() " << solution.objective()
    //    << " objective_new " << objective_new
    //    << std::endl;
    return EvaluateTwoOptChangeCityStatesOutput{
        city_state_ids_new,
        objective_new};
}

EfficientLocalScheme::EvaluateShiftOutput EfficientLocalScheme::evaluate_shift_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id)
{
    const CityState& city_state = city_states_[city_id][solution.city(city_id).city_state_id];

    std::vector<Time> time_from_start = {0};
    std::vector<Weight> weight_from_start = {0};
    std::vector<CityId> city_ids = {0};
    CityId city_id_prev = 0;
    for (CityPos city_pos = 1;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        CityId city_id_cur = solution.visit(city_pos).city_id;
        if (city_id_cur == city_id)
            continue;
        const CityState& city_state_cur = city_states_[city_id_cur][solution.city(city_id_cur).city_state_id];
        city_ids.push_back(city_id_cur);
        time_from_start.push_back(time_from_start.back() + instance_.duration(city_id_prev, city_id_cur, weight_from_start.back()));
        weight_from_start.push_back(weight_from_start.back() + city_state_cur.total_weight);
        city_id_prev = city_id_cur;
    }

    Profit objective_best = -std::numeric_limits<Profit>::infinity();
    CityPos city_pos_best = -1;
    CityId city_id_next = 0;
    Time time_to_end = 0;
    for (CityPos city_pos = instance_.number_of_cities() - 1;
            city_pos > 0;
            --city_pos) {
        CityId city_id_cur = city_ids[city_pos - 1];
        //std::cout << "city_id_cur " << city_id_cur
        //    << " city_id " << city_id
        //    << " city_id_next " << city_id_next
        //    << " number_of_cities " << instance.number_of_cities()
        //    << std::endl;
        Time time_cur
            = time_from_start[city_pos - 1]
            + instance_.duration(
                    city_id_cur,
                    city_id,
                    weight_from_start[city_pos - 1])
            + instance_.duration(
                    city_id,
                    city_id_next,
                    weight_from_start[city_pos - 1] + city_state.total_weight)
            + time_to_end;

        time_to_end += instance_.duration(
                city_id_cur,
                city_id_next,
                weight_from_start[city_pos - 1] + city_state.total_weight);
        city_id_next = city_id_cur;

        double objective = solution.profit() - instance_.renting_ratio() * time_cur;
        if (objective_best < objective) {
            objective_best = objective;
            city_pos_best = city_pos;
        }
    }

    return EvaluateShiftOutput{city_pos_best, objective_best};
}

EfficientLocalScheme::EvaluateShiftChangeCityStateOutput EfficientLocalScheme::evaluate_shift_change_city_state_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id,
        CityStateId city_state_id)
{
    const CityState& city_state_old = city_states_[city_id][solution.city(city_id).city_state_id];
    const CityState& city_state = city_states_[city_id][city_state_id];
    Weight weight_new = solution.weight()
        - city_state_old.total_weight
        + city_state.total_weight;
    Weight overweight_new = std::max(
            (Weight)0,
            weight_new - instance_.capacity());
    if (solution.overweight() < overweight_new)
        return EvaluateShiftChangeCityStateOutput{
            -1,
            std::numeric_limits<Weight>::max(),
            -std::numeric_limits<Profit>::infinity()};
    Profit profit_new = solution.profit()
        - city_state_old.total_profit
        + city_state.total_profit;

    std::vector<Time> time_from_start = {0};
    std::vector<Weight> weight_from_start = {0};
    std::vector<CityId> city_ids = {0};
    CityId city_id_prev = 0;
    for (CityPos city_pos = 1;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        CityId city_id_cur = solution.visit(city_pos).city_id;
        if (city_id_cur == city_id)
            continue;
        const CityState& city_state_cur = city_states_[city_id_cur][solution.city(city_id_cur).city_state_id];
        city_ids.push_back(city_id_cur);
        time_from_start.push_back(time_from_start.back() + instance_.duration(city_id_prev, city_id_cur, weight_from_start.back()));
        weight_from_start.push_back(weight_from_start.back() + city_state_cur.total_weight);
        city_id_prev = city_id_cur;
    }

    Profit objective_best = -std::numeric_limits<Profit>::infinity();
    CityPos city_pos_best = -1;
    CityId city_id_next = 0;
    Time time_to_end = 0;
    for (CityPos city_pos = instance_.number_of_cities() - 1;
            city_pos > 0;
            --city_pos) {
        CityId city_id_cur = city_ids[city_pos - 1];
        //std::cout << "city_id_cur " << city_id_cur
        //    << " city_id " << city_id
        //    << " city_id_next " << city_id_next
        //    << " number_of_cities " << instance.number_of_cities()
        //    << std::endl;
        Time time_cur
            = time_from_start[city_pos - 1]
            + instance_.duration(
                    city_id_cur,
                    city_id,
                    weight_from_start[city_pos - 1])
            + instance_.duration(
                    city_id,
                    city_id_next,
                    weight_from_start[city_pos - 1] + city_state.total_weight)
            + time_to_end;

        time_to_end += instance_.duration(
                city_id_cur,
                city_id_next,
                weight_from_start[city_pos - 1] + city_state.total_weight);
        city_id_next = city_id_cur;

        double objective = profit_new - instance_.renting_ratio() * time_cur;
        if (objective_best < objective) {
            objective_best = objective;
            city_pos_best = city_pos;
        }
    }

    return EvaluateShiftChangeCityStateOutput{
        city_pos_best,
        overweight_new,
        objective_best};
}

EfficientLocalScheme::EvaluateShiftChangeCityState2Output EfficientLocalScheme::evaluate_shift_change_city_state_2_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id)
{
    const CityState& city_state_old = city_states_[city_id][solution.city(city_id).city_state_id];

    Profit objective_best = solution.objective();
    CityPos city_pos_best = -1;
    CityStateId city_state_id_best = -1;
    Weight overweight_best = solution.overweight();

    for (CityStateId city_state_id = 0;
            city_state_id < (CityStateId)city_states_[city_id].size();
            ++city_state_id) {
        auto output = evaluate_change_city_state_move(
                solution,
                city_id,
                city_state_id);
        if (solution.overweight() < output.overweight)
            continue;
        if (slightly_greater(objective_best) < output.objective) {
            city_pos_best = solution.city(city_id).position;
            city_state_id_best = city_state_id;
            overweight_best = output.overweight;
            objective_best = output.objective;
            //std::cout << "*" << std::endl;
            //break;
        }
    }
    if (city_pos_best != -1) {
        return EvaluateShiftChangeCityState2Output{
            city_pos_best,
            city_state_id_best,
            overweight_best,
            objective_best};
    }

    for (CityStateId city_state_id = 0;
            city_state_id < (CityStateId)city_states_[city_id].size();
            ++city_state_id) {

        const CityState& city_state = city_states_[city_id][city_state_id];
        Weight weight_new = solution.weight()
            - city_state_old.total_weight
            + city_state.total_weight;
        Weight overweight_new = std::max(
                (Weight)0,
                weight_new - instance_.capacity());
        if (solution.overweight() < overweight_new)
            continue;
        Profit profit_new = solution.profit()
            - city_state_old.total_profit
            + city_state.total_profit;

        std::vector<Time> time_from_start = {0};
        std::vector<Weight> weight_from_start = {0};
        std::vector<CityId> city_ids = {0};
        CityId city_id_prev = 0;
        for (CityPos city_pos = 1;
                city_pos < instance_.number_of_cities();
                ++city_pos) {
            CityId city_id_cur = solution.visit(city_pos).city_id;
            if (city_id_cur == city_id)
                continue;
            const CityState& city_state_cur = city_states_[city_id_cur][solution.city(city_id_cur).city_state_id];
            city_ids.push_back(city_id_cur);
            time_from_start.push_back(time_from_start.back() + instance_.duration(city_id_prev, city_id_cur, weight_from_start.back()));
            weight_from_start.push_back(weight_from_start.back() + city_state_cur.total_weight);
            city_id_prev = city_id_cur;
        }
        CityId city_id_next = 0;
        Time time_to_end = 0;
        for (CityPos city_pos = instance_.number_of_cities() - 1;
                city_pos > 0;
                --city_pos) {
            CityId city_id_cur = city_ids[city_pos - 1];
            //std::cout << "city_id_cur " << city_id_cur
            //    << " city_id " << city_id
            //    << " city_id_next " << city_id_next
            //    << " number_of_cities " << instance.number_of_cities()
            //    << std::endl;
            Time time_cur
                = time_from_start[city_pos - 1]
                + instance_.duration(
                        city_id_cur,
                        city_id,
                        weight_from_start[city_pos - 1])
                + instance_.duration(
                        city_id,
                        city_id_next,
                        weight_from_start[city_pos - 1] + city_state.total_weight)
                + time_to_end;

            time_to_end += instance_.duration(
                    city_id_cur,
                    city_id_next,
                    weight_from_start[city_pos - 1] + city_state.total_weight);
            city_id_next = city_id_cur;

            double objective = profit_new - instance_.renting_ratio() * time_cur;
            //std::cout << "city_pos " << city_pos
            //    << " (" << solution.city(city_id).position << ")"
            //    << " time_cur " << time_cur
            //    << " (" << solution.travel_time() << ")"
            //    << " objective " << objective
            //    << " (" << solution.objective() << ")"
            //    << std::endl;
            if (slightly_greater(objective_best) < objective) {
                objective_best = objective;
                city_pos_best = city_pos;
                city_state_id_best = city_state_id;
                overweight_best = overweight_new;
                //std::cout << "*" << std::endl;
                //break;
            }
        }
    }

    return EvaluateShiftChangeCityState2Output{
        city_pos_best,
        city_state_id_best,
        overweight_best,
        objective_best};
}

EfficientLocalScheme::EvaluateChangeTwoCityStatesOutput EfficientLocalScheme::evaluate_change_two_city_states_move(
        const EfficientLocalSearchSolution& solution,
        CityId city_id_1)
{
    CityPos city_pos_1 = solution.city(city_id_1).position;
    CityStateId city_state_id_1_cur = solution.city(city_id_1).city_state_id;
    const CityState& city_state_1_cur = city_states_[city_id_1][city_state_id_1_cur];

    CityPos city_id_2_best = -1;
    CityStateId city_state_id_1_best = -1;
    CityStateId city_state_id_2_best = -1;
    Profit objective_lower_bound_best = -std::numeric_limits<Profit>::infinity();

    for (CityPos city_pos_2 = city_pos_1 + 1;
            city_pos_2 < instance_.number_of_cities();
            ++city_pos_2) {

        CityId city_id_2 = solution.visit(city_pos_2).city_id;
        CityStateId city_state_id_2_cur = solution.city(city_id_2).city_state_id;
        const CityState& city_state_2_cur = city_states_[city_id_2][city_state_id_2_cur];

        for (CityStateId city_state_id_1 = 0;
                city_state_id_1 < city_state_id_1_cur;
                ++city_state_id_1) {
            const CityState& city_state_1 = city_states_[city_id_1][city_state_id_1];
            for (CityStateId city_state_id_2 = city_state_id_2_cur + 1;
                    city_state_id_2 < (CityStateId)city_states_[city_id_2].size();
                    ++city_state_id_2) {
                const CityState& city_state_2 = city_states_[city_id_2][city_state_id_2];
                if (solution.weight()
                        - city_state_1_cur.total_weight
                        - city_state_2_cur.total_weight
                        + city_state_1.total_weight
                        + city_state_2.total_weight
                        > instance_.capacity()) {
                    continue;
                }
                Profit profit_diff
                    = - city_state_1_cur.total_profit
                    + city_state_1.total_profit
                    - city_state_2_cur.total_profit
                    + city_state_2.total_profit;
                //std::cout << "profit_diff " << profit_diff << std::endl;
                //std::cout << "city_state_1_cur.total_weight " << city_state_1_cur.total_weight << std::endl;
                //std::cout << "city_state_2_cur.total_weight " << city_state_2_cur.total_weight << std::endl;
                //std::cout << "city_state_1.total_weight " << city_state_1.total_weight << std::endl;
                //std::cout << "city_state_2.total_weight " << city_state_2.total_weight << std::endl;
                Profit objective_lower_bound = solution.objective() + profit_diff;
                if (city_state_1_cur.total_weight
                        < city_state_1.total_weight) {
                    // If the weight at the first city is greater, we look at
                    // the unit loss at the last edge of the middle
                    // sub-sequence.
                    //std::cout << "0 0" << std::endl;
                    Weight weight_new = solution.visit(city_pos_2 - 1).weight_from_start
                        - city_state_1_cur.total_weight
                        + city_state_1.total_weight;
                    double speed_cur = instance_.speed(solution.visit(city_pos_2 - 1).weight_from_start);
                    double speed_new = instance_.speed(weight_new);
                    double unitary_lost_time = 1.0 / speed_new - 1.0 / speed_cur;
                    //std::cout << "unitary_lost_time " << unitary_lost_time << std::endl;
                    Distance d = solution.visit(city_pos_2).distance_from_start
                        - solution.visit(city_pos_1).distance_from_start;
                    //std::cout << "d " << d << " / " << solution.distance() << std::endl;
                    Profit loss = instance_.renting_ratio() * d * unitary_lost_time;
                    //std::cout << "loss " << loss << std::endl;
                    objective_lower_bound -= loss;
                } else {
                    // If the weight at the first city is smaller, we look at
                    // the unit gain at the first edge of the middle
                    // sub-sequence.
                    //std::cout << "0 1" << std::endl;
                    Weight weight_new = solution.visit(city_pos_1).weight_from_start
                        - city_state_1_cur.total_weight
                        + city_state_1.total_weight;
                    double speed_cur = instance_.speed(solution.visit(city_pos_1).weight_from_start);
                    double speed_new = instance_.speed(weight_new);
                    double unitary_gained_time = 1.0 / speed_cur - 1.0 / speed_new;
                    //std::cout << "unitary_gained_time " << unitary_gained_time << std::endl;
                    Distance d = solution.visit(city_pos_2).distance_from_start
                        - solution.visit(city_pos_1).distance_from_start;
                    //std::cout << "d " << d << " / " << solution.distance() << std::endl;
                    Profit gain = instance_.renting_ratio() * d * unitary_gained_time;
                    //std::cout << "gain " << gain << std::endl;
                    objective_lower_bound += gain;
                }
                if (city_state_1_cur.total_weight
                        + city_state_2_cur.total_weight
                        < city_state_1.total_weight
                        + city_state_2.total_weight) {
                    // If the weight at the second city is greater, we look at
                    // the unit loss at the last edge of the last
                    // sub-sequence.
                    //std::cout << "1 0" << std::endl;
                    Weight weight_new = solution.visit(instance_.number_of_cities() - 1).weight_from_start
                        - city_state_1_cur.total_weight
                        - city_state_2_cur.total_weight
                        + city_state_1.total_weight
                        + city_state_2.total_weight;
                    double speed_cur = instance_.speed(solution.visit(instance_.number_of_cities() - 1).weight_from_start);
                    double speed_new = instance_.speed(weight_new);
                    //std::cout << "capacity " << instance_.capacity() << std::endl;
                    //std::cout << "weight_cur " << solution.visit(instance_.number_of_cities() - 1).weight_from_start << std::endl;
                    //std::cout << "weight_new " << weight_new << std::endl;
                    //std::cout << "speed_cur " << speed_cur << std::endl;
                    //std::cout << "speed_new " << speed_new << std::endl;
                    //std::cout << speed_cur - speed_new << std::endl;
                    double unitary_lost_time = 1.0 / speed_new - 1.0 / speed_cur;
                    //std::cout << "unitary_lost_time " << unitary_lost_time << std::endl;
                    Distance d = solution.distance()
                        - solution.visit(city_pos_2).distance_from_start;
                    //std::cout << "d " << d << " / " << solution.distance() << std::endl;
                    Profit loss = instance_.renting_ratio() * d * unitary_lost_time;
                    //std::cout << "loss " << loss << std::endl;
                    objective_lower_bound -= loss;
                } else {
                    // If the weight at the second city is smaller, we look at
                    // the unit gain at the first edge of the middle
                    // sub-sequence.
                    //std::cout << "1 1" << std::endl;
                    Weight weight_new = solution.visit(city_pos_2).weight_from_start
                        - city_state_1_cur.total_weight
                        - city_state_2_cur.total_weight
                        + city_state_1.total_weight
                        + city_state_2.total_weight;
                    double speed_cur = instance_.speed(solution.visit(city_pos_2).weight_from_start);
                    double speed_new = instance_.speed(weight_new);
                    double unitary_gained_time = 1.0 / speed_cur - 1.0 / speed_new;
                    //std::cout << "unitary_gained_time " << unitary_gained_time << std::endl;
                    Distance d = solution.distance()
                        - solution.visit(city_pos_2).distance_from_start;
                    //std::cout << "d " << d << " / " << solution.distance() << std::endl;
                    Profit gain = instance_.renting_ratio() * d * unitary_gained_time;
                    //std::cout << "gain " << gain << std::endl;
                    objective_lower_bound += gain;
                }

                if (objective_lower_bound > objective_lower_bound_best) {
                    //std::cout << "objective_lower_bound " << objective_lower_bound << std::endl;
                    //std::vector<CityId> city_ids = {0};
                    //std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), 0);
                    //for (CityPos city_pos = 1;
                    //        city_pos < instance_.number_of_cities();
                    //        ++city_pos) {
                    //    CityId city_id = solution.visit(city_pos).city_id;
                    //    city_ids.push_back(city_id);
                    //    if (city_id == city_id_1) {
                    //        city_state_ids[city_id] = city_state_id_1;
                    //    } else if (city_id == city_id_2) {
                    //        city_state_ids[city_id] = city_state_id_2;
                    //    } else {
                    //        city_state_ids[city_id] = solution.city(city_id).city_state_id;
                    //    }
                    //}
                    //Solution solution_new = EfficientLocalSearchSolution(
                    //        instance_,
                    //        city_states_,
                    //        city_ids,
                    //        city_state_ids);
                    //std::cout << "time " << solution.travel_time() << std::endl;
                    //std::cout << "time_new " << solution_new.travel_time() << std::endl;
                    //std::cout << "cost " << instance_.renting_ratio() * solution.travel_time() << std::endl;
                    //std::cout << "cost_new " << instance_.renting_ratio() * solution_new.travel_time() << std::endl;
                    //Profit spared = 0;
                    //for (CityPos city_pos = city_pos_1;
                    //        city_pos < city_pos_2;
                    //        ++city_pos) {
                    //    CityId city_id = solution.visit(city_pos).city_id;
                    //    Time time_1_diff = solution.visit(city_pos + 1).time_from_start - solution.visit(city_pos).time_from_start;
                    //    Time time_2_diff = solution_new.visit(city_pos + 1).time_from_start - solution_new.visit(city_pos).time_from_start;
                    //    Time unitary_gained_time = (time_1_diff - time_2_diff)
                    //        / instance_.distance(city_id, city_id + 1);
                    //    spared += instance_.renting_ratio() * (time_1_diff - time_2_diff);
                    //    std::cout << "city_pos " << city_pos
                    //        << " weight_1 " << solution.visit(city_pos).weight_from_start
                    //        << " weight_2 " << solution_new.visit(city_pos).weight_from_start
                    //        << " time_1 " << solution.visit(city_pos).time_from_start
                    //        << " time_2 " << solution_new.visit(city_pos).time_from_start
                    //        << " time_1_diff " << time_1_diff
                    //        << " time_2_diff " << time_2_diff
                    //        << " spared " << instance_.renting_ratio() * (time_1_diff - time_2_diff)
                    //        << " unitary_gained_time " << unitary_gained_time
                    //        << " " << spared
                    //        << std::endl;
                    //}
                    city_id_2_best = city_id_2;
                    city_state_id_1_best = city_state_id_1;
                    city_state_id_2_best = city_state_id_2;
                }
            }
        }
    }

    if (objective_lower_bound_best > solution.objective()) {
        return EvaluateChangeTwoCityStatesOutput{
            city_id_2_best,
            city_state_id_1_best,
            city_state_id_2_best};
    }

    if (city_id_2_best == -1) {
        return EvaluateChangeTwoCityStatesOutput{-1, -1, -1};
    }

    CityPos city_pos_2 = solution.city(city_id_2_best).position;
    CityStateId city_state_id_2_cur = solution.city(city_id_2_best).city_state_id;
    const CityState& city_state_2_cur = city_states_[city_id_2_best][city_state_id_2_cur];
    const CityState& city_state_1 = city_states_[city_id_1][city_state_id_1_best];
    const CityState& city_state_2 = city_states_[city_id_2_best][city_state_id_2_best];

    Profit profit_new = solution.profit()
        - city_state_1_cur.total_profit
        - city_state_2_cur.total_profit
        + city_state_1.total_profit
        + city_state_2.total_profit;

    Time time_cur = solution.visit(city_pos_1).time_from_start;

    Weight weight_diff_1
        = -city_state_1_cur.total_weight
        + city_state_1.total_weight;
    for (CityPos city_pos = city_pos_1;
            city_pos < city_pos_2;
            ++city_pos) {
        CityId city_id = solution.visit(city_pos).city_id;
        CityId city_id_next = (city_pos == instance_.number_of_cities() - 1)?
            0:
            solution.visit(city_pos + 1).city_id;
        Weight weight_cur = solution.visit(city_pos).weight_from_start + weight_diff_1;
        time_cur += instance_.duration(
                city_id,
                city_id_next,
                weight_cur);
    }

    Weight weight_diff_2
        = -city_state_1_cur.total_weight
        - city_state_2_cur.total_weight
        + city_state_1.total_weight
        + city_state_2.total_weight;
    for (CityPos city_pos = city_pos_2;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        CityId city_id = solution.visit(city_pos).city_id;
        CityId city_id_next = (city_pos == instance_.number_of_cities() - 1)?
            0:
            solution.visit(city_pos + 1).city_id;
        Weight weight_cur = solution.visit(city_pos).weight_from_start + weight_diff_2;
        time_cur += instance_.duration(
                city_id,
                city_id_next,
                weight_cur);
    }
    Profit objective_new = profit_new - instance_.renting_ratio() * time_cur;
    if (objective_new > solution.objective()) {
        //std::cout << "time_cur " << time_cur << std::endl;
        //std::cout << "objective_new " << objective_new << " / " << solution.objective() << std::endl;
        //std::cout << "toto" << std::endl;
        return EvaluateChangeTwoCityStatesOutput{
            city_id_2_best,
            city_state_id_1_best,
            city_state_id_2_best};
    }

    return EvaluateChangeTwoCityStatesOutput{-1, -1, -1};
}

void EfficientLocalScheme::apply_perturbation(
        Solution& solution,
        const Perturbation&,
        std::mt19937_64& generator) const
{
    std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), 0);
    std::vector<CityId> city_ids;
    for (CityPos city_pos = 0;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        CityId city_id = solution.visit(city_pos).city_id;
        city_ids.push_back(city_id);
        city_state_ids[city_id] = solution.city(city_id).city_state_id;
    }

    // Draw 5 random cities and change their state.
    Weight weight = solution.weight();
    for (Counter c = 0; c < 5; ++c) {
        // Draw city.
        std::uniform_int_distribution<CityStateId> d_city(1, instance_.number_of_cities() - 1);
        CityId city_id = d_city(generator);

        // Draw city state.
        std::uniform_int_distribution<CityStateId> d(0, city_states_[city_id].size() - 2);
        CityStateId city_state_id = d(generator);
        if (city_state_id >= solution.city(city_id).city_state_id)
            city_state_id++;

        weight -= city_states_[city_id][city_state_ids[city_id]].total_weight;
        city_state_ids[city_id] = city_state_id;
        weight += city_states_[city_id][city_state_ids[city_id]].total_weight;
    }
    // Fix overweight.
    while (weight > instance_.capacity()) {
        // Draw city.
        std::uniform_int_distribution<CityStateId> d_city(1, instance_.number_of_cities() - 1);
        CityId city_id = d_city(generator);
        if (city_state_ids[city_id] == 0)
            continue;

        weight -= city_states_[city_id][city_state_ids[city_id]].total_weight;
        city_state_ids[city_id]--;
        weight += city_states_[city_id][city_state_ids[city_id]].total_weight;
    }

    // Draw 5 random two-opt moves.
    for (Counter c = 0; c < 5; ++c) {
        std::vector<CityPos> edges = optimizationtools::bob_floyd<CityPos>(
                2, instance_.number_of_cities() - 1, generator);
        std::sort(edges.begin(), edges.end());

        CityPos city_pos_1 = edges[0] + 1;
        CityPos city_pos_2 = edges[1] + 1;

        std::vector<CityId> city_ids_new;
        for (CityPos city_pos = 0;
                city_pos <= city_pos_1;
                ++city_pos) {
            city_ids_new.push_back(solution.visit(city_pos).city_id);
        }
        for (CityPos city_pos = city_pos_2;
                city_pos > city_pos_1;
                --city_pos) {
            city_ids_new.push_back(solution.visit(city_pos).city_id);
        }
        for (CityPos city_pos = city_pos_2 + 1;
                city_pos < instance_.number_of_cities();
                ++city_pos) {
            city_ids_new.push_back(solution.visit(city_pos).city_id);
        }
        if ((CityPos)city_ids_new.size() != instance_.number_of_cities()) {
            throw std::runtime_error(
                    "(CityPos)city_ids_new.size() != instance.number_of_cities()");
        }
        city_ids = city_ids_new;
    }

    solution = Solution(
            instance_,
            city_states_,
            city_ids,
            city_state_ids);
}

EfficientLocalScheme::Solution EfficientLocalScheme::cycle_crossover(
        const Solution& solution_parent_1,
        const Solution& solution_parent_2,
        std::mt19937_64& generator)
{
    //std::cout << "crossover" << std::endl;
    std::vector<CityId> city_ids(instance_.number_of_cities(), -1);
    std::vector<CityPos> positions(instance_.number_of_cities(), -1);
    std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), -1);

    CityPos n = 1;
    CityPos k = 0;
    city_ids[0] = 0;
    city_state_ids[0] = 0;
    std::uniform_int_distribution<CityPos> d_city(1, instance_.number_of_cities() - 1);
    while (k < instance_.number_of_cities() / 2 && n < instance_.number_of_cities()) {
        CityPos pos = d_city(generator);
        while (positions[solution_parent_2.visit(pos).city_id] != -1) {
            pos++;
            if (pos == instance_.number_of_cities())
                pos = 1;
        }
        CityPos p = pos;
        do {
            //std::cout << "p " << p << " element_id " << solution_parent_2[p].element_id << std::endl;
            CityId city_id = solution_parent_2.visit(p).city_id;
            city_ids[p] = city_id;
            city_state_ids[city_id] = solution_parent_2.city(city_id).city_state_id;
            positions[city_id] = p;
            if (city_id != solution_parent_1.visit(p).city_id)
                k++;
            p = solution_parent_2.city(solution_parent_1.visit(p).city_id).position;
            n++;
        } while (p != pos);
    }
    for (CityPos city_pos = 1;
            city_pos < instance_.number_of_cities();
            ++city_pos) {
        if (city_ids[city_pos] != -1)
            continue;
        CityId city_id = solution_parent_1.visit(city_pos).city_id;
        city_ids[city_pos] = city_id;
        city_state_ids[city_id] = solution_parent_1.city(city_id).city_state_id;
        positions[city_id] = city_pos;
    }

    Solution solution(
            instance_,
            city_states_,
            city_ids,
            city_state_ids);

    //for (CityPos city_pos = 0;
    //        city_pos < instance_.number_of_cities();
    //        ++city_pos) {
    //    std::cout << city_pos
    //        << " " << solution_parent_1.visit(city_pos).city_id
    //        << " " << solution_parent_2.visit(city_pos).city_id
    //        << " " << solution.visit(city_pos).city_id
    //        << std::endl;
    //}
    //std::cout << "profit"
    //    << " " << solution_parent_1.profit()
    //    << " " << solution_parent_2.profit()
    //    << " " << solution.profit()
    //    << std::endl;
    //std::cout << "travel_time"
    //    << " " << solution_parent_1.travel_time()
    //    << " " << solution_parent_2.travel_time()
    //    << " " << solution.travel_time()
    //    << std::endl;
    //std::cout << "objective"
    //    << " " << solution_parent_1.objective()
    //    << " " << solution_parent_2.objective()
    //    << " " << solution.objective()
    //    << std::endl;
    return solution;
}

EfficientLocalScheme::Solution EfficientLocalScheme::crossover(
        const Solution& solution_parent_1,
        const Solution& solution_parent_2,
        std::mt19937_64& generator)
{
    std::vector<CityId> city_ids(instance_.number_of_cities(), -1);
    std::vector<CityPos> positions(instance_.number_of_cities(), -1);
    std::vector<CityStateId> city_state_ids(instance_.number_of_cities(), -1);

    std::vector<CityPos> edges = optimizationtools::bob_floyd<CityPos>(
            2, instance_.number_of_cities(), generator);
    std::sort(edges.begin(), edges.end());

    CityPos pos_1 = edges[0] + 1;
    CityPos pos_2 = edges[1] + 1;

    for (CityPos pos = pos_1; pos < pos_2; ++pos) {
        CityId city_id = solution_parent_1.visit(pos).city_id;
        city_ids[pos] = city_id;
        positions[city_id] = pos;
        city_state_ids[city_id] = solution_parent_1.city(city_id).city_state_id;
    }

    city_ids[0] = 0;
    city_state_ids[0] = 0;
    CityPos p = pos_2;
    if (p == instance_.number_of_cities())
        p = 1;
    for (CityPos pos_0 = pos_2;
            pos_0 < instance_.number_of_cities() + pos_1;
            ++pos_0) {
        CityPos pos = pos_0 % (instance_.number_of_cities());
        if (pos == 0)
            continue;
        while (positions[solution_parent_2.visit(p).city_id] != -1) {
            p++;
            if (p == instance_.number_of_cities())
                p = 1;
        }
        CityId city_id = solution_parent_2.visit(p).city_id;
        city_ids[pos] = city_id;
        positions[city_id] = pos;
        city_state_ids[city_id] = solution_parent_2.city(city_id).city_state_id;
    }
    return Solution(
            instance_,
            city_states_,
            city_ids,
            city_state_ids);
}

ItemId EfficientLocalScheme::distance(
        const Solution& solution_1,
        const Solution& solution_2) const
{
    CityPos d = 0;
    for (CityId city_id = 0;
            city_id < instance_.number_of_cities();
            ++city_id) {
        CityId city_pos_1 = solution_1.city(city_id).position;
        CityId city_pos_2 = solution_2.city(city_id).position;
        if (city_pos_1 == instance_.number_of_cities() - 1
                && city_pos_2 == instance_.number_of_cities() - 1)
            continue;
        if (city_pos_1 == instance_.number_of_cities() - 1
                || city_pos_2 == instance_.number_of_cities() - 1) {
            d++;
            continue;
        }
        CityId city_id_next_1 = solution_1.visit(city_pos_1 + 1).city_id;
        CityId city_id_next_2 = solution_2.visit(city_pos_2 + 1).city_id;
        if (city_id_next_1 != city_id_next_2)
            d++;
    }
    return d;
}

EfficientLocalSearchOutput travellingthiefsolver::travellingthief::efficient_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        EfficientLocalSearchOptionalParameters parameters)
{
    init_display(instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Efficient local search" << std::endl
        << std::endl
        << "Parameters" << std::endl
        << "----------" << std::endl
        << "Neighborhoods" << std::endl
        << "    Change-city-state:           " << parameters.neighborhood_change_city_state << std::endl
        << "    Two-opt:                     " << parameters.neighborhood_two_opt << std::endl
        << "    Two-opt-change-city-states:  " << parameters.neighborhood_two_opt_change_city_states << std::endl
        << "    Shift:                       " << parameters.neighborhood_shift << std::endl
        << "    Shift-Change-city-state:     " << parameters.neighborhood_shift_change_city_state << std::endl
        << "    Shift-Change-city-state 2:   " << parameters.neighborhood_shift_change_city_state_2 << std::endl
        << "    Change-two-city-state:       " << parameters.neighborhood_change_two_city_states << std::endl
        << std::endl;

    EfficientLocalSearchOutput output(instance, parameters.info);
    auto city_states = packingwhiletravelling::compute_city_states<Instance>(instance);

    EfficientLocalScheme local_scheme(
            instance,
            city_states,
            parameters,
            output,
            generator);

    auto solution = local_scheme.empty_solution();
    if (parameters.initial_solution != nullptr
            && parameters.initial_solution->number_of_cities()
            == instance.number_of_cities()) {
        auto city_state_ids = solution2states(
                instance,
                city_states,
                *parameters.initial_solution);
        std::vector<CityId> city_ids;
        for (CityPos city_pos = 0;
                city_pos < instance.number_of_cities();
                ++city_pos) {
            CityId city_id = parameters.initial_solution->city_id(city_pos);
            city_ids.push_back(city_id);
        }
        solution = EfficientLocalSearchSolution(
                    instance,
                    city_states,
                    city_ids,
                    city_state_ids);
    } else {
        solution = local_scheme.initial_solution(0, generator);
    }
    local_scheme.local_search(solution, generator);

    std::stringstream ss;
    output.update_solution(solution.to_solution(), ss, parameters.info);

    output.algorithm_end(parameters.info);
    return output;
}

EfficientLocalSearchOutput travellingthiefsolver::travellingthief::efficient_genetic_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        EfficientLocalSearchOptionalParameters parameters)
{
    init_display(instance, parameters.info);
    parameters.info.os()
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << "Efficient genetic local search" << std::endl
        << std::endl
        << "Parameters" << std::endl
        << "----------" << std::endl
        << "Neighborhoods" << std::endl
        << "    Change-city-state:           " << parameters.neighborhood_change_city_state << std::endl
        << "    Two-opt:                     " << parameters.neighborhood_two_opt << std::endl
        << "    Two-opt-change-city-states:  " << parameters.neighborhood_two_opt_change_city_states << std::endl
        << "    Shift:                       " << parameters.neighborhood_shift << std::endl
        << "    Shift-Change-city-state:     " << parameters.neighborhood_shift_change_city_state << std::endl
        << "    Shift-Change-city-state 2:   " << parameters.neighborhood_shift_change_city_state_2 << std::endl
        << "    Change-two-city-state:       " << parameters.neighborhood_change_two_city_states << std::endl
        << std::endl;

    EfficientLocalSearchOutput output(instance, parameters.info);
    auto city_states = packingwhiletravelling::compute_city_states<Instance>(instance);

    EfficientLocalScheme local_scheme(
            instance,
            city_states,
            parameters,
            output,
            generator);

    for (Counter i = 0; i < 64; ++i)
        local_scheme.generate_initial_solutions(generator);

    localsearchsolver::GeneticLocalSearchOptionalParameters<EfficientLocalScheme> ls_parameters;
    ls_parameters.info = optimizationtools::Info(parameters.info, false, "");
    //ls_parameters.info.set_verbosity_level(1);
    ls_parameters.number_of_threads = 1;
    ls_parameters.new_solution_callback
        = [&parameters, &output](
                const EfficientLocalScheme::Solution& solution)
        {
            std::stringstream ss;
            output.update_solution(solution.to_solution(), ss, parameters.info);
        };
    localsearchsolver::genetic_local_search(local_scheme, ls_parameters);

    output.algorithm_end(parameters.info);
    return output;
}

