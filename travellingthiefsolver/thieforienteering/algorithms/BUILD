cc_library(
        name = "tree_search",
        hdrs = ["tree_search.hpp"],
        srcs = ["tree_search.cpp"],
        deps = [
                "//travellingthiefsolver/thieforienteering:thieforienteering",
                "@treesearchsolver//treesearchsolver:treesearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "local_search",
        hdrs = ["local_search.hpp"],
        srcs = ["local_search.cpp"],
        deps = [
                "//travellingthiefsolver/thieforienteering:thieforienteering",
                "@localsearchsolver//localsearchsolver:localsearchsolver",
        ],
        visibility = ["//visibility:public"],
)
