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
    return -fitnes;
}

auto fenotype = [](const chromosome_t& chromosome) {
    auto decoded = decodeGreyCode(chromosome);
    return std::pair<double,double>(decoded[0],decoded[1]);
};

//Selekcja ruletkowa:
population_t roulette_selection(const population_t& population) {
    population_t selected;
    double total_fitness = 0;
    std::vector<double> cumulative_fitness;

    for (auto &chromosome : population) {
        double fitness = -Ackley(fenotype(chromosome).first, fenotype(chromosome).second);
        total_fitness += fitness;
        cumulative_fitness.push_back(total_fitness);
    }

    for (int i = 0; i < population.size(); i++) {
        double r = std::uniform_real_distribution<double>(0, total_fitness)(mt_generator);
        auto selected_chromosome_index = std::distance(cumulative_fitness.begin(), std::lower_bound(cumulative_fitness.begin(), cumulative_fitness.end()-1, r));
        selected.push_back(population[selected_chromosome_index]);
    }
    return selected;
}

//Selekcja turniejowa
int selection_tournament(std::vector<double> fitnesses) {
    std::uniform_int_distribution<int> uniform(0, fitnesses.size()-1);
    int a = uniform(mt_generator);
    int b = uniform(mt_generator);
    return (fitnesses[a]>fitnesses[b])?a:b;
}

//MutacjaGaussian
void gaussianMutation(std::vector<double>& chromosome, double mutationRate, double mean, double stddev) {
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, stddev);

    for (int i = 0; i < chromosome.size(); i++) {
        if (std::uniform_real_distribution<double>(0,1)(generator) < mutationRate) {
            chromosome[i] += distribution(generator);
        }
    }
}

//Krzyżowanie jednopunktowe:
std::pair<chromosome_t, chromosome_t> one_point_crossover(const chromosome_t& parent1, const chromosome_t& parent2) {
    int crossover_point = std::uniform_int_distribution<int>(0, parent1.size()-1)(mt_generator);
    chromosome_t child1(parent1.begin(), parent1.begin() + crossover_point);
    child1.insert(child1.end(), parent2.begin() + crossover_point, parent2.end());
    chromosome_t child2(parent2.begin(), parent2.begin() + crossover_point);
    child2.insert(child2.end(), parent1.begin() + crossover_point, parent1.end());
    return {child1, child2};
}

//Krzyżowanie dwupunktowe
std::pair<chromosome_t, chromosome_t> two_point_crossover(const chromosome_t& parent1, const chromosome_t& parent2) {
    int crossover_point1 = std::uniform_int_distribution<int>(0, parent1.size()-2)(mt_generator);
    int crossover_point2 = std::uniform_int_distribution<int>(crossover_point1 + 1, parent1.size()-1)(mt_generator);
    chromosome_t child1(parent1.begin(), parent1.begin() + crossover_point1);
    child1.insert(child1.end(), parent2.begin() + crossover_point1, parent2.begin() + crossover_point2);
    child1.insert(child1.end(), parent1.begin() + crossover_point2, parent1.end());
    chromosome_t child2(parent2.begin(), parent2.begin() + crossover_point1);
    child2.insert(child2.end(), parent1.begin() + crossover_point1, parent1.begin() + crossover_point2);
    child2.insert(child2.end(), parent2.begin() + crossover_point2, parent2.end());
    return {child1, child2};
}

//Mutacja probabilistyczna (jednorodna):
void uniform_mutation(chromosome_t& chromosome, double mutation_prob) {
    for (int i = 0; i < chromosome.size(); i++) {
        double r = std::uniform_real_distribution<double>(0, 1)(mt_generator);
        if (r < mutation_prob) {
            chromosome[i] = (chromosome[i] == 0) ? 1 : 0;
        }
    }
}

int main() {
    population_t population = generate_population(100 + (19727 % 10) * 2);
    int generation = 0;
    int max_generations = 100;
    while (generation < max_generations) {
        population = roulette_selection(population);
        population_t new_population;
        for (int i = 0; i < population.size(); i += 2) {
            auto children = one_point_crossover(population[i], population[i + 1]);
            auto mutation_prob = 0.01;
            uniform_mutation(children.first, mutation_prob);
            uniform_mutation(children.second, mutation_prob);
            new_population.push_back(children.first);
            new_population.push_back(children.second);
        }
        population = new_population;
        generation++;
    }

    std::vector<chromosome_t> fitnesses(population.size());

    std::vector<chromosome_t> new_population(population.size());
    for (int i = 0; i < population.size(); i++) {
        std::vector<chromosome_t> p1 = roulette_selection(fitnesses);
        std::vector<chromosome_t> p2 = roulette_selection(fitnesses);
        std::cout << "parent1: " << &p1 << " parent2: " << &p2  << " mutated: " << &new_population[i] << std::endl;
    }

    double best_fitness = std::numeric_limits<double>::max();
    chromosome_t best_chromosome;
    for (const auto &chromosome: population) {
        double fitness = Ackley(fenotype(chromosome).first, fenotype(chromosome).second);
        if (fitness < best_fitness) {
            best_fitness = fitness;
            best_chromosome = chromosome;
        }
    }

    for (auto &chromosome: population) {
        std::cout << "x= " << fenotype(chromosome).first << ", y= " << fenotype(chromosome).second << " | fitness = " << Ackley(fenotype(chromosome).first,fenotype(chromosome).second) << std::endl;
    }

    std::cout << std::endl;

    auto best_solution = std::min_element(population.begin(), population.end(),
                                          [](const chromosome_t &a, const chromosome_t &b) {
                                              return Ackley(fenotype(a).first, fenotype(a).second) <
                                                     Ackley(fenotype(b).first, fenotype(b).second);
                                          });
    std::cout << "Best solution: x: " << fenotype(*best_solution).first << " y: " << fenotype(*best_solution).second
              << " with fitness: " << Ackley(fenotype(*best_solution).first, fenotype(*best_solution).second)
              << std::endl;

    std::cout << "Best chromosome: ";
    for (const auto &gene : best_chromosome) {
        std::cout << gene << " ";
    }
    std::cout << "with fitness: " << best_fitness << std::endl;

    return 0;

}

