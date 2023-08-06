#include "travellingthiefsolver/utils.hpp"

#include "travelingsalesmansolver/algorithms/lkh.hpp"

using namespace travellingthiefsolver;

travelingsalesmansolver::Instance travellingthiefsolver::create_tsp_instance(
        const Instance& instance)
{
    travelingsalesmansolver::Instance tsp_instance(instance.number_of_cities());
    tsp_instance.set_edge_weight_type(instance.edge_weight_type());
    tsp_instance.set_node_coord_type(instance.node_coord_type());
    // Set coordiantes.
    for (travelingsalesmansolver::VertexId vertex_id = 0;
            vertex_id < tsp_instance.number_of_vertices();
            ++vertex_id) {
        const City& city = instance.city(vertex_id);
        tsp_instance.set_coordinates(vertex_id, city.x, city.y, city.z);
    }
    // Set distances.
    if (instance.edge_weight_type() == "EXPLICIT") {
        for (travelingsalesmansolver::VertexId vertex_id_1 = 0;
                vertex_id_1 < tsp_instance.number_of_vertices();
                ++vertex_id_1) {
            for (travelingsalesmansolver::VertexId vertex_id_2 = 0;
                    vertex_id_2 < tsp_instance.number_of_vertices();
                    ++vertex_id_2) {
                tsp_instance.set_distance(
                        vertex_id_1,
                        vertex_id_2,
                        instance.distance(vertex_id_1, vertex_id_2));
            }
        }
    }
    return tsp_instance;
}

Solution travellingthiefsolver::retrieve_solution(
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution)
{
    Solution solution(instance);
    for (CityId pos = 1; pos < instance.number_of_cities(); ++pos) {
        CityId city_id = tsp_solution.vertex_id(pos);
        solution.add_city(city_id);
    }
    return solution;
}

Solution travellingthiefsolver::retrieve_solution_2(
        const Instance& instance,
        const travelingsalesmansolver::Solution& tsp_solution)
{
    Solution solution(instance);
    for (CityId pos = instance.number_of_cities() - 1; pos > 0; --pos) {
        CityId city_id = tsp_solution.vertex_id(pos);
        solution.add_city(city_id);
    }
    return solution;
}

packingwhiletravellingsolver::Instance travellingthiefsolver::create_pwt_instance(
        const Instance& instance,
        const Solution& solution_pred)
{
    std::vector<CityId> cities_tsp2pwt(instance.number_of_cities(), -1);
    for (CityId pwt_city_id = 0;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id = solution_pred.city_id(pwt_city_id);
        cities_tsp2pwt[city_id] = pwt_city_id;
    }
    packingwhiletravellingsolver::InstanceBuilder pwt_instance_builder;
    pwt_instance_builder.add_cities(instance.number_of_cities());
    pwt_instance_builder.set_capacity(instance.capacity());
    pwt_instance_builder.set_minimum_speed(instance.minimum_speed());
    pwt_instance_builder.set_maximum_speed(instance.maximum_speed());
    pwt_instance_builder.set_renting_ratio(instance.renting_ratio());
    // Set distances.
    for (CityId pwt_city_id = 0;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id_1 = (pwt_city_id == 0)?
            solution_pred.city_id(instance.number_of_cities() - 1):
            solution_pred.city_id(pwt_city_id - 1);
        CityId city_id_2 = solution_pred.city_id(pwt_city_id);
        pwt_instance_builder.set_distance(
                pwt_city_id,
                instance.distance(city_id_1, city_id_2));
    }
    // Add items.
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        CityId pwt_city_id = cities_tsp2pwt[item.city_id];
        pwt_instance_builder.add_item(
                pwt_city_id,
                item.weight,
                item.profit);
    }
    return pwt_instance_builder.build();
}

Solution travellingthiefsolver::retrieve_solution(
        const Instance& instance,
        const Solution& solution_pred,
        const packingwhiletravellingsolver::Solution& pwt_solution)
{
    Solution solution(instance);
    for (ItemId item_id: instance.city(0).item_ids) {
        if (pwt_solution.contains(item_id))
            solution.add_item(item_id);
    }
    for (CityId pwt_city_id = 1;
            pwt_city_id < instance.number_of_cities();
            ++pwt_city_id) {
        CityId city_id = solution_pred.city_id(pwt_city_id);
        //std::cout << "pwt_city_id " << pwt_city_id
        //    << " city_id " << city_id
        //    << std::endl;
        solution.add_city(city_id);
        for (ItemId item_id: instance.city(city_id).item_ids) {
            if (pwt_solution.contains(item_id))
                solution.add_item(item_id);
        }
    }
    return solution;
}

