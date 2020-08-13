#pragma once
#include <iostream>
#include "Tipo_Viga.h"
#include <vector>
using namespace std;



class Padrao {
protected:
	void gerar_cobertos(int k);
	

public:
	//Construtores
	Padrao();
	//Estruturas
	int id;
	int tipo;
	int n_cobertos;
	int k;
	vector<int> tamanhos;
	double cap;
	//Métodos

	bool comparar_demandas(const Tipo_Viga & c1);
	bool comparar_demandas(const Tipo_Viga & c1, int IND_TAMANHO);
	void contar();
	bool contem(int tam);
	int n_cobre_naocobertos;
	void alocar_PADRAO(int k, int tipo);
	

};

bool operator== (const Padrao &c1, const Padrao &c2);

bool operator< (const Padrao &c1, const Padrao &c2);

bool operator> (const Padrao &c1, const Padrao &c2);
bool operador_padrao(const Padrao &c1, const Padrao &c2);

bool operador_padrao_naocobertos(const Padrao & c1, const Padrao & c2);

