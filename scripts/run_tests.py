import argparse
import sys
import os

parser = argparse.ArgumentParser(description='')
parser.add_argument('directory')
parser.add_argument(
        "-t", "--tests",
        type=str,
        nargs='*',
        help='')

args = parser.parse_args()



travelling_thief_main = os.path.join(
        "bazel-bin",
        "travellingthiefsolver",
        "travelling_thief",
        "main")


if args.tests is None or "travelling-thief-local-search" in args.tests:
    print("Travelling thief problem / local search")
    print("---------------------------------------")
    print()

    travelling_thief_local_search_data = [
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_uncorr-similar-weights_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_uncorr-similar-weights_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_uncorr-similar-weights_01.ttp")]
    for instance in travelling_thief_local_search_data:
        instance_path = os.path.join(
                "data",
                "travelling_thief",
                instance)
        json_output_path = os.path.join(
                args.directory,
                "travelling_thief",
                "local_search",
                instance + ".json")
        if not os.path.exists(os.path.dirname(json_output_path)):
            os.makedirs(os.path.dirname(json_output_path))
        command = (
                travelling_thief_main
                + "  --verbosity-level 1"
                + "  --input \"" + instance_path + "\""
                + "  --algorithm local-search"
                + " --maximum-number-of-iterations 32"
                + "  --output \"" + json_output_path + "\"")
        print(command)
        os.system(command)
        print()
    print()
    print()


if args.tests is None or "travelling-thief-tree-search" in args.tests:
    print("Travelling thief problem / tree search")
    print("--------------------------------------")
    print()

    travelling_thief_tree_search_data = [
            os.path.join("wu2017", "eil51_n14_m13_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n14_m13_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n14_m13_uncorr-similar-weights_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_uncorr-similar-weights_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_uncorr-similar-weights_01.ttp")]
    for instance in travelling_thief_tree_search_data:
        instance_path = os.path.join(
                "data",
                "travelling_thief",
                instance)
        json_output_path = os.path.join(
                args.directory,
                "travelling_thief",
                "local_search",
                instance + ".json")
        if not os.path.exists(os.path.dirname(json_output_path)):
            os.makedirs(os.path.dirname(json_output_path))
        command = (
                travelling_thief_main
                + "  --verbosity-level 1"
                + "  --input \"" + instance_path + "\""
                + "  --algorithm tree-search"
                + "  --output \"" + json_output_path + "\"")
        print(command)
        os.system(command)
        print()
    print()
    print()


thief_orienteering_main = os.path.join(
        "bazel-bin",
        "travellingthiefsolver",
        "thief_orienteering",
        "main")


if args.tests is None or "thief-orienteering-local-search" in args.tests:
    print("Thief orienteering / local search")
    print("---------------------------------")
    print()

    thief_orienteering_local_search_data = [
            os.path.join("santos2018", "eil51-thop", "eil51_01_bsc_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_01_unc_05_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_01_usw_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_bsc_05_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_unc_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_usw_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_bsc_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_unc_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_usw_05_01.thop")]
    for instance in thief_orienteering_local_search_data:
        instance_path = os.path.join(
                "data",
                "thief_orienteering",
                instance)
        json_output_path = os.path.join(
                args.directory,
                "thief_orienteering",
                "local_search",
                instance + ".json")
        if not os.path.exists(os.path.dirname(json_output_path)):
            os.makedirs(os.path.dirname(json_output_path))
        command = (
                thief_orienteering_main
                + "  --verbosity-level 1"
                + "  --input \"" + instance_path + "\""
                + "  --algorithm local-search"
                + " --maximum-number-of-nodes 32"
                + "  --output \"" + json_output_path + "\"")
        print(command)
        os.system(command)
        print()
    print()
    print()
