#include "travellingthiefsolver/thieforienteering/algorithms/local_search.hpp"

using namespace travellingthiefsolver::thieforienteering;

const Output travellingthiefsolver::thieforienteering::local_search(
        const Instance& instance,
        const Parameters& parameters)
{
    return FUNCTION_WITH_DISTANCES(
            local_search,
            instance.distances(),
            instance,
            parameters);
}
