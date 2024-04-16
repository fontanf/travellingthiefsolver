#pragma once

#include "travellingthiefsolver/packing_while_travelling/solution.hpp"

namespace travellingthiefsolver
{
namespace packing_while_travelling
{

class SolutionBuilder
{

public:

    /** Constructor. */
    SolutionBuilder() { }

    /** Set the instance of the solution. */
    SolutionBuilder& set_instance(const Instance& instance);

    /** Add cities. */
    void add_item(ItemId item_id);

    /** Read an solution from a file. */
    void read(std::string solution_path);

    /*
     * Build
     */

    /** Build. */
    Solution build();

private:

    /*
     * Private methods
     */

    /*
     * Private attributes
     */

    /** Solution. */
    Solution solution_;

};

}
}
