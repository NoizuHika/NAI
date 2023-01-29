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

int main() {
    population_t population = generate_population(100 + (19727 % 10) * 2);

    for (auto &chromosome: population) {
        std::cout << "x= " << fenotype(chromosome).first << ", y= " << fenotype(chromosome).second << " | fitness = " << -Ackley(fenotype(chromosome).first,fenotype(chromosome).second) << std::endl;
    }

}