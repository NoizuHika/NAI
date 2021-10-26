#include <functional>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <vector>
#include <stdexcept>
#include "Source.h"

using namespace std;

random_device rd;  //Will be used to obtain a seed for the random number engine
mt19937 mt_generator(rd()); //Standard mersenne_twister_engine seeded with rd()

ostream& operator<<(ostream& o, vector<double> v)
{
	for (auto e : v) {
		o << std::fixed << std::setprecision(5) << "\t" << e;
	}
	return o;
}

vector<double> hill_climbing(function<double(vector<double>)> f, function<bool(vector<double>)> f_domain, vector<double> p0, int iterations)
{
	auto p = p0;
	uniform_int_distribution<> distrib(0, p.size() - 1);
	uniform_real_distribution<> distrib_r(-0.1, 0.1);

	
	if (!f_domain(p)) throw std::invalid_argument("The p0 point must be in domain");
	for (int i = 0; i < iterations; i++) {
		auto p2 = p;
		
		p2[distrib(mt_generator)] += distrib_r(mt_generator);
			double y2 = f(p2);
			if (y2 < f(p)) {
				p = p2;
			}
		}
		
	return p;
}

vector<double> operator+(vector<double> a, vector<double> b)
{
	for (int i = 0; i < a.size(); i++) {
		a[i] += b[i];
	}
	return a;
}

vector<double> tabu_search(function<double(vector<double>)> f, function<bool(vector<double>)> f_domain, vector<double> p0, int iterations)
{
	const double mod_range = 0.01;

	auto offset_f = [=](int i, double dir) {
		vector<double> v(p0.size());
		v[i] = dir;
		return v;
	};
	vector<vector<double>> directions;
	for (int i = 0; i < p0.size(); i++) {
		directions.push_back(offset_f(i, mod_range));
		directions.push_back(offset_f(i, -mod_range));
	}

	vector<vector<double>> tabu_list = { p0 };
	auto in_tabu = [&tabu_list](vector<double> p) {
		for (auto& e : tabu_list) {
			int checks = 0;
			for (int i = 0; i < e.size(); i++)
				if (abs(e[i] - p[i]) < 0.00000001) checks++;
			if (checks == e.size()) return true; // in tabu
		}
		return false;
	};

	for (int i = 0; i < iterations; i++) {
		int tabu_i = tabu_list.size() - 1;
		vector<double> best_neighbour;

		while (tabu_i >= 0) {
			for (auto direction : directions) {
				if (((best_neighbour.size() == 0) ||
					(f(tabu_list[tabu_i] + direction) < f(best_neighbour))) &&
					(!in_tabu(tabu_list[tabu_i] + direction))) {
					best_neighbour = tabu_list[tabu_i] + direction;
				}
			}
			if (best_neighbour.size() != 0) break;
			tabu_i--;
		}
		tabu_list.push_back(best_neighbour);
		cout << (tabu_list.back()) << " " << f(tabu_list.back()) << endl;
	}
	return tabu_list.back();
}

int main() {

	int wybor;
	int a;

	cout << "Wybierz funkcje: \n1) Beale \n2) Himmelblau \n3) Porownianie dwoch funkcji na raz \n : ";
	cin >> wybor;

	switch (wybor) {
	case 1: {
		auto beale = [](vector<double> v) {
			double x = v.at(0), y = v.at(1);
			return pow((1.5 - x + x * y),2) + pow((2.25 - x + x * pow(y,2)),2) + pow((2.625 - x + x * pow(y, 3)),2);
		};

		auto beale_domain = [](vector<double> v) {
			return (abs(v[0]) <= 4.5) && (abs(v[1]) <= 4.5);
		};

		uniform_real_distribution<> distrib_r(-4.5, 4.5);
		vector<double> beale_p0 = {
			distrib_r(mt_generator),
			distrib_r(mt_generator),
		};

		cout << "Wybierz metod. Hill climbing 1, Tabu Search 2: ";
		cin >> a;
		if (a == 1) {
			auto result = hill_climbing(beale, beale_domain, beale_p0, 1000);
			cout << result << " -> " << beale(result) << endl;
		}
		else {
			auto result = tabu_search(beale, beale_domain, beale_p0, 1000);
			cout << result << " -> " << beale(result) << endl;
		}
		break;
	}
	case 2: {
		auto himmelblau = [](vector<double> v) {
			double x = v.at(0), y = v.at(1);
			return pow((pow(x,2) + y - 11),2) + pow((x + pow(y,2) - 7),2);
		};

		auto himmelablau_domain = [](vector<double> v) {
			return (abs(v[0]) <= 5) && (abs(v[1]) <= 5);
		};

		uniform_real_distribution<> distrib_r(-5, 5);
		vector<double> himmeblau_p0 = {
			distrib_r(mt_generator),
			distrib_r(mt_generator),
		};

		cout << "Wybierz metod. Hill climbing 1, Tabu Search 2: ";
		cin >> a;
		if (a == 1) {
			auto result = hill_climbing(himmelblau, himmelablau_domain, himmeblau_p0, 1000);
			cout << result << " -> " << himmelblau(result) << endl;
			
		}
		else {
			auto result = tabu_search(himmelblau, himmelablau_domain, himmeblau_p0, 1000);
			cout << result << " -> " << himmelblau(result) << endl;
		}
		break;
	}
	case 3: {
		auto himmelblau = [](vector<double> v) {
			double x = v.at(0), y = v.at(1);
			return pow((pow(x, 2) + y - 11), 2) + pow((x + pow(y, 2) - 7), 2);
		};

		auto himmelablau_domain = [](vector<double> v) {
			return (abs(v[0]) <= 5) && (abs(v[1]) <= 5);
		};

		uniform_real_distribution<> distrib_r(-5, 5);
		vector<double> himmeblau_p0 = {
			distrib_r(mt_generator),
			distrib_r(mt_generator),
		};

		int dif = 0;
		int dif1 = 0;
		int n = 1;
		for (int i = 0; i <= n; i++)
		{
			auto result = hill_climbing(himmelblau, himmelablau_domain, himmeblau_p0, 1000);
			cout << result << " -> " << himmelblau(result) << endl;
			dif++;
		}
		cout << " ----------------------------------- " << endl;
		for (int i = 0; i <= n; i++)
		{
			auto result = tabu_search(himmelblau, himmelablau_domain, himmeblau_p0, 1000);
			cout << result << " -> " << himmelblau(result) << endl;
			dif1++;
		}
		cout << dif << endl;
		cout << dif1 << endl;
		if (dif < dif1) {
			cout << "Lepszy wynik pokazal method: hill_climbing";
		}
		else if (dif > dif1) {
			cout << "Lepszy wynik pokazal method: tabu_search";
		}
		else {
			cout << "Remis";
		}

		break;
	}
	default: {
		cout << "Wybierz 1, 2 lub 3.";
		break;
	}
	}
	return 0;
}
