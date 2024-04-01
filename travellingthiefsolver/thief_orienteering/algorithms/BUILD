cc_library(
        name = "tree_search",
        hdrs = ["tree_search.hpp"],
        srcs = ["tree_search.cpp"],
        deps = [
                "//travellingthiefsolver/thief_orienteering:thief_orienteering",
                "@treesearchsolver//treesearchsolver:treesearchsolver",
        ],
        visibility = ["//visibility:public"],
)

cc_library(
        name = "local_search",
        hdrs = ["local_search.hpp"],
        srcs = ["local_search.cpp"],
        deps = [
                "//travellingthiefsolver/thief_orienteering:thief_orienteering",
                "@localsearchsolver//localsearchsolver:localsearchsolver",
        ],
        visibility = ["//visibility:public"],
)
