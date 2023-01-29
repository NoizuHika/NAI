#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const int lower_bounded = -5.12; //dolny zakres wartosci (do elementu wektora rozwiazania)
const int upper_bounded = 5.12; //gorny
const int num_coords = 3; //liczba wspolrzednych wektora

vector<int> gray_encode(vector<int> binary) { //przyjmuje wektor liczb calkowitych i zwraca wektor po kodowaniu Graya
    int n = binary.size(); //rozmiar wektora binarnego
    vector<int> gray(n); //wektor o tym samym rozmiarze
    gray[0] = binary[0]; //1 element graya = 1 element binary
    for (int i = 1; i < n; i++) { //przejsc przez elementy, XOR biezacy a poprzedni element. 0XOR0=0. 0XOR1=1. 1XOR0=1. 1XOR1=0
        gray[i] = binary[i] ^ binary[i - 1]; //przypisac wynik do elementu
    }
    return gray;
}

vector<int> gray_decode(vector<int> gray) { //przyjmuje wektor liczb calkowitych po kodowaniu Graya i zwraca wektor po dekodowaniu
    int n = gray.size(); //rozmiar wektora gray.
    vector<int> binary(n); //Tworzymy binary z tym samym rozmiarem
    binary[0] = gray[0]; //1 element binary = 1 element gray
    for (int i = 1; i < n; i++) { //przechodzimy przez elementy. XOR.
        binary[i] = gray[i] ^ binary[i - 1]; //przypisujemy wyniki
    }
    return binary;
}

//przyjmuje wektor liczb całkowitych jako genotyp i zwraca wektor liczb zmiennoprzecinkowych jako fenotyp.
vector<double> decode(vector<int> genotype) {
    //Wykonujemy dekodowanie kodu Graya przy użyciu funkcji "gray_decode" i przypisujemy wynik do wektora "binary".
    vector<int> binary = gray_decode(genotype);
    //Zmienna "num_bits" jest ustawiona na rozmiar wektora "binary", który jest liczbą bitów w genotypie.
    int num_bits = binary.size();
    //Tworzymy wektor "solution" o liczbie współrzędnych (num_coords) i typie double.
    vector<double> solution(num_coords);
    //Używamy pętli for, aby przejść przez każdą współrzędną rozwiązania.
    // Zmienna "start" jest ustawiona na pozycję początkową dla bieżącej współrzędnej w wektorze "binary".
    for (int i = 0; i < num_coords; i++) {
        int start = i * (num_bits / num_coords);
        //Ta linijka oblicza końcowy indeks dla aktualnej współrzędnej, która jest rozpatrywana w pętli.
        // Indeks końcowy jest obliczany jako iloczyn liczby bitów dzielonej przez liczbę współrzędnych rozwiązania
        // i aktualnie rozpatrywanej współrzędnej (i) plus jeden.
        int end = (i + 1) * (num_bits / num_coords);
        //Ta część kodu jest odpowiedzialna za zamianę ciągu binarnego na wartość dziesiętną.
        // Zmienna value jest inicjalizowana jako zero, a następnie przechodzi przez pętle od indeksu start do indeksu end.
        // Wewnątrz pętli, wartość bitu na pozycji j jest dodawana do wartości, mnożona przez 2 do potęgi
        int value = 0;
        for (int j = start; j < end; j++) {
            value = value * 2 + binary[j];
        }
        //Ta linijka konwertuje wartość dziesiętną na wartość rzeczywistą, reprezentującą współrzędną rozwiązania.
        // Przede wszystkim zmienna value jest dzielona przez (pow(2, (num_bits / num_coords)) - 1) aby otrzymać wartość proporcjonalną.
        // Następnie mnoży się ją przez (upper_bound - lower_bound) i dodaje (lower_bound) aby otrzymać końcową wartość rzeczywistą.
        solution[i] = lower_bounded + (upper_bounded - lower_bounded) * ((double) value / (pow(2, (num_bits / num_coords)) - 1));
    }
    return solution;
}

// zmienna x, y, z jest przypisana do odpowiednich elementów wektora phenotype i następnie sumowane są ich kwadraty.
double fitness(vector<double> phenotype) {
    double x = phenotype[0];
    double y = phenotype[1];
    double z = phenotype[2];
    return (x*x + y*y + z*z);
}

void selection(vector<vector<int>> &population, vector<double> fitness_values) {
    int population_size = population.size();
    vector<pair<vector<int>, double>> population_fitness;
    for (int i = 0; i < population_size; i++) {
        population_fitness.push_back({population[i], fitness_values[i]});
    }
    sort(population_fitness.begin(), population_fitness.end(),
         [](auto &left, auto &right) { return left.second > right.second; });
    for (int i = 0; i < population_size; i++) {
        population[i] = population_fitness[i].first;
    }
}

vector<int> crossover_one_point(vector<int> parent1, vector<int> parent2) {
    int crossover_point = rand() % parent1.size();
    vector<int> offspring(parent1.size());
    for (int i = 0; i < crossover_point; i++) {
        offspring[i] = parent1[i];
    }
    for (int i = crossover_point; i < parent1.size(); i++) {
        offspring[i] = parent2[i];
    }
    return offspring;
}

vector<int> mutation(vector<int> genotype, double mutation_probability) {
    for (int i = 0; i < genotype.size(); i++) {
        double random_value = ((double) rand() / (RAND_MAX));
        if (random_value < mutation_probability) {
            genotype[i] = (genotype[i] == 0 ? 1 : 0);
        }
    }
    return genotype;
}


int main() {
    int index = 19727;
    int length = 100 + (index % 10) * 2;
    //vector<int> genotype jest tworzony o długości length i wypełniony losowymi liczbami binarnymi (0 lub 1).
    vector<int> genotype(length);
    for (int i = 0; i < length; i++) {
        genotype[i] = rand() % 2;
    }



    //genotype jest kodowany na kod Gray przez funkcję gray_encode.
    genotype = gray_encode(genotype);
    //vector<double> phenotype jest dekodowany z genotypu przez funkcję decode.
    vector<double> phenotype = decode(genotype);
    double test_phenotype_fitness = fitness(phenotype); //funkcja fintess
    cout << "Fitness for test phenotype: " << test_phenotype_fitness << endl;
    vector<double> optimal_phenotype(num_coords, 0);//optimal_phenotype jest tworzony jako wektor o liczbie współrzędnych num_coords i wszystkich elementach równych 0.
    double optimal_phenotype_fitness = fitness(optimal_phenotype);//optimal_phenotype_fitness jest obliczany przez funkcję fitness dla optimal_phenotype.
    cout << "Fitness for optimal phenotype: " << optimal_phenotype_fitness << endl;
    return 0;
}