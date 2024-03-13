#pragma once

#include "travellingthiefsolver/travellingthief/solution.hpp"

namespace travellingthiefsolver
{
namespace travellingthief
{

struct LocalSearchParameters: Parameters
{
    /** Maximum number of iterations. */
    Counter maximum_number_of_iterations = -1;

    /** Number of threads. */
    Counter number_of_threads = 1;


    virtual int format_width() const override { return 31; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Maximum number of iterations: " << maximum_number_of_iterations << std::endl
            << std::setw(width) << std::left << "Number of threads: " << number_of_threads << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"MaximumNumberOfIterations", maximum_number_of_iterations},
                {"NumberOfThreads", number_of_threads},
                });
        return json;
    }
};

/**
 * Local search algorithm for the travelling thief problem.
 *
 * The implementation is inspired by:
 * - "Investigation of the Traveling Thief Problem" (Wuijts, 2018)
 *   https://studenttheses.uu.nl/handle/20.500.12932/29179
 * - "Investigation of the traveling thief problem" (Wuijts et Thierens, 2019)
 *   https://doi.org/10.1145/3321707.3321766
 */
const Output local_search(
        const Instance& instance,
        const LocalSearchParameters& parameters = {});

}
}
