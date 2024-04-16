# Enable FetchContent.
include(FetchContent)

# Fetch fontanf/knapsacksolver.
FetchContent_Declare(
    knapsacksolver
    GIT_REPOSITORY https://github.com/fontanf/knapsacksolver.git
    GIT_TAG abbe052b56c6e985112cda23abda26f1eb0e43ce)
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../knapsacksolver/")
FetchContent_MakeAvailable(knapsacksolver)

# Fetch fontanf/treesearchsolver.
FetchContent_Declare(
    treesearchsolver
    GIT_REPOSITORY https://github.com/fontanf/treesearchsolver.git
    GIT_TAG dca5b35d5cb6de3443b9ab08175f931b2df29b6b)
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../treesearchsolver/")
FetchContent_MakeAvailable(treesearchsolver)

# Fetch fontanf/localsearchsolver.
FetchContent_Declare(
    localsearchsolver
    GIT_REPOSITORY https://github.com/fontanf/localsearchsolver.git
    GIT_TAG ecb15ef667d107f25ea51d056368f9c4e15545e2)
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../localsearchsolver/")
FetchContent_MakeAvailable(localsearchsolver)
