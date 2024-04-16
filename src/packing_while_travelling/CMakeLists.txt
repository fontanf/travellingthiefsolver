add_library(TravellingThiefSolver_packing_while_travelling)
target_sources(TravellingThiefSolver_packing_while_travelling PRIVATE
    instance.cpp
    instance_builder.cpp
    solution.cpp
    solution_builder.cpp
    utils.cpp
    reduction.cpp
    algorithm.cpp
    algorithm_formatter.cpp)
target_include_directories(TravellingThiefSolver_packing_while_travelling PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_packing_while_travelling PUBLIC
    OptimizationTools::containers
    OptimizationTools::utils)
add_library(TravellingThiefSolver::knapsack ALIAS TravellingThiefSolver_packing_while_travelling)

add_subdirectory(algorithms)

add_executable(TravellingThiefSolver_packing_while_travelling_main)
target_sources(TravellingThiefSolver_packing_while_travelling_main PRIVATE
    main.cpp)
target_link_libraries(TravellingThiefSolver_packing_while_travelling_main PUBLIC
    TravellingThiefSolver_packing_while_travelling_greedy
    TravellingThiefSolver_packing_while_travelling_sequential_value_correction
    TravellingThiefSolver_packing_while_travelling_dynamic_programming
    TravellingThiefSolver_packing_while_travelling_efficient_local_search
    TravellingThiefSolver_packing_while_travelling_large_neighborhood_search
    Boost::program_options)
set_target_properties(TravellingThiefSolver_packing_while_travelling_main PROPERTIES OUTPUT_NAME "travellingthiefsolver_packing_while_travelling")
install(TARGETS TravellingThiefSolver_packing_while_travelling_main)
