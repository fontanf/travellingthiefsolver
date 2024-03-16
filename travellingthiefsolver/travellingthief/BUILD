load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "travellingthief",
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
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "//travellingthiefsolver/travellingwhilepacking:travellingwhilepacking",
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
                "//travellingthiefsolver/travellingthief/algorithms:tree_search",
                "//travellingthiefsolver/travellingthief/algorithms:local_search",
                "//travellingthiefsolver/travellingthief/algorithms:efficient_local_search",
                "//travellingthiefsolver/travellingthief/algorithms:iterative_tsp_pwt",
                "//travellingthiefsolver/travellingthief/algorithms:iterative_tsp_pwt_ttp",
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
                ":travellingthief",
                "@boost//:program_options",
        ],
)

cc_binary(
        name = "solution2instances",
        srcs = ["solution2instances.cpp"],
        deps = [
                ":travellingthief",
                "@boost//:program_options",
        ],
        linkopts = select({
                    "@bazel_tools//src/conditions:windows": [],
                    "//conditions:default":                 ["-lpthread"],
        }),
)

