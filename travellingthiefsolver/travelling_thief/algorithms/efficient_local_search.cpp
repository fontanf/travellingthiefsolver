#include "travellingthiefsolver/travelling_thief/algorithms/efficient_local_search.hpp"

using namespace travellingthiefsolver::travelling_thief;

const EfficientLocalSearchOutput travellingthiefsolver::travelling_thief::efficient_local_search(
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

const EfficientLocalSearchOutput travellingthiefsolver::travelling_thief::efficient_genetic_local_search(
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
