# Enable FetchContent.
include(FetchContent)

# Fetch boost.
if(TRAVELLINGTHIEFSOLVER_BUILD_MAIN OR TRAVELLINGTHIEFSOLVER_BUILD_TEST)
    set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options dynamic_bitset)
    set(BOOST_ENABLE_CMAKE ON)
    FetchContent_Declare(
            Boost
            URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
            EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(Boost)
endif()

# Fetch googletest.
if(TRAVELLINGTHIEFSOLVER_BUILD_TEST)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF)
    FetchContent_MakeAvailable(googletest)
endif()

# Fetch fontanf/knapsacksolver.
set(KNAPSACKSOLVER_BUILD_MAIN OFF)
set(KNAPSACKSOLVER_BUILD_TEST OFF)
FetchContent_Declare(
    knapsacksolver
    GIT_REPOSITORY https://github.com/fontanf/knapsacksolver.git
    GIT_TAG 1d7a689f88e953c099087a31f174cb6646002f9f
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../knapsacksolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(knapsacksolver)

# Fetch fontanf/travelingsalesmansolver.
set(TRAVELINGSALESMANSOLVER_BUILD_MAIN OFF)
set(TRAVELINGSALESMANSOLVER_BUILD_TEST OFF)
FetchContent_Declare(
    travelingsalesmansolver
    GIT_REPOSITORY https://github.com/fontanf/travelingsalesmansolver.git
    GIT_TAG ae5c463d52b1bc4f7deb89c7bf815373f047c56e
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../travelingsalesmansolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(travelingsalesmansolver)

# Fetch fontanf/treesearchsolver.
set(TREESEARCHSOLVER_BUILD_TEST OFF)
set(TREESEARCHSOLVER_BUILD_EXAMPLES OFF)
FetchContent_Declare(
    treesearchsolver
    GIT_REPOSITORY https://github.com/fontanf/treesearchsolver.git
    GIT_TAG 61ddb8cdd5a4b278e0bcbd327f098e015b682fa4
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../treesearchsolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(treesearchsolver)

# Fetch fontanf/localsearchsolver.
set(LOCALSEARCHSOLVER_BUILD_TEST OFF)
set(LOCALSEARCHSOLVER_BUILD_EXAMPLES OFF)
FetchContent_Declare(
    localsearchsolver
    GIT_REPOSITORY https://github.com/fontanf/localsearchsolver.git
    GIT_TAG 6cc833ea92eaf8bcac0e5e0c6119a0c936d58457
    #SOURCE_DIR "${PROJECT_SOURCE_DIR}/../localsearchsolver/"
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(localsearchsolver)
