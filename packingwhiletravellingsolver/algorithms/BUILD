cc_library(
        name = "greedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//packingwhiletravellingsolver:packingwhiletravellingsolver"],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "sequential_value_correction",
        hdrs = ["sequential_value_correction.hpp"],
        srcs = ["sequential_value_correction.cpp"],
        deps = [
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
                "@knapsacksolver//knapsacksolver/algorithms:dynamic_programming_primal_dual",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "dynamic_programming",
        hdrs = ["dynamic_programming.hpp"],
        srcs = ["dynamic_programming.cpp"],
        deps = [
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "efficient_local_search",
        hdrs = ["efficient_local_search.hpp"],
        srcs = ["efficient_local_search.cpp"],
        deps = [
                ":sequential_value_correction",
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "large_neighborhood_search",
        hdrs = ["large_neighborhood_search.hpp"],
        srcs = ["large_neighborhood_search.cpp"],
        deps = [
                "//packingwhiletravellingsolver/algorithms:greedy",
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
                ":sequential_value_correction",
                "@knapsacksolver//knapsacksolver/algorithms:dynamic_programming_primal_dual",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "algorithms",
        hdrs = ["algorithms.hpp"],
        srcs = ["algorithms.cpp"],
        deps = [
                ":greedy",
                ":sequential_value_correction",
                ":dynamic_programming",
                ":efficient_local_search",
                ":large_neighborhood_search",
                "@boost//:program_options",
        ],
        visibility = ["//visibility:public"],
)

