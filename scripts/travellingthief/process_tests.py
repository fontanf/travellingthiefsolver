import process_tests

process_tests.process_tests(
        os.path.join("test_results_ref", "travellingthiefsolver", "local_search"),
        os.path.join("test_results", "travellingthiefsolver", "local_search"),
        ["Value"],
        ["Time"])
