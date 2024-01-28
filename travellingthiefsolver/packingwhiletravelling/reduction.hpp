#pragma once

#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packingwhiletravelling
{

/**
 * Structure passed as parameters of the reduction algorithm and the other
 * algorithm to determine whether and how to reduce.
 */
struct ReductionParameters
{
    /** Boolean indicating if the reduction should be performed. */
    bool reduce = true;

    /** Enable expensive reduction. */
    bool enable_expensive_reduction = false;

    /** Maximum number of rounds. */
    Counter maximum_number_of_rounds = 4;
};

class Reduction
{

public:

    /** Constructor. */
    Reduction(
            const Instance& instance,
            const ReductionParameters& parameters = {});

    /** Get the reduced instance. */
    const Instance& instance() const { return instance_; };

    /** Unreduce a solution of the reduced instance. */
    Solution unreduce_solution(
            const Solution& solution) const;

    /** Unreduce a bound of the reduced instance. */
    Profit unreduce_bound(
            Profit bound) const;

private:

    /*
     * Private methods
     */

    /** Remove unprofitable items. */
    bool reduce_unprofitable_items();

    /** Remove compulsory items. */
    bool reduce_compulsory_items();

    /** Apply reduction from polyakovskiy2017. */
    bool reduce_polyakovskiy2017(
        Counter maximum_number_of_rounds);

    /*
     * Private attributes
     */

    /** Original instance. */
    const Instance* original_instance_ = nullptr;

    /** Reduced instance. */
    Instance instance_;

    /** For each item, unreduction operations. */
    std::vector<ItemId> unreduction_operations_;

    /** Mandatory items (from the original instance). */
    std::vector<ItemId> mandatory_items_;

    /**
     * Profit to add to a solution of the reduced instance to get the profit of
     * the corresponding solution of the original instance.
     */
    Weight extra_profit_;

};

}
}
