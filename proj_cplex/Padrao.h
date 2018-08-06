#pragma once
#include <iostream>
#include "Tipo_Viga.h"
using namespace std;



class Padrao {
protected:
	void gerar_cobertos(int k);
	

public:
	void alocar_PADRAO(int k, int tipo);
	bool comparar_demandas(const Tipo_Viga & c1, const Padrao & c2);
	int id;
	int tipo;
	
	void contar();
	int n_cobertos;
	int k;
	int *tamanhos;
	double cap;

	bool contem(int tam);
	int n_cobre_naocobertos;
	void alocar_PADRAO(int k, int tipo);
	Padrao();


	

};

bool operator== (const Padrao &c1, const Padrao &c2);

bool operator< (const Padrao &c1, const Padrao &c2);

bool operator> (const Padrao &c1, const Padrao &c2);
bool operador_padrao(const Padrao &c1, const Padrao &c2);

bool operador_padrao_naocobertos(const Padrao & c1, const Padrao & c2);

