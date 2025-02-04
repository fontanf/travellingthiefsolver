#pragma once

#include "travellingthiefsolver/thief_orienteering/solution.hpp"

#include "travellingthiefsolver/thief_orienteering/algorithm_formatter.hpp"

#include "treesearchsolver/best_first_search.hpp"

namespace travellingthiefsolver
{
namespace thief_orienteering
{

/**
 * Tree search algorithm (dynamic programming) for the thief orienteering
 * problem..
 *
 * The algorithm is close from:
 * - "Exact Approaches for the Travelling Thief Problem" (Wu et al., 2017)
 *   https://doi.org/10.1007/978-3-319-68759-9_10
 */
const Output tree_search(
        const Instance& instance,
        const Parameters& parameters = {});

template <typename Distances>
const Output tree_search(
        const Distances& distances,
        const Instance& instance,
        const Parameters& parameters = {});

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename Distances>
class BranchingScheme
{

public:

    struct Node
    {
        /** Id of the node. */
        treesearchsolver::NodeId node_id = -1;

        /** Parent of the node. */
        std::shared_ptr<Node> parent = nullptr;

        /** Visited cities. */
        std::vector<bool> visited_cities;

        /** Last visited city. */
        CityId last_visited_city_id = -1;

        /** State of the last visited city. */
        CityStateId last_visited_city_state_id = -1;

        /** Number of visited cities. */
        CityId number_of_cities = 0;

        /** Number of collected items. */
        ItemId number_of_items = 0;

        /** Current traveled distance. */
        Distance distance = 0;

        /** Current traveled distance. */
        Distance distance_full = 0;

        /** Current traval time. */
        Time time = 0;

        /** Current traval time. */
        Time time_full = 0;

        /** Current profit. */
        Profit profit = 0;

        /** Current weight. */
        Weight weight = 0;

        /** Minimum remaining distance. */
        Distance minimum_remaining_distance = 0;

        /** Remaining profit. */
        Profit remaining_profit = 0;

        /** Remaining weight. */
        Weight remaining_weight = 0;

        /** Bound. */
        Profit bound = std::numeric_limits<Profit>::infinity();

        /** Guide value. */
        double guide = -std::numeric_limits<Profit>::infinity();

        /** City of the next child. */
        CityId next_child_city_id = 0;

        /** State of the city of the next child. */
        CityStateId next_child_city_state_id = 0;
    };

    struct Parameters
    {
    };

    BranchingScheme(
            const Instance& instance,
            const Distances& distances,
            const std::vector<std::vector<travellingthiefsolver::packing_while_travelling::CityState>>& city_states,
            const Parameters& parameters):
        instance_(instance),
        distances_(distances),
        city_states_(city_states),
        parameters_(parameters),
        closest_city_distances_(instance_.number_of_cities(), std::numeric_limits<Distance>::max())
    {
        for (CityId city_id = 0;
                city_id < instance_.number_of_cities();
                ++city_id) {
            Profit profit = city_states_[city_id].back().total_profit;
            Weight weight = city_states_[city_id].back().total_weight;
            double eff = profit / weight;
            if (best_efficiency_ < eff)
                best_efficiency_ = eff;
        }

        for (CityId city_id_1 = 0;
                city_id_1 < instance_.number_of_cities();
                ++city_id_1) {
            for (CityId city_id_2 = city_id_1 + 1;
                    city_id_2 < instance_.number_of_cities();
                    ++city_id_2) {
                Distance d = distances_.distance(city_id_1, city_id_2);
                if (closest_city_distances_[city_id_1] > d)
                    closest_city_distances_[city_id_1] = d;
                if (closest_city_distances_[city_id_2] > d)
                    closest_city_distances_[city_id_2] = d;
            }
        }
    }

    inline const std::shared_ptr<Node> root() const
    {
        auto r = std::shared_ptr<Node>(new BranchingScheme::Node());
        r->visited_cities.resize(instance_.number_of_cities(), false);
        r->visited_cities[0] = true;
        r->visited_cities[instance_.number_of_cities() - 1] = true;
        r->number_of_cities = 2;
        r->last_visited_city_id = 0;
        r->remaining_profit = 0;
        r->remaining_weight = 0;
        for (CityId city_id = 0;
                city_id < instance_.number_of_cities();
                ++city_id) {
            Profit profit = city_states_[city_id].back().total_profit;
            Weight weight = city_states_[city_id].back().total_weight;
            r->remaining_profit += profit;
            r->remaining_weight += weight;
        }
        r->minimum_remaining_distance = 0;
        for (CityId city_id = 1;
                city_id < instance_.number_of_cities();
                ++city_id) {
            r->minimum_remaining_distance += closest_city_distances_[city_id];
        }
        r->node_id = node_id_cur_;
        node_id_cur_++;
        return r;
    }

