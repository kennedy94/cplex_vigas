#pragma once
#include "Padrao.h"
#include "Tipo_Viga.h"
#include <list>
#include <iostream>
#include <fstream>
#include <set>
#include <ilcplex/ilocplex.h>
#include "OPERACAO.h"


using namespace std;


class Problema_Vigas {
protected:
	//dados da instancia
	int C, M, T;
	double *c_; // Guarda as capacidades das formas
	const char* instancia_nome;

	Tipo_Viga *Viga;
	Padrao *Pattern; 
	int P; //#padroes
	int P_antigo;
	
private:
	IloEnv env;
	IloModel model;
	IloCplex cplex;
	IloArray<IloArray<IloBoolVarArray> > x;          //1 se o padrao 'i' comeca na forma 'm' no periodo 't'
	IloBoolVarArray z;
	//IloIntVar z;
	int numvar, numvarP;


	bool relaxacaolinear;
	

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

	void simetria();

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

	//Número de vezes em que o conjunto conj cobre o tamanho tamanho do tipo tipo o_O
	int cobre(list<Padrao> conj, int tipo, int tamanho);

	bool cobre_tudo_kvezes(list<Padrao> conj, int vezes);



	//Computar número de tamanhos não cobertos que o padrão pat cobre

	int cobre_naocobertos(list<Padrao> conj, Padrao pat, int n_vezes);

	list<OPERACAO> HEURISTIQUE_PLUS_VITE_PLUS_PETITES();

	void TRANSFORMAR_em_MAXIMAL(Padrao P, double FORMA_cap);






	int get_P();
	int get_C();
	Padrao *get_Padroes();
	Tipo_Viga *get_Tipos();
	

	~Problema_Vigas();
};
