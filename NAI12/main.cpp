#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <fstream>
#include <chrono>

using namespace std;

//g++ main.cpp -o main
//./main input.txt

//MAX jest stałą o wartości 10^4 + 5, która będzie używana jako maksymalny rozmiar grafu
const int MAX = 1e4 + 5;

//liczba wierzchołków, liczba krawędzi, wierzchołek startowy i wierzchołek docelowy.
int n, m, s, d;
//adj to tablica list sąsiedztwa, która przechowuje informację o krawędziach i ich wagach dla każdego wierzchołka.
vector<pair<int, int>> adj[MAX];
//dist_dijkstra to wektor, który przechowuje najkrótsze odległości dla każdego wierzchołka w grafie
vector<int> dist_dijkstra(MAX, INT_MAX);
//dist_a_star to wektor, który przechowuje najkrótsze odległości dla każdego wierzchołka w grafie
vector<int> dist_a_star(MAX, INT_MAX);
//heurystyczne odległości
vector<int> h(MAX, 0);
//czy dany wierzchołek został już odwiedzony
vector<bool> visited_dijkstra(MAX, false);
vector<bool> visited_a_star(MAX, false);

void loadGraph(const string& fileName)
{
    ifstream in(fileName);
    in >> n >> m; //liczba wierzchołków i krawędzi
    for (int i = 0; i < m; i++) //wczytania krawędzi i ich wag z pliku
    {
        //Każdy wierzchołek u i v jest dodawany jako para do tablicy list sąsiedztwa "adj"
        int u, v, w;
        in >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
    //Wierzchołek startowy i wierzchołek docelowy
    in >> s >> d;
    in.close();
}

int heuristic(int u, int d)
{
    // Funkcja heurystyczna, np. odległość euklidesowa
    return sqrt(pow(u - d, 2));
}

void dijkstra(int s)
{
    auto czas_start = std::chrono::system_clock::now();
    //kolejka, odległość, wierzchołek
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    //start, odleglosc 0
    pq.push({0, s});
    dist_dijkstra[s] = 0;

    //dla każdej krawędzi z wierzchołka u, sprawdzana jest możliwość zmniejszenia odległości do wierzchołka v i
    // jeśli jest to możliwe, to wierzchołek v jest dodawany do kolejki i jego odległość jest aktualizowana.
    //dopóki kolejka nie jest pusta.
    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        if (visited_dijkstra[u])
            continue;

        visited_dijkstra[u] = true;

        for (int i = 0; i < adj[u].size(); i++)
        {
            int v = adj[u][i].first;
            int weight = adj[u][i].second;

            if (dist_dijkstra[v] > dist_dijkstra[u] + weight)
            {
                dist_dijkstra[v] = dist_dijkstra[u] + weight;
                pq.push({dist_dijkstra[v], v});
            }
        }
    }
    auto czas_stop = std::chrono::system_clock::now();

    chrono::duration<double> czas_obliczen = czas_stop-czas_start;
    cout << "Dijkstra: "  << " " << czas_obliczen.count() << "s\n";
}

void A_star(int s, int d)
{
    auto czas_start1 = std::chrono::system_clock::now();
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    //Wierzchołek startowy i wierzchołek docelowy
    h[s] = heuristic(s, d);
    //kolejka do przechowywania
    pq.push({0 + h[s], s});
    dist_a_star[s] = 0;

    //W każdej iteracji Wierzchołek o najniższym priorytecie (czyli najbliżej Wierzchołka źródłowego i Wierzchołka węzła docelowego)
    // jest usuwany z kolejkii przetwarzany
    // Jeśli przetworzony Wierzchołek jest Wierzchołek docelowym, pętla się kończy
    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        if (visited_a_star[u])
            continue;

        visited_a_star[u] = true;
        if (u == d)
            break;

        //algorytm aktualizuje odległość wszystkich swoich sąsiednich Wierzchołek i dodaje je do kolejki priorytetowej
        for (int i = 0; i < adj[u].size(); i++)
        {
            int v = adj[u][i].first;
            int weight = adj[u][i].second;

            if (dist_a_star[v] > dist_a_star[u] + weight)
            {
                dist_a_star[v] = dist_a_star[u] + weight;
                h[v] = heuristic(v, d);
                pq.push({dist_a_star[v] + h[v], v});
            }
        }
    }
    auto czas_stop1 = std::chrono::system_clock::now();

    chrono::duration<double> czas_obliczen = czas_stop1-czas_start1;
    cout << "A*: " << czas_obliczen.count() << "s\n";
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "Niepoprawna liczba argumentow.\n Uzycie: " << argv[0] << " input.txt\n";
        return 1;
    }

    // ładuje graf z pliku i wywołuje algorytm Dijkstra i A*
    string fileName(argv[1]);
    loadGraph(fileName);

    dijkstra(s);
    A_star(s, d);

    cout << "Najkrotsza odleglosc (Dijkstra): " << dist_dijkstra[d] << endl;
    cout << "Najkrotsza odleglosc (A*): " << dist_a_star[d] << endl;

    return 0;

    //Im bliżej wierzchołki tym szybszy jest A*, jednak Dijkstra zapewnia większą dokładność.
}
