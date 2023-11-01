#include "travellingthiefsolver/travellingwhilepacking/solution.hpp"

using namespace travellingthiefsolver::travellingwhilepacking;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    cities_is_visited_(instance.number_of_cities(), false)
{
    // Add initial city.
    city_ids_.push_back(0);
    cities_is_visited_[0] = true;
    item_weight_ += instance.city(0).weight;
}

Solution::Solution(
        const Instance& instance,
        std::string certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    // TODO
}

bool Solution::feasible() const
{
    return (number_of_cities() == instance().number_of_cities());
}

void Solution::add_city(CityId city_id)
{
    // Check that the city has not already been visited.
    if (cities_is_visited_[city_id]) {
        throw std::runtime_error("");  // TODO
    }

    CityId city_id_prev = city_ids_.back();
    city_ids_.push_back(city_id);
    cities_is_visited_[city_id] = true;
    distance_cur_ += instance().distance(city_id_prev, city_id);
    distance_ = distance_cur_ + instance().distance(city_id, 0);
    travel_time_cur_ += instance().duration(city_id_prev, city_id, item_weight_);
    item_weight_ += instance().city(city_id).weight;
    travel_time_ = travel_time_cur_ + instance().duration(city_id, 0, item_weight_);
}

std::ostream& Solution::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of vertices:  " << optimizationtools::Ratio<CityId>(number_of_cities(), instance().number_of_cities()) << std::endl
            << "Distance:            " << distance() << std::endl
            << "Travel time:         " << travel_time() << std::endl
            << "Feasible:            " << feasible() << std::endl
            << "Renting cost:        " << renting_cost() << std::endl
            ;
    }

    if (verbose >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::endl
            << std::setw(12) << "----"
            << std::endl;
        for (CityId city_id: city_ids_) {
            os
                << std::setw(12) << city_id
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
    for (CityId city_id: city_ids_) {
        file << separator << city_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output::Output(
        const Instance& instance,
        optimizationtools::Info& info):
    solution(instance)
{
    info.os()
        << std::setw(12) << "T (s)"
        << std::setw(12) << "Cost"
        << std::setw(24) << "Comment"
        << std::endl
        << std::setw(12) << "-----"
        << std::setw(12) << "----"
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
        << std::setw(12) << solution.renting_cost()
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

    if (solution_new.feasible()
            && (!solution.feasible()
                || solution.renting_cost() > solution_new.renting_cost())) {
        // Update solution
        solution = solution_new;
        print(info, s);

        std::string solution_value = optimizationtools::solution_value(
                optimizationtools::ObjectiveDirection::Minimize,
                solution.feasible(),
                solution.renting_cost());
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
            optimizationtools::ObjectiveDirection::Minimize,
            solution.feasible(),
            solution.renting_cost());
    double absolute_optimality_gap = optimizationtools::absolute_optimality_gap(
            optimizationtools::ObjectiveDirection::Minimize,
            solution.feasible(),
            solution.renting_cost(),
            bound);
    double relative_optimality_gap = optimizationtools::relative_optimality_gap(
            optimizationtools::ObjectiveDirection::Minimize,
            solution.feasible(),
            solution.renting_cost(),
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