    inline std::shared_ptr<Node> next_child(
            const std::shared_ptr<Node>& parent) const
    {
        assert(!infertile(parent));
        assert(!leaf(parent));
        //std::cout << "parent id " << parent->node_id
        //    << " next_child_city_id " << parent->next_child_city_id
        //    << " next_child_city_state_id " << parent->next_child_city_state_id
        //    << std::endl;
        //std::cout << parent->bound << " " << parent->objective << std::endl;

        CityId city_id_next = parent->next_child_city_id;;
        CityStateId city_state_id_next = parent->next_child_city_state_id;

        // Update parent
        parent->next_child_city_state_id++;
        if (parent->next_child_city_state_id
                == (CityStateId)city_states_[parent->next_child_city_id].size()) {
            parent->next_child_city_id++;
            parent->next_child_city_state_id = 0;
        }

        // Check if the next city has already been visited.
        if (parent->visited_cities[city_id_next])
            return nullptr;

        // Check capacity.
        Weight weight = parent->weight
            + city_states_[city_id_next][city_state_id_next].total_weight;
        if (weight > instance_.capacity())
            return nullptr;

        // Check time limit.
        Time t = instance_.duration(
                distances_,
                parent->last_visited_city_id,
                city_id_next,
                parent->weight);
        Time t_end = instance_.duration(
                distances_,
                city_id_next,
                instance().number_of_cities() - 1,
                weight);
        if (parent->time + t + t_end > instance().time_limit())
            return nullptr;

        // Compute new child.
        auto child = std::shared_ptr<Node>(new BranchingScheme::Node());
        child->node_id = node_id_cur_;
        node_id_cur_++;
        child->parent = parent;
        child->visited_cities = parent->visited_cities;
        child->visited_cities[city_id_next] = true;
        child->last_visited_city_id = city_id_next;
        child->last_visited_city_state_id = city_state_id_next;
        child->number_of_cities = parent->number_of_cities + 1;
        child->number_of_items = parent->number_of_items
            + city_states_[city_id_next][city_state_id_next].item_ids.size();
        Distance d = distances_.distance(
                parent->last_visited_city_id,
                city_id_next);
        child->distance = parent->distance + d;
        child->time = parent->time + t;
        child->profit = parent->profit
            + city_states_[city_id_next][city_state_id_next].total_profit;
        child->weight = parent->weight
            + city_states_[city_id_next][city_state_id_next].total_weight;
        child->minimum_remaining_distance = parent->minimum_remaining_distance
            - closest_city_distances_[city_id_next];
        child->remaining_profit = parent->remaining_profit
            - city_states_[city_id_next].back().total_profit;
        child->remaining_weight = parent->remaining_weight
            - city_states_[city_id_next].back().total_weight;
        Time d_end = distances_.distance(
                city_id_next,
                instance_.number_of_cities() - 1);
        child->distance_full = child->distance + d_end;
        child->time_full = child->time + t_end;
        Weight remaining_capacity = instance_.capacity() - child->weight;
        Profit profit_bound = child->profit + std::min(
                child->remaining_profit,
                best_efficiency_ * remaining_capacity);
        child->bound = profit_bound;
        child->guide = -child->bound;
        //std::cout << "child id " << child->node_id << std::endl;
        return child;
    }

    inline bool infertile(
            const std::shared_ptr<Node>& node) const
    {
        assert(node != nullptr);
        return (node->next_child_city_id == instance_.number_of_cities());
    }

    inline bool operator()(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        if (node_1->guide != node_2->guide)
            return node_1->guide < node_2->guide;
        return node_1->node_id < node_2->node_id;
    }

    inline bool leaf(
            const std::shared_ptr<Node>& node) const
    {
        return node->number_of_cities == instance_.number_of_cities();
    }

    bool bound(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        if (node_1->bound <= node_2->profit)
            return true;
        return false;
    }

    /*
     * Solution pool.
     */

    bool better(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        return node_1->profit > node_2->profit;
    }

    bool equals(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        (void)node_1;
        (void)node_2;
        return false;
    }

    /*
     * Dominances.
     */

    inline bool comparable(
            const std::shared_ptr<Node>&) const
    {
        return true;
    }

    const Instance& instance() const { return instance_; }

    struct NodeHasher
    {
        std::hash<std::vector<bool>> hasher_cities;
        std::hash<CityId> hasher_city;

        inline bool operator()(
                const std::shared_ptr<Node>& node_1,
                const std::shared_ptr<Node>& node_2) const
        {
            if (node_1->last_visited_city_id != node_2->last_visited_city_id)
                return false;
            if (node_1->visited_cities != node_2->visited_cities)
                return false;
            return true;
        }

