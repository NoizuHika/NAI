#include <iostream>
#include <vector>
#include <functional>
#include <random>
std::random_device rd;
std::mt19937 mt_generator(rd());

using chromosome_t = std::vector<int>;
using population_t = std::vector<chromosome_t>;

population_t generate_population(int size) {
    using namespace std;
    population_t pop;

    for (int i = 0; i < size; ++i) {
        chromosome_t ch;

        for (int j = 0; j < size ; ++j) {
            uniform_int_distribution<int> uni(0, 1);
            ch.push_back(uni(mt_generator));
        }
        pop.push_back(ch);
    }
    return pop;
}

auto decodeGreyCode = [](const chromosome_t& chromosome) {
    chromosome_t decoded;
    int x = 0, y = 0;
    for (int i = 0; i < chromosome.size()/2; i++) {
        x += chromosome[i] * std::pow(2,chromosome.size()/2-i-1);
    }
    for (int i = chromosome.size()/2; i < chromosome.size(); i++) {
        y += chromosome[i] * std::pow(2,chromosome.size()-i-1);
    }
    decoded.push_back(x);
    decoded.push_back(y);
    return decoded;
};

double Ackley(double x, double y) {
    double a = 20, b = 0.2, c = 2 * 3.14159265; //M_PI
    double sum_sq = pow(x, 2) + pow(y, 2);
    double sum_cos = cos(c * x) + cos(c * y);

    //fitnes
    double fitnes = -a * exp(-b * sqrt(sum_sq / 2)) - exp(sum_cos / 2) + a + exp(1); //M_E
    return fitnes;
}

auto fenotype = [](const chromosome_t& chromosome) {
    auto decoded = decodeGreyCode(chromosome);
    return std::pair<double,double>(decoded[0],decoded[1]);
};

double fitness(const chromosome_t &chromosome) {
    return -Ackley(fenotype(chromosome).first, fenotype(chromosome).second);
}

population_t selection(const population_t &population) {
    using namespace std;
    vector<double> fitness_values;
    for (const auto &chromosome : population) {
        fitness_values.push_back(fitness(chromosome));
    }

//Selekcja ruletkowa:


//Krzyżowanie jednopunktowe:


//Mutacja probabilistyczna (jednorodna):


int main() {
    population_t population = generate_population(50);
    int generation = 0;
    auto max_generations = 100;


    double best_fitness = std::numeric_limits<double>::max();
    chromosome_t best_chromosome;
    for (const auto &chromosome: population) {
        double fitness = -Ackley(fenotype(chromosome).first, fenotype(chromosome).second);
        if (fitness < best_fitness) {
            best_fitness = fitness;
            best_chromosome = chromosome;
        }
    }

    auto best_solution = std::min_element(population.begin(), population.end(),
                                          [](const chromosome_t &a, const chromosome_t &b) {
                                              return Ackley(fenotype(a).first, fenotype(a).second) <
                                                     Ackley(fenotype(b).first, fenotype(b).second);
                                          });
    std::cout << "Best solution: x: " << fenotype(*best_solution).first << " y: " << fenotype(*best_solution).second
              << " with fitness: " << Ackley(fenotype(*best_solution).first, fenotype(*best_solution).second)
              << std::endl;
}
