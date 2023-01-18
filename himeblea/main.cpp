// File: subset_sum.cpp

#include <algorithm>
#include <iostream>
#include <vector>

// Data structure to represent the subset sum problem.
struct SubsetSumProblem {
    std::vector<int> set;  // The set of integers to choose from.
    int target;  // The target sum.
};

// Data structure to represent a solution to the subset sum problem.
struct SubsetSumSolution {
    std::vector<int> subset;  // The subset of integers that sum up to the target.
};

/**
 * Finds a subset of a set of integers that sums up to a target value.
 *
 * @param problem The subset sum problem.
 * @return A solution to the problem, or an empty solution if no solution exists.
 */
SubsetSumSolution findSubset(const SubsetSumProblem& problem) {
    // Implementation of the findSubset function...
}

/**
 * Solves the subset sum problem using a brute force search.
 *
 * @param problem The subset sum problem.
 * @return A solution to the problem, or an empty solution if no solution exists.
 */
SubsetSumSolution solveSubsetSumBruteForce(const SubsetSumProblem& problem) {
    // Iterate over all possible subsets of the set.
    for (int i = 0; i < (1 << problem.set.size()); i++) {
        // Construct the subset for this iteration.
        SubsetSumSolution solution;
        for (int j = 0; j < problem.set.size(); j++) {
            if (i & (1 << j)) {
                solution.subset.push_back(problem.set[j]);
            }
        }
        // Check if the subset is a valid solution.
        int sum = 0;
        for (int x : solution.subset) {
            sum += x;
        }
        if (sum == problem.target) {
            return solution;
        }
    }
// No valid solution was found.
    return {};
}

/**
 * Prints a solution to the subset sum problem to the console.
 *
 * @param solution The solution to print.
 */
void printSolution(const SubsetSumSolution& solution) {
    std::cout << "Subset: ";
    for (int x : solution.subset) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

int main() {
    // Create an example subset sum problem.
    SubsetSumProblem problem = {{3, 5, 7, 12, 14}, 21};
    // Solve the problem using the brute force search.
    SubsetSumSolution solution = solveSubsetSumBruteForce(problem);
    // Print the solution.
    if (solution.subset.empty()) {
        std::cout << "No solution found." << std::endl;
    } else {
        printSolution(solution);
    }
    return 0;
}
