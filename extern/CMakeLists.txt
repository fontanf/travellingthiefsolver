# Enable FetchContent.
include(FetchContent)

# Fetch fontanf/knapsacksolver.
FetchContent_Declare(
    knapsacksolver
    GIT_REPOSITORY https://github.com/fontanf/knapsacksolver.git
    GIT_TAG 5115e4db582c97e2e1ade8ea3ab5e678a921b223
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../knapsacksolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(knapsacksolver)

# Fetch fontanf/treesearchsolver.
FetchContent_Declare(
    treesearchsolver
    GIT_REPOSITORY https://github.com/fontanf/treesearchsolver.git
    GIT_TAG 980bdd0d04f1a8a233a3af310d3152d812d614ce
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../treesearchsolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(treesearchsolver)

# Fetch fontanf/localsearchsolver.
FetchContent_Declare(
    localsearchsolver
    GIT_REPOSITORY https://github.com/fontanf/localsearchsolver.git
    GIT_TAG f7110fdd15fa1b9001006bfbf7a8ab53f925016d
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../localsearchsolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(localsearchsolver)
