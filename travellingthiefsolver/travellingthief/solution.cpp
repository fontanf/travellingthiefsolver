#include "travellingthiefsolver/travellingthief/solution.hpp"

using namespace travellingthiefsolver::travellingthief;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    cities_(instance.number_of_cities()),
    items_is_selected_(instance.number_of_items(), false)
{
    // Add initial city.
    city_ids_.push_back(0);
    cities_[0].position = 0;
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

    char sep;
    file >> sep;

    std::vector<CityId> city_ids;
    CityId city_id = -1;
    for (CityId city_pos = 0;
            city_pos < instance.number_of_cities();
            ++city_pos) {
        file >> city_id >> sep;
        city_id--;
        city_ids.push_back(city_id);
    }

    ItemId item_id;
    CityId city_pos = 0;
    file >> sep;
    for (;;) {
        file >> item_id >> sep;
        if (file.eof())
            break;
        item_id--;
        const Item& item = instance.item(item_id);
        while (item.city_id != city_ids[city_pos]) {
            city_pos++;
            add_city(city_ids[city_pos]);
        }
        add_item(item_id);
    }
}

bool Solution::feasible() const
{
    return (item_weight_ <= instance().capacity())
        && (number_of_cities() == instance().number_of_cities());
}

void Solution::add_city(CityId city_id)
{
    // Check that the city has not already been visited.
    if (cities_[city_id].position >= 0) {
        throw std::runtime_error(
                "travellingthiefsolver::Solution::add_city(CityId)."
                "City " + std::to_string(city_id) + " is already in the solution.");
    }

    CityId city_id_prev = city_ids_.back();
    cities_[city_id].position = city_ids_.size();
    distance_cur_ += instance().distance(city_id_prev, city_id);
    distance_ = distance_cur_ + instance().distance(city_id, 0);
    travel_time_cur_ += instance().duration(city_id_prev, city_id, item_weight_);
    travel_time_ = travel_time_cur_ + instance().duration(city_id, 0, item_weight_);
    city_ids_.push_back(city_id);
}

void Solution::add_item(ItemId item_id)
{
    const Item& item = instance().item(item_id);

    // Check that the item is available at the last visited city.
    if (item.city_id != city_ids_.back()) {
        throw std::runtime_error("");  // TODO
    }

    // Check that the item has not already been selected.
    if (items_is_selected_[item_id]) {
        throw std::runtime_error("");  // TODO
    }

    items_is_selected_[item_id] = 1;
    item_ids_.push_back(item_id);
    item_profit_ += item.profit;
    item_weight_ += item.weight;
    CityId city_id = city_ids_.back();
    travel_time_ = travel_time_cur_ + instance().duration(city_id, 0, item_weight_);

    cities_[city_id].number_of_items++;
    cities_[city_id].profit += item.profit;
    cities_[city_id].weight += item.weight;
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
            << "Renting cost:        " << renting_cost() << std::endl
            << "Number of items:     " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Item weight:         " << optimizationtools::Ratio<Weight>(item_weight(), instance().capacity()) << std::endl
            << "Item profit:         " << item_profit() << std::endl
            << "Feasible:            " << feasible() << std::endl
            << "Objective:           " << objective() << std::endl
            ;
    }

    if (verbose >= 2) {
        os << std::endl
            << std::setw(12) << "City"
            << std::setw(12) << "# items"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::setw(12) << "Eff."
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "-------"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "----"
            << std::endl;
        for (CityId city_id: city_ids_) {
            const SolutionCity& city = cities_[city_id];
            os
                << std::setw(12) << city_id
                << std::setw(12) << city.number_of_items
                << std::setw(12) << city.weight
                << std::setw(12) << city.profit
                << std::setw(12) << city.profit / city.weight
                << std::endl;
        }
    }

    if (verbose >= 3) {
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
    for (CityId city_id: city_ids_) {
        file << separator << city_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
    separator = "[";
    for (ItemId item_id: item_ids_) {
        file << separator << item_id + 1;
        separator = ",";
    }
    file << "]" << std::endl;
}

void Solution::write_csv(std::string output_path) const
{
    if (output_path.empty())
        return;
    std::ofstream file(output_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + output_path + "\".");
    }

    file << "City,Distance,Weight,Speed" << std::endl;
    Distance distance = 0;
    Weight weight = 0;
    CityId city_id_prev = -1;
    for (CityPos city_pos = 0;
            city_pos < instance().number_of_cities();
            ++city_pos) {
        CityId city_id = this->city_id(city_pos);
        const City& city = instance().city(city_id);
        if (city_id_prev != -1)
            distance += instance().distance(city_id_prev, city_id);
        for (ItemId item_id: city.item_ids)
            if (contains(item_id))
                weight += instance().item(item_id).weight;
        double speed = instance().maximum_speed()
            - (double)(weight * (instance().maximum_speed() - instance().minimum_speed())) / instance().capacity();
        file
            << city_id << ","
            << distance << ","
            << weight << ","
            << speed << std::endl;
        city_id_prev = city_id;
    }
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
        << std::setw(12) << "Distance"
        << std::setw(12) << "Cost"
        << std::setw(12) << "# items"
        << std::setw(12) << "Profit"
        << std::setw(12) << "Objective"
        << std::setw(24) << "Comment"
        << std::endl
        << std::setw(12) << "-----"
        << std::setw(12) << "--------"
        << std::setw(12) << "----"
        << std::setw(12) << "-------"
        << std::setw(12) << "------"
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
        << std::setw(12) << solution.distance()
        << std::setw(12) << solution.renting_cost()
        << std::setw(12) << solution.number_of_items()
        << std::setw(12) << solution.item_profit()
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

    if (solution_new.feasible()
            && (!solution.feasible()
                || solution.objective() < solution_new.objective())) {
        // Update solution
        solution = solution_new;
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

void Output::update_bound(
        Profit bound_new,
        const std::stringstream& s,
        optimizationtools::Info& info)
{
    info.lock();

    if (bound > bound_new) {
        // Update solution
        bound = bound_new;
        print(info, s);

        double t = info.elapsed_time();

        info.output->number_of_bounds++;
        std::string sol_str = "Bound" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Value", bound_new);
        info.add_to_json(sol_str, "Time", t);
        info.add_to_json(sol_str, "String", s.str());
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
    info.add_to_json("Solution", "Time", time);
    if (bound != std::numeric_limits<Profit>::infinity()) {
        info.add_to_json("Bound", "Value", bound);
        info.add_to_json("Bound", "Time", time);
    }
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
