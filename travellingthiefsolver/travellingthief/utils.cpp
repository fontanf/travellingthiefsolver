#include "travellingthiefsolver/travellingthief/utils.hpp"

#include "travellingthiefsolver/travellingwhilepacking/instance_builder.hpp"

using namespace travellingthiefsolver::travellingthief;

travelingsalesmansolver::Instance travellingthiefsolver::travellingthief::create_tsp_instance(
        const Instance& instance)
{
    return travelingsalesmansolver::Instance(instance.distances_ptr());
}

travellingthiefsolver::travellingwhilepacking::Instance travellingthiefsolver::travellingthief::create_twp_instance(
        const Instance& instance,
        const Solution& solution_pred)
{
    travellingwhilepacking::InstanceBuilder twp_instance_builder;
    twp_instance_builder.set_distances(instance.distances_ptr());
    twp_instance_builder.set_capacity(instance.capacity());
    twp_instance_builder.set_minimum_speed(instance.minimum_speed());
    twp_instance_builder.set_maximum_speed(instance.maximum_speed());
    twp_instance_builder.set_renting_ratio(instance.renting_ratio());
    // Set city weights.
    std::vector<Weight> weights(instance.number_of_cities(), 0);
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        if (solution_pred.contains(item_id)) {
            const Item& item = instance.item(item_id);
            weights[item.city_id] += item.weight;
        }
    }
    for (CityId city_id = 0; city_id < instance.number_of_cities(); ++city_id)
        twp_instance_builder.set_weight(city_id, weights[city_id]);
    const travellingwhilepacking::Instance twp_instance = twp_instance_builder.build();

    if (twp_instance.total_weight() > twp_instance.capacity()) {
        throw std::runtime_error(
                "twp_instance.total_weight() > twp_instance.capacity()");
    }
    return twp_instance;
}
