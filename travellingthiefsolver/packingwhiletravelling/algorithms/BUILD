cc_library(
        name = "greedy",
        hdrs = ["greedy.hpp"],
        srcs = ["greedy.cpp"],
        deps = ["//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling"],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "sequential_value_correction",
        hdrs = ["sequential_value_correction.hpp"],
        srcs = ["sequential_value_correction.cpp"],
        deps = [
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "@knapsacksolver//knapsacksolver/knapsack/algorithms:dynamic_programming_primal_dual",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "dynamic_programming",
        hdrs = ["dynamic_programming.hpp"],
        srcs = ["dynamic_programming.cpp"],
        deps = [
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "efficient_local_search",
        hdrs = ["efficient_local_search.hpp"],
        srcs = ["efficient_local_search.cpp"],
        deps = [
                ":sequential_value_correction",
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "large_neighborhood_search",
        hdrs = ["large_neighborhood_search.hpp"],
        srcs = ["large_neighborhood_search.cpp"],
        deps = [
                ":sequential_value_correction",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:greedy",
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "@knapsacksolver//knapsacksolver/knapsack/algorithms:dynamic_programming_primal_dual",
        ],
        visibility = ["//visibility:public"],
)
