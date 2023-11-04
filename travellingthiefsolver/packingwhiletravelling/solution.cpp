#include "travellingthiefsolver/packingwhiletravelling/solution.hpp"

#include "travellingthiefsolver/packingwhiletravelling/solution_builder.hpp"

using namespace travellingthiefsolver::packingwhiletravelling;

void Solution::update(const Solution& solution)
{
    if (&instance() != &solution.instance()
            && &instance() != solution.instance().original_instance()) {
        throw std::runtime_error(
                "Cannot update a solution with a solution from a different instance.");
    }

    if (solution.instance().is_reduced()
            && solution.instance().original_instance() == &instance()) {
        SolutionBuilder solution_builder(instance());
        for (ItemId item_id: solution.instance().unreduction_info().mandatory_items) {
            solution_builder.add_item(item_id);
        }
        for (ItemId item_id = 0;
                item_id < solution.instance().number_of_items();
                ++item_id) {
            if (solution.contains(item_id)) {
                ItemId item_id_2 = solution.instance().unreduction_info().unreduction_operations[item_id];
                solution_builder.add_item(item_id_2);
            }
        }
        *this = solution_builder.build();
        if (item_profit() != solution.item_profit() + solution.instance().unreduction_info().extra_profit) {
            throw std::runtime_error(
                    "Wrong profit after unreduction. Weight: "
                    + std::to_string(item_profit())
                    + "; reduced solution profit: "
                    + std::to_string(solution.item_profit())
                    + "; extra profit: "
                    + std::to_string(solution.instance().unreduction_info().extra_profit)
                    + ".");
        }
    } else {
        *this = solution;
    }
}

bool Solution::feasible() const
{
    return (item_weight_ <= instance().capacity());
}

std::ostream& Solution::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Travel time:      " << travel_time() << std::endl
            << "Renting cost:     " << renting_cost() << std::endl
            << "Number of items:  " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Item weight:      " << optimizationtools::Ratio<Weight>(item_weight(), instance().capacity()) << std::endl
            << "Item profit:      " << item_profit() << std::endl
            << "Feasible:         " << feasible() << std::endl
            << "Objective:        " << objective() << std::endl
            ;
    }

    if (verbose >= 2) {
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

void Solution::write(std::string certificate_path) const
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

bool Solution::is_strictly_better_than(const Solution& solution) const
{
    if (!feasible())
        return false;
    if (!solution.feasible())
        return true;
    Profit p1 = objective();
    if (instance().is_reduced())
        p1 += instance().unreduction_info().extra_profit;
    Profit p2 = solution.objective();
    //if (instance().is_reduced())
    //    p2 += solution.instance().unreduction_info().extra_profit;
    return p1 > p2;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output::Output(
        const Instance& instance,
        optimizationtools::Info& info):
    solution(SolutionBuilder(instance).build())
{
    info.os()
        << std::setw(12) << "T (s)"
        << std::setw(12) << "# items"
        << std::setw(12) << "Profit"
        << std::setw(12) << "Cost"
        << std::setw(12) << "Objective"
        << std::setw(24) << "Comment"
        << std::endl
        << std::setw(12) << "-----"
        << std::setw(12) << "-------"
        << std::setw(12) << "------"
        << std::setw(12) << "----"
        << std::setw(12) << "---------"
        << std::setw(24) << "-------"
        << std::endl;
    print(info, std::stringstream(""));
}

void Output::print(
        optimizationtools::Info& info,
        const std::stringstream& s) const
{
    double t = info.elapsed_time();
    std::streamsize precision = std::cout.precision();

    info.os()
        << std::setw(12) << std::fixed << std::setprecision(3) << t << std::defaultfloat << std::setprecision(precision)
        << std::setw(12) << solution.number_of_items()
        << std::setw(12) << solution.item_profit()
        << std::setw(12) << solution.renting_cost()
        << std::setw(12) << solution.objective()
        << std::setw(24) << s.str()
        << std::endl;

    if (!info.output->only_write_at_the_end)
        info.write_json_output();
}

void Output::update_solution(
        const Solution& solution_new,
        const std::stringstream& s,
        optimizationtools::Info& info)
{
    info.lock();

    if (solution_new.is_strictly_better_than(solution)) {
        solution.update(solution_new);
        print(info, s);

        std::string solution_value = optimizationtools::solution_value(
                optimizationtools::ObjectiveDirection::Maximize,
                solution.feasible(),
                solution.objective());
        double t = info.elapsed_time();

        info.output->number_of_solutions++;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Value", solution_value);
        info.add_to_json(sol_str, "Time", t);
        info.add_to_json(sol_str, "String", s.str());
        if (!info.output->only_write_at_the_end) {
            info.write_json_output();
            solution.write(info.output->certificate_path);
        }
    }

    info.unlock();
}

Output& Output::algorithm_end(optimizationtools::Info& info)
{
    std::string solution_value = optimizationtools::solution_value(
            optimizationtools::ObjectiveDirection::Maximize,
            solution.feasible(),
            solution.objective());
    double absolute_optimality_gap = optimizationtools::absolute_optimality_gap(
            optimizationtools::ObjectiveDirection::Maximize,
            solution.feasible(),
            solution.objective(),
            bound);
    double relative_optimality_gap = optimizationtools::relative_optimality_gap(
            optimizationtools::ObjectiveDirection::Maximize,
            solution.feasible(),
            solution.objective(),
            bound);
    time = info.elapsed_time();

    info.add_to_json("Solution", "Value", solution_value);
    info.add_to_json("Bound", "Value", bound);
    info.add_to_json("Solution", "Time", time);
    info.add_to_json("Bound", "Time", time);
    info.os()
        << std::endl
        << "Final statistics" << std::endl
        << "----------------" << std::endl
        << "Value:                        " << solution_value << std::endl
        << "Bound:                        " << bound << std::endl
        << "Absolute optimality gap:      " << absolute_optimality_gap << std::endl
        << "Relative optimality gap (%):  " << relative_optimality_gap * 100 << std::endl
        << "Time (s):                     " << time << std::endl
        ;
    print_statistics(info);
    info.os() << std::endl
        << "Solution" << std::endl
        << "--------" << std::endl ;
    solution.print(info.os(), info.verbosity_level());

    info.write_json_output();
    solution.write(info.output->certificate_path);
    return *this;
}
