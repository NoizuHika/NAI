#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <cmath>
#include <vector>
#include <numeric>

#define PI 3.14159265

using namespace std;
using mojafunkcja_t = std::function<double(double, double)>;
using formatery_t = std::map<std::string,mojafunkcja_t>;

struct AccumulateMapValues
{
    template<class V, class Pair>
    V operator()(V value, const Pair &pair) const {
        return value + pair.second;
    }
};

void f1(){
    cout << "Sumuje dwie wartosci\n";

    double a;
    double b;
    cin >> a >> b;
    std::map<std::string , double> map = {
            {"1", a}, {"2",b}
    };

    int total_sum = accumulate(map.begin(), map.end(), 0, AccumulateMapValues());
    cout << total_sum << endl;
}

void f2(){
    cout << "Oblicza wartosc reszty z dzielenia\n";
    int m;
    int n;
    cin >> m >> n;
    int x = m % n;
    cout << x;
}

void f3(){
    cout << "Liczy wartosc funkcji sinus dla jednego argumentu\n";
    double param;
    cin >> param;
    cout << sin(param * PI / 180);
}

int main(int argc, char **argv) {
    using namespace std;

    map<string, void(*)()> m;
    string S;

    map<string,double> mapa;

    formatery_t formatery;
    formatery["add"] = [](double a, double b) {return a+b;};
    formatery["mod"] = [](double a, double b) {return fmod(a,b);};
    formatery["sin"] = [](double a, double b) {return sin(a);};


    m.insert(pair<string, void(*)()>("lab1 add", f1));
    m.insert(pair<string, void(*)()>("lab1 mod", f2));
    m.insert(pair<string, void(*)()>("lab1 sin", f3));


  /*
    map< double, double > nums = {
            { a, b }
    };

    map< int, int, double(*)(double, double) > mg1(f1add);
    cout << f1add;
    */


    cout << "Podaj argument w postaci: lab1 [FUNKCJA] [Argument1] [Argument2]. Dostepne to: ";
    for (auto [k, v] : formatery) cout << " " << k;
    cout << endl;
    getline(cin, S);

    map<string, void(*)()>::iterator it;
    it = m.find(S);

    if (it != m.end()){
        it -> second();
    } else {
        cout << "Wpisz prawidlowo zapytanie. " << endl;
        cout << "Przyklad: lab1 [FUNKCJA] [Argument1] [Argument2]";
    }


    try {
        vector<string> argumenty(argv, argv + argc);
    }
    catch (std::out_of_range aor){
        cout << endl;
    }


    return 0;
}


/*
using mojamapa_t = std::map<std::string, std::string>;
using mojafunkcja_t = std::function<std::string(std::string)>;
void wypisz(mojamapa_t mapa, mojafunkcja_t fun) {
    using namespace std;
    for (auto kv : mapa) {
        auto [k, v] = kv;
        cout << "klucz: " << k << "; wartosc " << fun(v) << endl;
    }
}
int main(int argc, char **argv) {
    using namespace std;
    map<string, string> mapa = {{"imie", "Jan"}};
    map<string, mojafunkcja_t> formatery;
    formatery["r"] = [](string x) { return "== " + x + " =="; };
    formatery["p"] = [](string x) { return "__" + x + "__"; };
    try {
        vector<string> argumenty(argv, argv + argc);
        auto selected_f = argumenty.at(1);
        wypisz(mapa, formatery.at(selected_f));
    } catch (std::out_of_range aor) {
        cout << "podaj argument. Dostepne to: ";
        for (auto [k, v] : formatery) cout << " " << k;
        cout << endl;
    }
    return 0;
}*/

