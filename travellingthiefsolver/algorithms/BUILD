cc_library(
        name = "local_search",
        hdrs = ["local_search.hpp"],
        srcs = ["local_search.cpp"],
        deps = [
                "//travellingthiefsolver:travellingthiefsolver",
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
                "@localsearchsolver//localsearchsolver:localsearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "efficient_local_search",
        hdrs = ["efficient_local_search.hpp"],
        srcs = ["efficient_local_search.cpp"],
        deps = [
                "//travellingthiefsolver:travellingthiefsolver",
                "//packingwhiletravellingsolver:packingwhiletravellingsolver",
                "//packingwhiletravellingsolver/algorithms:sequential_value_correction",
                "//packingwhiletravellingsolver/algorithms:efficient_local_search",
                "@travelingsalesmansolver//travelingsalesmansolver/algorithms:lkh",
                "@localsearchsolver//localsearchsolver:localsearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "iterative_tsp_pwt",
        hdrs = ["iterative_tsp_pwt.hpp"],
        srcs = ["iterative_tsp_pwt.cpp"],
        deps = [
                "//travellingthiefsolver:travellingthiefsolver",
                "//packingwhiletravellingsolver/algorithms:sequential_value_correction",
                "//packingwhiletravellingsolver/algorithms:efficient_local_search",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "iterative_tsp_pwt_ttp",
        hdrs = ["iterative_tsp_pwt_ttp.hpp"],
        srcs = ["iterative_tsp_pwt_ttp.cpp"],
        deps = [
                "//travellingthiefsolver:travellingthiefsolver",
                ":efficient_local_search",
                "//packingwhiletravellingsolver/algorithms:sequential_value_correction",
                "//packingwhiletravellingsolver/algorithms:efficient_local_search",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "algorithms",
        hdrs = ["algorithms.hpp"],
        srcs = ["algorithms.cpp"],
        deps = [
                ":local_search",
                ":efficient_local_search",
                ":iterative_tsp_pwt",
                ":iterative_tsp_pwt_ttp",
                "@boost//:program_options",
        ],
        visibility = ["//visibility:public"],
)