        inline std::size_t operator()(
                const std::shared_ptr<Node>& node) const
        {
            size_t hash = hasher_city(node->last_visited_city_id);
            optimizationtools::hash_combine(hash, hasher_cities(node->visited_cities));
            return hash;
        }
    };

    inline NodeHasher node_hasher() const { return NodeHasher(); }

    inline bool dominates(
            const std::shared_ptr<Node>& node_1,
            const std::shared_ptr<Node>& node_2) const
    {
        if (node_1->time <= node_2->time
                && node_1->profit >= node_2->profit
                && node_1->weight <= node_2->weight)
            return true;
        return false;
    }

    /*
     * Outputs.
     */

    std::string display(const std::shared_ptr<Node>& node) const
    {
        std::stringstream ss;
        ss << node->profit
            << " (t" << node->time
            << " m" << node->number_of_items
            << " w" << node->weight
            << ")";
        return ss.str();
    }

    std::ostream& print_solution(
            std::ostream& os,
            const std::shared_ptr<Node>& node) const
    {
        os << "node_id " << node->node_id << std::endl;
        for (auto node_tmp = node;
                node_tmp->parent != nullptr;
                node_tmp = node_tmp->parent) {
            os << "node_tmp"
                << " n " << node_tmp->number_of_cities
                << " m " << node_tmp->number_of_items
                << " d " << node_tmp->distance
                << " d_full " << node_tmp->distance_full
                << " t " << node_tmp->time
                << " t_full " << node_tmp->time_full
                << " w " << node_tmp->weight
                << " p " << node_tmp->profit
                << " guide " << node_tmp->guide
                << " j " << node_tmp->last_visited_city_id
                << " p1 " << node_tmp->next_child_city_id
                << " p2 " << node_tmp->next_child_city_state_id
                << std::endl;
        }
        return os;
    }

    inline void write(
            const std::shared_ptr<Node>&,
            std::string) const
    {
    }


private:

    /** Instance. */
    const Instance& instance_;

    /** Distances. */
    const Distances& distances_;

    /** City states. */
    const std::vector<std::vector<travellingthiefsolver::packing_while_travelling::CityState>>& city_states_;

    /** Parameters. */
    Parameters parameters_;

    double best_efficiency_ = 0;

    std::vector<Distance> closest_city_distances_;

    /** Current node id. */
    mutable treesearchsolver::NodeId node_id_cur_ = 0;

};

template <typename Distances>
const Output tree_search(
        const Distances& distances,
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(distances, instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Tree search");
    algorithm_formatter.print_header();

    auto city_states = packing_while_travelling::compute_city_states<Instance>(instance);
    typename BranchingScheme<Distances>::Parameters bs_parameters;
    BranchingScheme<Distances> branching_scheme(instance, distances, city_states, bs_parameters);
    treesearchsolver::BestFirstSearchParameters<BranchingScheme<Distances>> bfs_parameters;
    bfs_parameters.verbosity_level = 0;
    bfs_parameters.timer = parameters.timer;
    bfs_parameters.new_solution_callback
        = [&instance, &distances, &city_states, &algorithm_formatter](
                const treesearchsolver::Output<BranchingScheme<Distances>>& ts_output)
        {
            const auto& bfs_output = static_cast<const treesearchsolver::BestFirstSearchOutput<BranchingScheme<Distances>>&>(ts_output);
            Solution solution(distances, instance);
            auto node = bfs_output.solution_pool.best();
            std::vector<std::shared_ptr<typename BranchingScheme<Distances>::Node>> ancestors;
            for (auto node_tmp = node;
                    node_tmp->parent != nullptr;
                    node_tmp = node_tmp->parent) {
                ancestors.push_back(node_tmp);
            }
            std::reverse(ancestors.begin(), ancestors.end());
            for (auto node_tmp: ancestors) {
                CityId city_id = node_tmp->last_visited_city_id;
                CityStateId city_state_id = node_tmp->last_visited_city_state_id;
                solution.add_city(distances, city_id);
                for (ItemId item_id: city_states[city_id][city_state_id].item_ids) {
                    solution.add_item(distances, item_id);
                }
            }
            std::stringstream ss;
            ss << "node " << bfs_output.number_of_nodes;
            algorithm_formatter.update_solution(solution, ss.str());
        };
    treesearchsolver::best_first_search(branching_scheme, bfs_parameters);

    if (!parameters.timer.needs_to_end()) {
        algorithm_formatter.update_bound(
                output.solution.item_profit(),
                "tree search completed");
    }

    algorithm_formatter.end();
    return output;
}

}
}
