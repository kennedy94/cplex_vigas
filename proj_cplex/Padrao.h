#pragma once
#include <iostream>
using namespace std;


class Padrao {
protected:
	void gerar_cobertos(int k);
	

public:
	int id;
	int tipo;
	int k;
	int *tamanhos;
	double cap;
	int n_cobertos;
	bool contem(int tam);
};

bool operator== (const Padrao &c1, const Padrao &c2);

bool operator< (const Padrao &c1, const Padrao &c2);