travellingwhilepackingsolver::Instance travellingthiefsolver::create_twp_instance(
        const Instance& instance,
        const Solution& solution_pred)
{
    travellingwhilepackingsolver::Instance twp_instance(instance.distances());
    twp_instance.set_capacity(instance.capacity());
    twp_instance.set_minimum_speed(instance.minimum_speed());
    twp_instance.set_maximum_speed(instance.maximum_speed());
    twp_instance.set_renting_ratio(instance.renting_ratio());
    // Set city weights.
    std::vector<Weight> weights(instance.number_of_cities(), 0);
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id) {
        if (solution_pred.contains(item_id)) {
            const Item& item = instance.item(item_id);
            weights[item.city_id] += item.weight;
        }
    }
    for (CityId city_id = 0; city_id < instance.number_of_cities(); ++city_id)
        twp_instance.set_weight(city_id, weights[city_id]);
    if (twp_instance.total_weight() > twp_instance.capacity()) {
        throw std::runtime_error(
                "twp_instance.total_weight() > twp_instance.capacity()");
    }
    return twp_instance;
}

travellingwhilepackingsolver::Solution travellingthiefsolver::create_twp_solution(
        const travellingwhilepackingsolver::Instance& twp_instance,
        const Solution& solution)
{
    travellingwhilepackingsolver::Solution twp_solution(twp_instance);
    for (CityPos city_pos = 1;
            city_pos < twp_instance.number_of_cities();
            ++city_pos) {
        CityId city_id = solution.city_id(city_pos);
        twp_solution.add_city(city_id);
    }
    return twp_solution;
}

Solution travellingthiefsolver::retrieve_solution(
        const Instance& instance,
        const Solution& solution_pred,
        const travellingwhilepackingsolver::Solution& twp_solution)
{
    Solution solution(instance);
    for (ItemId item_id: instance.city(0).item_ids) {
        if (solution_pred.contains(item_id))
            solution.add_item(item_id);
    }
    for (CityId pos = 1;
            pos < instance.number_of_cities();
            ++pos) {
        CityId city_id = twp_solution.city_id(pos);
        solution.add_city(city_id);
        for (ItemId item_id: instance.city(city_id).item_ids) {
            if (solution_pred.contains(item_id))
                solution.add_item(item_id);
        }
    }
    //std::cout << "solution feasible " << solution.feasible()
    //    << " renting_cost " << solution.renting_cost()
    //    << " cost " << solution.objective()
    //    << std::endl;
    return solution;
}

std::vector<Solution> travellingthiefsolver::solve_tsp_lkh(
        const Instance& instance,
        const travelingsalesmansolver::Instance& tsp_instance,
        std::mt19937_64& generator,
        std::string& lkh_candidate_file_content)
{
    std::uniform_int_distribution<Counter> d_seed(1, 1e8);
    travelingsalesmansolver::LkhOptionalParameters tsp_parameters;
    if (instance.number_of_cities() < 1e4) {
        tsp_parameters.candidate_set_type = "DELAUNAY";
    } else {
        tsp_parameters.candidate_set_type = "POPMUSIC";
    }
    tsp_parameters.initial_period = "100";
    tsp_parameters.runs = "1";
    tsp_parameters.max_trials = "1";
    tsp_parameters.seed = std::to_string(d_seed(generator));
    tsp_parameters.candidate_file_content = lkh_candidate_file_content;
    auto tsp_output = travelingsalesmansolver::lkh(
            tsp_instance,
            tsp_parameters);
    //std::cout << tsp_output.solution.distance() << std::endl;
    if (lkh_candidate_file_content.empty())
        lkh_candidate_file_content = tsp_output.candidate_file_content;

    return {
        retrieve_solution(instance, tsp_output.solution),
        retrieve_solution_2(instance, tsp_output.solution)};

}