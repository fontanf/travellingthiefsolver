load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "58d77fa8070e8cec2dc1ed015d66b454c8d78850",
    shallow_since = "1656350095 -0400",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    remote = "https://github.com/nelhage/rules_boost",
    commit = "e83dfef18d91a3e35c8eac9b9aeb1444473c0efd",
    shallow_since = "1671181466 +0000",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

http_archive(
    name = "json",
    build_file_content = """
cc_library(
        name = "json",
        hdrs = ["single_include/nlohmann/json.hpp"],
        visibility = ["//visibility:public"],
        strip_include_prefix = "single_include/"
)
""",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip"],
    sha256 = "87b5884741427220d3a33df1363ae0e8b898099fbc59f1c451113f6732891014",
)

git_repository(
    name = "optimizationtools",
    remote = "https://github.com/fontanf/optimizationtools.git",
    commit = "9fd467cdbcf88e0fecdb34ca2152bd7de4d34a3a",
)

local_repository(
    name = "optimizationtools_",
    path = "/home/florian/Dev/optimizationtools/",
)

git_repository(
    name = "travelingsalesmansolver",
    remote = "https://github.com/fontanf/travelingsalesmansolver.git",
    commit = "004086c1a544efea380ce5d757528eee23a73969",
)

local_repository(
    name = "travelingsalesmansolver_",
    path = "/home/florian/Dev/travelingsalesmansolver/",
    #path = "/mnt/d//Dev/fontanf/travelingsalesmansolver/",
)

git_repository(
    name = "knapsacksolver",
    remote = "https://github.com/fontanf/knapsacksolver.git",
    commit = "5eb7326256722cff7ead250168d6cd97191da078",
)

local_repository(
    name = "knapsacksolver_",
    path = "/home/florian/Dev/knapsacksolver/",
)

git_repository(
    name = "treesearchsolver",
    remote = "https://github.com/fontanf/treesearchsolver.git",
    commit = "d6fde807857e151a3f4b2ce5e7e3a833957dbef9",
)

local_repository(
    name = "treesearchsolver_",
    path = "../treesearchsolver/",
)

git_repository(
    name = "localsearchsolver",
    remote = "https://github.com/fontanf/localsearchsolver.git",
    commit = "b49b95480671a886afb8159f565c3673cce2dd03",
)

local_repository(
    name = "localsearchsolver_",
    path = "../localsearchsolver/",
)
