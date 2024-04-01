load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "packing_while_travelling",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
                "solution_builder.hpp",
                "utils.hpp",
                "reduction.hpp",
                "algorithm.hpp",
                "algorithm_formatter.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
                "solution_builder.cpp",
                "utils.cpp",
                "reduction.cpp",
                "algorithm.cpp",
                "algorithm_formatter.cpp",
        ],
        deps = [
                "@optimizationtools//optimizationtools/utils:utils",
                "@optimizationtools//optimizationtools/containers:containers",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//travellingthiefsolver/packing_while_travelling/algorithms:greedy",
                "//travellingthiefsolver/packing_while_travelling/algorithms:sequential_value_correction",
                "//travellingthiefsolver/packing_while_travelling/algorithms:dynamic_programming",
                "//travellingthiefsolver/packing_while_travelling/algorithms:efficient_local_search",
                "//travellingthiefsolver/packing_while_travelling/algorithms:large_neighborhood_search",
                "@boost//:program_options",
        ],
        linkopts = select({
                    "@bazel_tools//src/conditions:windows": [],
                    "//conditions:default": ["-lpthread"],
        }),
)
