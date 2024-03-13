import run_tests
import os


commands = run_tests.generate_commands(
        "--algorithm local-search --maximum-number-of-iterations 32",
        os.path.join(
            "test_results",
            "travellingthief",
            "local_search"),
        "int(row['Number of items']) < 99")


if __name__ == "__main__":
    for command in commands:
        run_tests.run(command)
