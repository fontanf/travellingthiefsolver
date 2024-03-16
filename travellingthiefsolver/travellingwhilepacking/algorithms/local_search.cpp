#include "travellingthiefsolver/travellingwhilepacking/algorithms/local_search.hpp"

using namespace travellingthiefsolver::travellingwhilepacking;

const Output travellingthiefsolver::travellingwhilepacking::local_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
