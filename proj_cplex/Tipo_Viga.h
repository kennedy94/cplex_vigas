#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <vector>
using namespace std;
class Tipo_Viga {
protected:
	const Tipo_Viga &operator=(const Tipo_Viga &obj);
	
public:
	int tempo_cura, n_comprimentos;
	vector<double> comprimentos;
	vector<int> demandas;
};


