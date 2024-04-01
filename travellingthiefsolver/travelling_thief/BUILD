load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "travelling_thief",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
                "utils.hpp",
                "algorithm_formatter.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
                "utils.cpp",
                "algorithm_formatter.cpp",
        ],
        deps = [
                "//travellingthiefsolver/packing_while_travelling:packing_while_travelling",
                "//travellingthiefsolver/travelling_while_packing:travelling_while_packing",
                "@optimizationtools//optimizationtools/utils:utils",
                "@optimizationtools//optimizationtools/containers:containers",
                "@travelingsalesmansolver//travelingsalesmansolver/distances:distances",
                "@travelingsalesmansolver//travelingsalesmansolver:travelingsalesmansolver",
                "@travelingsalesmansolver//travelingsalesmansolver/algorithms:lkh",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//travellingthiefsolver/travelling_thief/algorithms:tree_search",
                "//travellingthiefsolver/travelling_thief/algorithms:local_search",
                "//travellingthiefsolver/travelling_thief/algorithms:efficient_local_search",
                "//travellingthiefsolver/travelling_thief/algorithms:iterative_tsp_pwt",
                "//travellingthiefsolver/travelling_thief/algorithms:iterative_tsp_pwt_ttp",
                "@boost//:program_options",
        ],
        linkopts = select({
                    "@bazel_tools//src/conditions:windows": [],
                    "//conditions:default":                 ["-lpthread"],
        }),
)

cc_binary(
        name = "checker",
        srcs = ["checker.cpp"],
        deps = [
                ":travelling_thief",
                "@boost//:program_options",
        ],
)

cc_binary(
        name = "solution2instances",
        srcs = ["solution2instances.cpp"],
        deps = [
                ":travelling_thief",
                "@boost//:program_options",
        ],
        linkopts = select({
                    "@bazel_tools//src/conditions:windows": [],
                    "//conditions:default":                 ["-lpthread"],
        }),
)

