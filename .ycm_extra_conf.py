def Settings(**kwargs):
    return {
            'flags': [
                '-x', 'c++',
                '-Wall', '-Wextra', '-Werror',
                '-I', '.',

                '-I', './bazel-travellingthiefsolver/external/'
                'json/single_include/',

                '-I', './bazel-travellingthiefsolver/external/'
                'googletest/googletest/include/',

                '-I', './bazel-travellingthiefsolver/external/'
                'boost/',

                # optimizationtools
                '-I', './bazel-travellingthiefsolver/external/'
                # '-I', './../'
                'optimizationtools/',

                # travelingsalesmansolver
                # '-I', './bazel-travellingthiefsolver/external/'
                '-I', './../'
                'travelingsalesmansolver/',

                # knapsacksolver
                '-I', './bazel-travellingthiefsolver/external/'
                # '-I', './../'
                'knapsacksolver/',

                # localsearchsolver
                '-I', './bazel-travellingthiefsolver/external/'
                # '-I', './../'
                'localsearchsolver/',

                # treesearchsolver
                '-I', './bazel-travellingthiefsolver/external/'
                # '-I', './../'
                'treesearchsolver/',
                ],
            }
