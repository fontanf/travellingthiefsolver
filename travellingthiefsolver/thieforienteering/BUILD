load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "thieforienteering",
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
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "//travellingthiefsolver/travellingwhilepacking:travellingwhilepacking",
                "@optimizationtools//optimizationtools/utils:utils",
                "@optimizationtools//optimizationtools/containers:containers",
                "@travelingsalesmansolver//travelingsalesmansolver:travelingsalesmansolver",
                "@travelingsalesmansolver//travelingsalesmansolver/algorithms:lkh",
        ],
        visibility = ["//visibility:public"],
)

cc_binary(
        name = "main",
        srcs = ["main.cpp"],
        deps = [
                "//travellingthiefsolver/thieforienteering/algorithms:tree_search",
                "//travellingthiefsolver/thieforienteering/algorithms:local_search",
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
                ":thieforienteering",
                "@boost//:program_options",
        ],
)
