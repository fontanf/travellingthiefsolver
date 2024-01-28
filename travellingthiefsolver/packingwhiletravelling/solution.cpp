#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

#include "optimizationtools//utils//utils.hpp"

#include <iomanip>
#include <fstream>

using namespace travellingthiefsolver::packingwhiletravelling;

bool Solution::feasible() const
{
    return (item_weight_ <= instance().capacity());
}

nlohmann::json Solution::to_json() const
{
    return nlohmann::json {
        {"TravelTime", travel_time()},
        {"RentingCost", renting_cost()},
        {"NumberOfItems", number_of_items()},
        {"ItemWeight", item_weight()},
        {"ItemProfit", item_profit()},
        {"ObjectiveValue", objective_value()},
    };
}

std::ostream& Solution::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Travel time:      " << travel_time() << std::endl
            << "Renting cost:     " << renting_cost() << std::endl
            << "Number of items:  " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Item weight:      " << optimizationtools::Ratio<Weight>(item_weight(), instance().capacity()) << std::endl
            << "Item profit:      " << item_profit() << std::endl
            << "Feasible:         " << feasible() << std::endl
            << "Objective value:  " << objective_value() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "City"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id: item_ids_) {
            os
                << std::setw(12) << item_id
                << std::endl;
        }
    }

    return os;
}

void Solution::write(
        const std::string& certificate_path) const
{
    if (certificate_path.empty())
        return;
    std::ofstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    std::string separator = "[";
    for (ItemId item_id: item_ids_) {
        file << separator << item_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
}
