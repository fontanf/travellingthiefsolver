load("//travellingthiefsolver:variables.bzl", "STDCPP")

cc_library(
        name = "thieforienteeringsolver",
        hdrs = [
                "instance.hpp",
                "instance_builder.hpp",
                "solution.hpp",
        ],
        srcs = [
                "instance.cpp",
                "instance_builder.cpp",
                "solution.cpp",
        ],
        deps = [
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravellingsolver",
                "//travellingthiefsolver/travellingwhilepacking:travellingwhilepackingsolver",
                "@optimizationtools//optimizationtools/utils:info",
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
                "//travellingthiefsolver/thieforienteering/algorithms:algorithms",
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
                ":thieforienteeringsolver",
                "@boost//:program_options",
        ],
)
