#pragma once
#include <ilcp/cp.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <list>
#include <vector>

using namespace std;

struct _Tipo_Viga {
	int tempo_cura, n_comprimentos;
	vector<int> demandas;
	vector<double> comprimentos;
};

class Gerador_P
{
private:
	IloEnv env;
	IloModel model;
	IloNumArray b;
	vector<_Tipo_Viga> Tipos_de_Viga;


	IloInt M, C, T, Gamma;
	IloInt e, k;
	vector<double> FORMAS;
	vector<double> L;
	IloNumArray l;
	IloIntArray d;
	IloIntArray estoque;
	IloNum Maior_Forma, Menor_Forma;
	IloInt Maior_Qc;
	IloNumArray Menor_tamanho;
	//Variaveis de Decisão Dependentes do Gerados de Packing
	IloIntVar gamma; //Variável auxiliar para gerar apenas maximais
	IloIntVar c;
	IloNumVar cap;
	IloIntVarArray A;

	list<list<double>> Padroes;

	const char* nome_instancia;

	void Resolver_Packing();

	void Iniciar_Modelo_Packing();

public:

	Gerador_P(const char* filename);

	void Rodar_Packing();

	void ImprimirPadrao_Packing();
};
