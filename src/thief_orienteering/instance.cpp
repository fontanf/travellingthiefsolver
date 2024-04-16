#include "travellingthiefsolver/thief_orienteering/instance.hpp"

#include <iomanip>

using namespace travellingthiefsolver::thief_orienteering;

std::ostream& Instance::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of cities:  " << number_of_cities() << std::endl
            << "Number of items:   " << number_of_items() << std::endl
            << "Capacity:          " << capacity() << std::endl
            << "Minimum speed:     " << minimum_speed() << std::endl
            << "Maximum speed:     " << maximum_speed() << std::endl
            << "Time limit:        " << time_limit() << std::endl
            << "Profit sum:        " << profit_sum_ << std::endl
            << "Weight sum:        " << weight_sum_ << std::endl
            << "Weight ratio:      " << (double)weight_sum_ / capacity() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::setw(12) << "# items"
            << std::endl
            << std::setw(12) << "------"
            << std::setw(12) << "-------"
            << std::endl;
        for (CityId city_id = 0;
                city_id < number_of_cities();
                ++city_id) {
            const City& city = this->city(city_id);
            os
                << std::setw(12) << city_id
                << std::setw(12) << city.item_ids.size()
                << std::endl;
        }
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "City"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "--------"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id = 0;
                item_id < number_of_items();
                ++item_id) {
            const Item& item = this->item(item_id);
            os
                << std::setw(12) << item_id
                << std::setw(12) << item.city_id
                << std::setw(12) << item.weight
                << std::setw(12) << item.profit
                << std::endl;
        }
    }

    distances_->format(os, verbosity_level);

    return os;
}
