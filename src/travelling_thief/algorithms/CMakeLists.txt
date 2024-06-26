add_library(TravellingThiefSolver_travelling_thief_tree_search)
target_sources(TravellingThiefSolver_travelling_thief_tree_search PRIVATE
    tree_search.cpp)
target_include_directories(TravellingThiefSolver_travelling_thief_tree_search PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_travelling_thief_tree_search PUBLIC
    TravellingThiefSolver_travelling_thief
    TreeSearchSolver::treesearchsolver)
add_library(TravellingThiefSolver::travelling_thief::tree_search ALIAS TravellingThiefSolver_travelling_thief_tree_search)

add_library(TravellingThiefSolver_travelling_thief_local_search)
target_sources(TravellingThiefSolver_travelling_thief_local_search PRIVATE
    local_search.cpp)
target_include_directories(TravellingThiefSolver_travelling_thief_local_search PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_travelling_thief_local_search PUBLIC
    TravellingThiefSolver_travelling_thief
    TravellingThiefSolver_packing_while_travelling
    LocalSearchSolver::localsearchsolver)
add_library(TravellingThiefSolver::travelling_thief::local_search ALIAS TravellingThiefSolver_travelling_thief_local_search)

add_library(TravellingThiefSolver_travelling_thief_efficient_local_search)
target_sources(TravellingThiefSolver_travelling_thief_efficient_local_search PRIVATE
    efficient_local_search.cpp)
target_include_directories(TravellingThiefSolver_travelling_thief_efficient_local_search PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_travelling_thief_efficient_local_search PUBLIC
    TravellingThiefSolver_travelling_thief
    TravellingThiefSolver_packing_while_travelling
    TravellingThiefSolver_packing_while_travelling_sequential_value_correction
    TravellingThiefSolver_packing_while_travelling_efficient_local_search
    TravelingSalesmanSolver::lkh 
    LocalSearchSolver::localsearchsolver)
add_library(TravellingThiefSolver::travelling_thief::efficient_local_search ALIAS TravellingThiefSolver_travelling_thief_efficient_local_search)

add_library(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt)
target_sources(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt PRIVATE
    iterative_tsp_pwt.cpp)
target_include_directories(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt PUBLIC
    TravellingThiefSolver_travelling_thief
    TravellingThiefSolver_packing_while_travelling_sequential_value_correction
    TravellingThiefSolver_packing_while_travelling_efficient_local_search)
add_library(TravellingThiefSolver::travelling_thief::iterative_tsp_pwt ALIAS TravellingThiefSolver_travelling_thief_iterative_tsp_pwt)

add_library(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt_ttp)
target_sources(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt_ttp PRIVATE
    iterative_tsp_pwt_ttp.cpp)
target_include_directories(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt_ttp PUBLIC
    ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(TravellingThiefSolver_travelling_thief_iterative_tsp_pwt_ttp PUBLIC
    TravellingThiefSolver_travelling_thief
    TravellingThiefSolver_travelling_thief_efficient_local_search
    TravellingThiefSolver_packing_while_travelling_sequential_value_correction
    TravellingThiefSolver_packing_while_travelling_efficient_local_search)
add_library(TravellingThiefSolver::travelling_thief::iterative_tsp_pwt_ttp ALIAS TravellingThiefSolver_travelling_thief_iterative_tsp_pwt_ttp)
