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



travelling_thief_data = os.environ['TRAVELLING_THIEF_DATA']
travelling_thief_main = os.path.join(
        "install",
        "bin",
        "travellingthiefsolver_travelling_thief")


if args.tests is None or "travelling-thief-local-search" in args.tests:
    print("Travelling thief problem / local search")
    print("---------------------------------------")
    print()

    data = [
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n51_uncorr-similar-weights_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n153_uncorr-similar-weights_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_bounded-strongly-corr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_uncorr_01.ttp"),
            os.path.join("polyakovskiy2014", "berlin52-ttp", "berlin52_n255_uncorr-similar-weights_01.ttp")]
    for instance in data:
        instance_path = os.path.join(
                travelling_thief_data,
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

    data = [
            os.path.join("wu2017", "eil51_n14_m13_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n14_m13_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n14_m13_uncorr-similar-weights_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n08_m35_uncorr-similar-weights_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_multiple-strongly-corr_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_uncorr_01.ttp"),
            os.path.join("wu2017", "eil51_n05_m40_uncorr-similar-weights_01.ttp")]
    for instance in data:
        instance_path = os.path.join(
                travelling_thief_data,
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


thief_orienteering_data = os.environ['THIEF_ORIENTEERING_DATA']
thief_orienteering_main = os.path.join(
        "install",
        "bin",
        "travellingthiefsolver_thief_orienteering")


if args.tests is None or "thief-orienteering-local-search" in args.tests:
    print("Thief orienteering / local search")
    print("---------------------------------")
    print()

    data = [
            os.path.join("santos2018", "eil51-thop", "eil51_01_bsc_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_01_unc_05_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_01_usw_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_bsc_05_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_unc_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_03_usw_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_bsc_10_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_unc_01_01.thop"),
            os.path.join("santos2018", "eil51-thop", "eil51_05_usw_05_01.thop")]
    for instance in data:
        instance_path = os.path.join(
                thief_orienteering_data,
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
