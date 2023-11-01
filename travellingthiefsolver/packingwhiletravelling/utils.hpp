#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

using CityStateId = int64_t;

/**
 * Structure for a state of a city.
 */
struct CityState
{
    /** Selected items for this city state. */
    std::vector<ItemId> item_ids;

    /** Profit of this state. */
    Profit total_profit;

    /** Weight of this state. */
    Weight total_weight;

    friend std::ostream &operator<<(std::ostream &os, const CityState &state)
    {
        os << "CityState(";
        for (ItemId item_id : state.item_ids)
            os << item_id << " ";
        os << state.total_profit << " " << state.total_weight << ")";
        return os;
    }
};

template <typename Instance>
std::vector<std::vector<CityState>> compute_city_states(const Instance& instance);

template <typename Instance, typename Solution>
std::vector<CityStateId> solution2states(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        const Solution& solution);

}
}

template <typename Instance>
std::vector<std::vector<travellingthiefsolver::packingwhiletravelling::CityState>> travellingthiefsolver::packingwhiletravelling::compute_city_states(
        const Instance &instance)
{
    std::vector<std::vector<CityState>> states(instance.number_of_cities());

    for (CityId city_id = 0; city_id < instance.number_of_cities(); ++city_id) {
        const auto& city = instance.city(city_id);
        std::vector<ItemId> items_to_take;
        std::vector<CityState> l0;
        l0.emplace_back(CityState());
        for (ItemId item_id: city.item_ids) {
            const auto& item = instance.item(item_id);
            std::vector<CityState> l;
            std::vector<CityState>::iterator it = l0.begin();
            std::vector<CityState>::iterator it1 = l0.begin();
            while (it != l0.end() || it1 != l0.end()) {
                if (it1 != l0.end() && (it == l0.end() || it->total_weight > it1->total_weight + item.weight)) {
                    CityState s1 = *it1;
                    s1.item_ids.push_back(item_id);
                    s1.total_profit += item.profit;
                    s1.total_weight += item.weight;
                    if (s1.total_weight > instance.capacity()) {
                        break;
                    }
                    if (l.empty() || s1.total_profit > l.back().total_profit) {
                        if (!l.empty() && s1.total_weight == l.back().total_weight) {
                            l.back() = s1;
                        } else {
                            l.push_back(s1);
                        }
                    }
                    ++it1;
                } else {
                    assert(it != l0.end());
                    if (l.empty() || it->total_profit > l.back().total_profit) {
                        if (!l.empty() && it->total_weight == l.back().total_weight) {
                            l.back() = *it;
                        } else {
                            l.push_back(*it);
                        }
                    }
                    ++it;
                }
            }
            l0 = std::move(l);
        }
        states[city_id] = std::move(l0);
    }
    return states;
}

template <typename Instance, typename Solution>
std::vector<travellingthiefsolver::packingwhiletravelling::CityStateId> travellingthiefsolver::packingwhiletravelling::solution2states(
        const Instance& instance,
        const std::vector<std::vector<CityState>>& city_states,
        const Solution& solution)
{
    std::vector<Weight> weights(instance.number_of_cities(), 0);
    std::vector<Profit> profits(instance.number_of_cities(), 0);
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        if (solution.contains(item_id)) {
            const auto& item = instance.item(item_id);
            weights[item.city_id] += item.weight;
            profits[item.city_id] += item.profit;
        }
    }

    std::vector<CityStateId> solution_city_states(instance.number_of_cities(), 0);
    for (CityId city_id = 1;
            city_id < instance.number_of_cities();
            ++city_id) {
        for (CityStateId city_state_id = 0;
                city_state_id < (CityStateId)city_states[city_id].size();
                ++city_state_id) {
            const CityState& city_state = city_states[city_id][city_state_id];
            if (city_state.total_weight <= weights[city_id]
                    && city_state.total_profit >= profits[city_id]) {
                solution_city_states[city_id] = city_state_id;
                break;
            }
        }
    }
    return solution_city_states;
}
