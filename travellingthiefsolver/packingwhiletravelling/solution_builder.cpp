#include "travellingthiefsolver/packingwhiletravelling/solution_builder.hpp"

#include <fstream>

using namespace travellingthiefsolver::packingwhiletravelling;

SolutionBuilder& SolutionBuilder::set_instance(const Instance& instance)
{
    solution_.instance_ = &instance;
    solution_.items_is_selected_ = std::vector<uint8_t>(instance.number_of_items(), 0);
    return *this;
}

void SolutionBuilder::add_item(ItemId item_id)
{
    solution_.items_is_selected_[item_id] = 1;
}

void SolutionBuilder::read(std::string solution_path)
{
    std::ifstream file(solution_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + solution_path + "\".");
    }

    // TODO

    file.close();
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Build /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Solution SolutionBuilder::build()
{
    const Instance& instance = solution_.instance();
    solution_.item_weight_ += instance.city(0).weight;
    for (ItemId item_id: instance.city(0).item_ids) {
        if (solution_.items_is_selected_[item_id]) {
            const Item& item = instance.item(item_id);
            solution_.item_weight_ += item.weight;
            solution_.item_profit_ += item.profit;
            solution_.item_ids_.push_back(item_id);
        }
    }
    for (CityId city_id = 1; city_id < instance.number_of_cities(); ++city_id) {
        solution_.travel_time_ += instance.duration(city_id, solution_.item_weight_);
        solution_.item_weight_ += instance.city(city_id).weight;
        for (ItemId item_id: instance.city(city_id).item_ids) {
            if (solution_.items_is_selected_[item_id]) {
                const Item& item = instance.item(item_id);
                solution_.item_weight_ += item.weight;
                solution_.item_profit_ += item.profit;
                solution_.item_ids_.push_back(item_id);
            }
        }
    }
    solution_.travel_time_ += instance.duration(0, solution_.item_weight_);
    return std::move(solution_);
}
