#include "travellingthiefsolver/travellingthief/algorithms/efficient_local_search.hpp"

using namespace travellingthiefsolver::travellingthief;

const EfficientLocalSearchOutput travellingthiefsolver::travellingthief::efficient_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        const EfficientLocalSearchParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            efficient_local_search,
            instance.distances(),
            instance,
            generator,
            parameters);
}

const EfficientLocalSearchOutput travellingthiefsolver::travellingthief::efficient_genetic_local_search(
        const Instance& instance,
        std::mt19937_64& generator,
        const EfficientLocalSearchParameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            efficient_genetic_local_search,
            instance.distances(),
            instance,
            generator,
            parameters);
}
