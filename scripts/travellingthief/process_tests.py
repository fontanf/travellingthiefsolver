import process_tests

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_cij"),
        os.path.join("test_results/", "greedy_cij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_wij"),
        os.path.join("test_results/", "greedy_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_cij_wij"),
        os.path.join("test_results/", "greedy_cij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_pij_wij"),
        os.path.join("test_results/", "greedy_pij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_wij_ti"),
        os.path.join("test_results/", "greedy_wij_ti"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_regret_cij"),
        os.path.join("test_results/", "greedy_regret_cij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_regret_wij"),
        os.path.join("test_results/", "greedy_regret_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_regret_cij_wij"),
        os.path.join("test_results/", "greedy_regret_cij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_regret_pij_wij"),
        os.path.join("test_results/", "greedy_regret_pij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "greedy_regret_wij_ti"),
        os.path.join("test_results/", "greedy_regret_wij_ti"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_cij"),
        os.path.join("test_results/", "mthg_cij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_wij"),
        os.path.join("test_results/", "mthg_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_cij_wij"),
        os.path.join("test_results/", "mthg_cij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_pij_wij"),
        os.path.join("test_results/", "mthg_pij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_wij_ti"),
        os.path.join("test_results/", "mthg_wij_ti"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_regret_cij"),
        os.path.join("test_results/", "mthg_regret_cij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_regret_wij"),
        os.path.join("test_results/", "mthg_regret_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_regret_cij_wij"),
        os.path.join("test_results/", "mthg_regret_cij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_regret_pij_wij"),
        os.path.join("test_results/", "mthg_regret_pij_wij"),
        ["Value"],
        ["Time"])

process_tests.process_tests(
        os.path.join("test_results_ref", "mthg_regret_wij_ti"),
        os.path.join("test_results/", "mthg_regret_wij_ti"),
        ["Value"],
        ["Time"])


process_tests.process_tests(
        os.path.join("test_results_ref", "milp_cbc"),
        os.path.join("test_results/", "milp_cbc"),
        ["Value"],
        ["Time"])


process_tests.process_tests(
        os.path.join("test_results_ref", "column_generation_heuristic_greedy"),
        os.path.join("test_results/", "column_generation_heuristic_greedy"),
        ["Value"],
        ["Time"])


process_tests.process_tests(
        os.path.join("test_results_ref", "local_search"),
        os.path.join("test_results/", "local_search"),
        ["Value"],
        ["Time"])
