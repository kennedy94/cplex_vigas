#pragma once
#include "Padrao.h"
#include "Tipo_Viga.h"
#include <list>
#include <iostream>
#include <fstream>
#include <set>
#include <ilcplex/ilocplex.h>

using namespace std;

class Problema_Vigas {
protected:
	//dados da instancia
	int C, M, T;
	double *c_;
	const char* instancia_nome;

	Tipo_Viga *Viga;
	Padrao *Pattern; 
	int P; //#padroes
	
private:
	IloEnv env;
	IloModel model;
	IloCplex cplex;
	IloArray<IloArray<IloBoolVarArray> > x;          //1 se o padrao 'i' comeca na forma 'm' no dia 't'
	IloBoolVarArray z;

	bool relaxacaolinear;
	int numvar, numvarP;

	void iniciar_variaveis();

	//lembrete: c_ eh um vetor de double
	void funcao_objetivo();

	void funcao_objetivo2();
	void funcao_objetivo3();

	void restricoes_onlyone();

	void restricoes_demanda();

	void restricoes_sequenciamento();

	void restricoes_continuidade();

	void restricoes_z();

	void exportar_lp();

	void resolver_linear();
	void revolver_ppl();

	void imprimir_solucao(ofstream& resultados);

	//----------------------------------------------------------------------
	bool maximal(const Padrao P, double C_FORMA);
	//pseudo-funcao de verificacao
	bool verificacao();

	void iniciar_lp(int fo, ofstream& resultados);


	

public:
	void RODAR(int fo);

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
