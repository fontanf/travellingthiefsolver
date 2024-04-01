load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "thief_orienteering",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
                "algorithm_formatter.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
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
                "//travellingthiefsolver/thief_orienteering/algorithms:tree_search",
                "//travellingthiefsolver/thief_orienteering/algorithms:local_search",
                "@boost//:program_options",
        ],
        linkopts = select({
                    "@bazel_tools//src/conditions:windows": [],
                    "//conditions:default": ["-lpthread"],
        }),
)

cc_binary(
        name = "checker",
        srcs = ["checker.cpp"],
        deps = [
                ":thief_orienteering",
                "@boost//:program_options",
        ],
)
