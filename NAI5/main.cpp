#include <iostream>
#include <vector>
#include <functional>
#include <random>
std::random_device rd;
std::mt19937 mt_generator(rd());

using chromosome_t = std::vector<int>;
using population_t = std::vector<chromosome_t>;
// Funkcja generująca populację
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

// Tworzenie funkcji lambda "decodeGreyCode"
// Funkcja przyjmuje jako argument "chromosome", czyli wektor liczb typu int
auto decodeGreyCode = [](const chromosome_t& chromosome) {

    // Zmienna "x" używana do przechowywania wartości pierwszej połówki chromosomu, y drugiej
    double x = 0, y = 0;
    // Pętla przeglądająca pierwszą połówkę chromosomu
    for (int i = 0; i < chromosome.size()/2; i++) {
        // Dodawanie do "x" wartości chromosomu powiększonej o odpowiedni wykładnik potęgi 2
        x += chromosome[i] * std::pow(2,chromosome.size()/2-i-1);
    }
    // Pętla przeglądająca drugą połówkę chromosomu
    for (int i = chromosome.size()/2; i < chromosome.size(); i++) {
        // Dodawanie do "y" wartości chromosomu powiększonej o odpowiedni wykładnik potęgi 2
        y += chromosome[i] * std::pow(2,chromosome.size()-i-1);
    }
    return std::make_pair(x/1000000000000000,y/1000000000000000);
};

// Tworzy funkcję Ackley, która przyjmuje dwa argumenty x i y i zwraca wartość fitnes dla danych wejściowych
double Ackley(double x, double y) {
    // Inicjalizacja stałych a, b i c
    double a = 20, b = 0.2, c = 2 * 3.14159265; //M_PI
    int abc = 40;
    // Suma kwadratów x i y
    double sum_sq = pow(x, 2) + pow(y, 2);
    // Suma wartości cosinusów dla x i y
    double sum_cos = cos(c * x) + cos(c * y);

    //fitnes
    // Obliczenie wartości fitnes
    double fitnes = -a * exp(-b * sqrt(sum_sq / 2)) - exp(sum_cos / 2) + a + exp(1); //M_E
    return -fitnes + abc;
}

// Tworzy funkcję fenotype, która przyjmuje jako argument chromosom i zwraca parę wartości double
auto fenotype = [](const chromosome_t& chromosome) {
    // Dekodowanie genów za pomocą funkcji decodeGreyCode
    auto decoded = decodeGreyCode(chromosome);
    // Zwrócenie dekodowanej pary wartości jako wartości x i y

    return decoded;

};

//Selekcja ruletkowa:
population_t roulette_selection(const population_t& population) {
    population_t selected;
    double total_fitness = 0;
    std::vector<double> cumulative_fitness;
    // Obliczenie fitness dla każdego chromosomu i dodanie do łącznej sumy fitness
    for (auto &chromosome : population) {
        double fitness = Ackley(fenotype(chromosome).first, fenotype(chromosome).second);
        total_fitness += fitness;
        cumulative_fitness.push_back(total_fitness);
    }
    // Wybieranie chromosomu za pomocą ruletki
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
    // Tworzenie generatora liczb losowych i rozkładu normalnego
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, stddev);
    // Iteracja przez każdy element chromsome
    for (int i = 0; i < chromosome.size(); i++) {
        // Jeśli wartość losowa z generatora jest mniejsza niż mutationRate, dokonaj mutacji
        if (std::uniform_real_distribution<double>(0,1)(generator) < mutationRate) {
            chromosome[i] += distribution(generator);
        }
    }
}

//Krzyżowanie jednopunktowe:
std::pair<chromosome_t, chromosome_t> one_point_crossover(const chromosome_t& parent1, const chromosome_t& parent2) {
    // - Funkcja krzyżuje dwie chromosomy rodzicielskie na pojedynczym punkcie.
    // - Określa losowy punkt krzyżowania.
    int crossover_point = std::uniform_int_distribution<int>(0, parent1.size()-1)(mt_generator);
    // - Tworzy dwoje dzieci, przypisując pierwszej część jednego rodzica do pierwszego dziecka i drugą część drugiego rodzica do tego samego dziecka.
    chromosome_t child1(parent1.begin(), parent1.begin() + crossover_point);
    child1.insert(child1.end(), parent2.begin() + crossover_point, parent2.end());
    chromosome_t child2(parent2.begin(), parent2.begin() + crossover_point);
    child2.insert(child2.end(), parent1.begin() + crossover_point, parent1.end());
    // - Zwraca pary dzieci jako wynik funkcji.
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
        //Generowanie liczby pseudolosowej z przedziału [0,1]
        double r = std::uniform_real_distribution<double>(0, 1)(mt_generator);
        if (r < mutation_prob) {
            //Wartość na tej pozycji zostaje zmieniona z 0 na 1 lub z 1 na 0
            chromosome[i] = (chromosome[i] == 0) ? 1 : 0;
        }
    }
}

int main() {
    population_t population = generate_population(100 + (19727 % 10) * 2);
    int generation = 0;
    int max_generations = 100;
    while (generation < max_generations) {
        //Funkcja roulette_selection jest wywoływana na population i wynik jest przypisywany do population
        population = roulette_selection(population);
        //Zmienna new_population jest definiowana jako pusta lista
        population_t new_population;
        for (int i = 0; i < population.size(); i += 2) {
            //Funkcja one_point_crossover jest wywoływana na tych dwóch elementach, a wynik jest przypisywany do zmiennej children.
            auto children = one_point_crossover(population[i], population[i + 1]);
            auto mutation_prob = 0.01;
            //Funkcja uniform_mutation jest wywoływana na children.first i children.second, gdzie children jest wynikiem funkcji one_point_crossover.
            uniform_mutation(children.first, mutation_prob);
            uniform_mutation(children.second, mutation_prob);//Oba elementy z children są dodawane do new_population
            new_population.push_back(children.first);
            new_population.push_back(children.second);
        }
        population = new_population;//population jest ustawiane na new_population
        generation++;
    }

    //Zmienna best_fitness jest ustawiona na std::numeric_limits<double>::max().
    // Zmienna best_chromosome jest definiowana jako pusty wektor.
    double best_fitness = std::numeric_limits<double>::max();
    chromosome_t best_chromosome;
    //Pętla for jest wykonywana dla każdego elementu w population.
    // Funkcja Ackley jest wywoływana na fenotype(chromosome).first i fenotype(chromosome).second, a wynik jest przypisywany do fitness.
    // Jeśli fitness jest mniejszy od best_fitness, wartość best_fitness jest zmieniana na fitness i best_chromosome jest zmieniana na chromosome.
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
    //Po zakończeniu pętli for, następuje wywołanie funkcji min_element na population.begin() i population.end(), za pomocą lambda funkcji porównującej fitness dla dwóch chromosomów.
    //W wyniku, best_solution jest ustawiony na najlepszy chromosom.
    auto best_solution = std::max_element(population.begin(), population.end(),
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

    return 0;

}

