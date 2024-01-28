cc_library(
        name = "tree_search",
        hdrs = ["tree_search.hpp"],
        srcs = ["tree_search.cpp"],
        deps = [
                "//travellingthiefsolver/travellingthief:travellingthief",
                "@treesearchsolver//treesearchsolver:treesearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "local_search",
        hdrs = ["local_search.hpp"],
        srcs = ["local_search.cpp"],
        deps = [
                "//travellingthiefsolver/travellingthief:travellingthief",
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "@localsearchsolver//localsearchsolver:localsearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "efficient_local_search",
        hdrs = ["efficient_local_search.hpp"],
        srcs = ["efficient_local_search.cpp"],
        deps = [
                "//travellingthiefsolver/travellingthief:travellingthief",
                "//travellingthiefsolver/packingwhiletravelling:packingwhiletravelling",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:sequential_value_correction",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:efficient_local_search",
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
                "//travellingthiefsolver/travellingthief:travellingthief",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:sequential_value_correction",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:efficient_local_search",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "iterative_tsp_pwt_ttp",
        hdrs = ["iterative_tsp_pwt_ttp.hpp"],
        srcs = ["iterative_tsp_pwt_ttp.cpp"],
        deps = [
                "//travellingthiefsolver/travellingthief:travellingthief",
                ":efficient_local_search",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:sequential_value_correction",
                "//travellingthiefsolver/packingwhiletravelling/algorithms:efficient_local_search",
        ],
        visibility = ["//visibility:public"],
)
