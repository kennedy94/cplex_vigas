#include "Gerador_P.h"

#include <algorithm>

#define EPSILON 0.0005

Gerador_P::Gerador_P(const char* filename) {
	ifstream instancia(filename, ifstream::in);
	if (instancia.fail()) {
		cerr << "     Arquivo \"" << filename << "\" nao encontrado." << endl;
		exit(1);
	}

	/*Leitura dos dados e alocação*/

	nome_instancia = filename;
	instancia >> C >> M >> T; //Só usa para padroes de empacotamento

	L.resize(M); //vai ser vetor unique
	FORMAS.resize(M);
	Tipos_de_Viga.resize(C);
	Maior_Forma = 0;
	Menor_Forma = INT_MAX;

	/*Para cada tipo de viga ler os n_tamanhos, tamanhos, demandas, cura e n_barras
	necessárias*/
	Maior_Qc = 0;
	Menor_tamanho = IloNumArray(env, C);
	for (int i = 0; i < C; i++) {
		Menor_tamanho[i] = 100;

		instancia >> Tipos_de_Viga[i].tempo_cura
			>> Tipos_de_Viga[i].n_comprimentos;

		Tipos_de_Viga[i].demandas.resize(Tipos_de_Viga[i].n_comprimentos);
		Tipos_de_Viga[i].comprimentos.resize(Tipos_de_Viga[i].n_comprimentos);

		if (Tipos_de_Viga[i].n_comprimentos > Maior_Qc)
			Maior_Qc = Tipos_de_Viga[i].n_comprimentos;



		//Ler comprimentos
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++) {
			instancia >> Tipos_de_Viga[i].comprimentos[k];
			if (Tipos_de_Viga[i].comprimentos[k] < Menor_tamanho[i])
				Menor_tamanho[i] = Tipos_de_Viga[i].comprimentos[k];
		}

		//Demandas dos comprimentos
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++)
			instancia >> Tipos_de_Viga[i].demandas[k];

	}

	//Ler vetor das formas e seus tamanhos
	for (int i = 0; i < M; i++) {
		instancia >> FORMAS[i];
		if (FORMAS[i] > Maior_Forma)
			Maior_Forma = FORMAS[i];
		if ((FORMAS[i] < Menor_Forma))
			Menor_Forma = FORMAS[i];
	}

	L = FORMAS;
	sort(L.begin(), L.end());

	/*Calculo do numero de tamanhos de forma diferente e seu vetor*/
	auto iterador_auxiliar = unique(L.begin(), L.end());//Tirar duplicados
	L.resize(distance(L.begin(), iterador_auxiliar));//Mudar o tamanho

	Gamma = L.size();//Gamma é o tamanho do vetor de comprimentos únicos de forma

	instancia.close();
}

void Gerador_P::Iniciar_Modelo_Packing() {
	/*Alocar variáveis*/
	model = IloModel(env);
	c = IloIntVar(env, 0, C - 1, "c");
	A = IloIntVarArray(env, Maior_Qc, 0, 100);
	gamma = IloIntVar(env, 0, Gamma - 1, "gamma");
	cap = IloNumVar(env, 0, Maior_Forma, "cap");

	for (int i = 0; i < C; i++) {
		for (int k = Tipos_de_Viga[i].n_comprimentos; k < Maior_Qc; k++) {
			model.add(IloIfThen(env, c == i, A[k] == 0));
		}
		IloExpr soma(env);
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++) {
			soma += Tipos_de_Viga[i].comprimentos[k] *
				A[k];
		}

		//model.add(IloIfThen(env, c == i, Menor_Forma - Menor_tamanho[i] + FLT_EPSILON <= soma));
		for (int m = 0; m < Gamma; m++)
		{
			model.add(IloIfThen(env, c == i && m == gamma, L[m] - Menor_tamanho[i] + FLT_EPSILON <= soma));
			model.add(IloIfThen(env, c == i && m == gamma, soma <= L[m]));
			model.add(IloIfThen(env, c == i && m == gamma, cap == soma));
		}
	}
}

void Gerador_P::Rodar_Packing() {
	/*Iniciar variáveis e montar o modelo*/
	Iniciar_Modelo_Packing();
	/*Resolver o modelo em si*/
	Resolver_Packing();
}

void Gerador_P::Resolver_Packing() {
	//Passar por cada solução e a guarda em uma lista de padrões que será impressa após
	try {

		IloCP cp(model);
		cp.propagate();
		cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);
		cout << cp.domain(A) << endl;

		cp.startNewSearch();
		while (cp.next()) {
			list<double> auxiliar;
			auxiliar.push_back(cp.getValue(c));

			auxiliar.push_back(cp.getValue(cap));

			for (int i = 0; i < Tipos_de_Viga[cp.getValue(c)].n_comprimentos; i++)
				auxiliar.push_back(cp.getValue(A[i]));

			Padroes.push_back(auxiliar);
		}

	}
	catch (IloException& ex) {
		env.out() << "Error: " << ex << endl;
	}
	env.end();
}

void Gerador_P::ImprimirPadrao_Packing() {

	/*Imprimir padroes no arquivo*/
	string arquivo_saida = ".pat";
	stringstream ss;
	ss << nome_instancia << arquivo_saida;
	arquivo_saida = ss.str();

	ofstream saida(arquivo_saida);

	saida << Padroes.size() + 1 << endl << "0 0 ";
	for (int i = 0; i < Tipos_de_Viga[0].n_comprimentos; i++)
		saida << "0 ";
	saida << endl;

	for (auto elemento : Padroes) {
		for (auto gene : elemento)
			saida << gene << " ";
		saida << endl;
	}
	saida.close();
}