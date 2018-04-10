#pragma once
#include "Padrao.h"
#include "Tipo_Viga.h"
#include <list>
#include <iostream>
#include <fstream>
#include <set>
using namespace std;

class Problema_Vigas {
private:
	//dados da instancia
	int C, M, T;
	double *c_;
	const char* instancia_nome;

	Tipo_Viga *Viga;
	Padrao *Pattern; int P; //#padroes
	
public:
	Problema_Vigas(const char* filename, const char* filename2);
	void Substituir_Padroes(list<Padrao>);
	list<Padrao> gerar_conj(Padrao *Conjunto);
	bool cobre(list<Padrao> conj, int tipo, int tamanho);
	
	int get_P();
	int get_C();
	Padrao *get_Padroes();
	Tipo_Viga *get_Tipos();

	~Problema_Vigas();
};
