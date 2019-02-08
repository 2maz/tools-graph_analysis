#ifndef GRAPH_ANALYSIS_TEST_UTILS
#define GRAPH_ANALYSIS_TEST_UTILS

#include <string>
#include <vector>
#include <graph_analysis/algorithms/LPSolver.hpp>


extern std::vector<graph_analysis::algorithms::LPSolver::Type> solverTypes;

/**
 * Retrieve the root directory by analysing the current execution path:
 * returns /../graph_analysis/ as path
 */
std::string getRootDir();
#endif
