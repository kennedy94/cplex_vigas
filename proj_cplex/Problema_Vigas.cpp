
#include "Problema_Vigas.h"
#include <chrono>
#include <time.h>

/*
OBS: trocar tudo de ponteiro para vector
*/
#include <algorithm>
#include <vector>



Problema_Vigas::Problema_Vigas(const char* filename, const char* filename2) {

	//leitura da instancia ---------------------------------------------------
	ifstream instancia(filename, ifstream::in);
	if (instancia.fail()) {
		cerr << "     Arquivo \"" << filename << "\" nao encontrado." << endl;
		getchar();
		exit(1);
	}
	instancia_nome = filename;
	instancia >> C >> M >> T;
	//if (T < 10)
	//	T++;
	c_ = new double[M];
	Viga = new Tipo_Viga[C];
	for (int i = 0; i < C; ++i) {
		instancia >> Viga[i].e >> Viga[i].k;
		Viga[i].l = new double[Viga[i].k];
		Viga[i].d = new int[Viga[i].k];
		for (int j = 0; j < Viga[i].k; j++)
			instancia >> Viga[i].l[j];
		for (int j = 0; j < Viga[i].k; j++)
			instancia >> Viga[i].d[j];
	}
	for (int i = 0; i < M; i++)
		instancia >> c_[i];
	instancia.close();

	//leitura dos padroes
	ifstream padroes(filename2, ifstream::in);
	if (padroes.fail()) {
		cerr << "     Arquivo \"" << filename2 << "\" nao encontrado." << endl;
		exit(1);
	}
	padroes >> P;
	Pattern = new Padrao[P];

	for (int i = 0; i < P; i++) {
		padroes >> Pattern[i].tipo;
		Pattern[i].id = i;
		Pattern[i].tamanhos = new int[Viga[Pattern[i].tipo].k];
		double capa = 0;
		for (int j = 0; j < Viga[Pattern[i].tipo].k; j++) {
			padroes >> Pattern[i].tamanhos[j];
			capa += Pattern[i].tamanhos[j] * Viga[Pattern[i].tipo].l[j];
		}
		Pattern[i].cap = capa;
		Pattern[i].k = Viga[Pattern[i].tipo].k;
		Pattern[i].contar();

	}
	padroes.close();
}

int Problema_Vigas::get_P() {
	return P;
}
int Problema_Vigas::get_C() {
	return C;
}
Padrao* Problema_Vigas::get_Padroes() {
	return Pattern;
}
Tipo_Viga* Problema_Vigas::get_Tipos() {
	return Viga;
}

int Problema_Vigas::cobre(list<Padrao> conj, int tipo, int tamanho) {
	if (conj.empty())
		return 0;
	int conta = 0;
	for (auto elemento : conj)
		if (elemento.tipo == tipo & elemento.tamanhos[tamanho] > 0)
			conta++;

	return conta;
}

bool Problema_Vigas::cobre_tudo_kvezes(list<Padrao> conj, int vezes) {
	if (conj.empty())
		return false;

	for (int c = 0; c < C; c++)
		for (int tam = 0; tam < Viga[c].k; tam++)
			if (cobre(conj, c, tam) < vezes)
				return false;

	return true;
}

int Problema_Vigas::cobre_naocobertos(list<Padrao> conj, Padrao pat, int n_vezes) {

	int contador = 0;

	for (int i = 0; i < pat.k; i++)
		if (cobre(conj, pat.tipo, i) < n_vezes)
			if (pat.tamanhos[i] > 0) {
				contador++;
			}

	return contador;
}

/*__________________________________________________________________________________________
HEURISTICAS
__________________________________________________________________________________________*/
list<OPERACAO> Problema_Vigas::HEURISTIQUE_PLUS_VITE_PLUS_PETITES() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus petite'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}

	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int menor = 100;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e < menor && !USOU_OU_NAO[i]) {
				menor = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			int tamanho_escolhido = 0;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				/*Ainda cabe mais um? Se sim, incrementa*/
				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] > c_[FORMA_ATUAL] ||
					DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;
				else
					tamanho_escolhido++;
			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}

list<OPERACAO> Problema_Vigas::HEURISTIQUE_PLUS_VITE_PLUS_GROSSES() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus grosse'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}


	/*Ordena os tipos por tempo de cura de ordem crescente*/
	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int menor = 100;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e < menor && !USOU_OU_NAO[i]) {
				menor = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			/*______________________________________________________________________
			Aqui muda a prioridade do tamanho para escolher as vigas maiores primeiro
			______________________________________________________________________*/
			int tamanho_escolhido = Padrao_ATUAL.k - 1;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				/*Ainda cabe mais um? Se sim, incrementa*/
				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] > c_[FORMA_ATUAL] ||
					DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;
				/*______________________________________________________________________
				Aqui muda a prioridade do tamanho para escolher as vigas maiores primeiro
				______________________________________________________________________*/
				else
					tamanho_escolhido--;

			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}

/*ok*/
double Problema_Vigas::CALCULAR_MAKESPAN_OP(list<OPERACAO> LISTA_PAT) {

	double makespan = 0;
	vector<int> FORMAS_TEMPO_ACUM(M);

	for (int i = 0; i < M; i++)
		FORMAS_TEMPO_ACUM[i] = 0;

	for (auto elemento : LISTA_PAT) {
		FORMAS_TEMPO_ACUM[elemento.FORMA] += Viga[elemento.PADRAO_OP.tipo].e;

		if (FORMAS_TEMPO_ACUM[elemento.FORMA] >= makespan) {
			makespan = FORMAS_TEMPO_ACUM[elemento.FORMA];
		}

	}
	return makespan;
}

double Problema_Vigas::CALCULAR_SOBRA_OP(list<OPERACAO> LISTA_PAT) {

	double sobra = 0;

	for (auto elemento : LISTA_PAT)
		sobra += (c_[elemento.FORMA] - elemento.PADRAO_OP.cap)* Viga[elemento.PADRAO_OP.tipo].e;

	return sobra;
}

double Problema_Vigas::CALCULAR_TOTALCT_OP(list<OPERACAO> LISTA_PAT) {

	double total_ct = 0;
	vector<int> FORMAS_TEMPO_ACUM(M);

	for (auto elemento : LISTA_PAT)
		total_ct += Viga[elemento.PADRAO_OP.tipo].e;

	return total_ct;
}


/*	PRESTE ATENÇÃO QUE TUDO EM RELAÇÃO AOS TAMANHOS DOS PADRÕES ESTÁ CONSIDERENDO QUE ELES JÁ
ESTÃO ORDENADOS NA INSTÂNCIA*/
void Problema_Vigas::TRANSFORMAR_em_MAXIMAL(Padrao &P, double FORMA_cap) {
	int contador = 1;
	/*Vou adicionando da maior para a menor viga*/
	while (!maximal(P, FORMA_cap)) {
		if (P.cap + Viga[P.tipo].l[P.k - contador] <= FORMA_cap) {
			P.tamanhos[P.k - contador]++;
			P.cap += Viga[P.tipo].l[P.k - contador];
		}
		else
			contador++;
	}
}

list<OPERACAO> Problema_Vigas::HEURISTIQUE_MOINS_VITE_PLUS_PETITES() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus petite'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}

	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int maior = -1;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e > maior && !USOU_OU_NAO[i]) {
				maior = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			int tamanho_escolhido = 0;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				/*Ainda cabe mais um? Se sim, incrementa*/
				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] > c_[FORMA_ATUAL] ||
					DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;
				else
					tamanho_escolhido++;
			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}

list<OPERACAO> Problema_Vigas::HEURISTIQUE_MOINS_VITE_PLUS_GROSSES() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus grosse'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}


	/*Ordena os tipos por tempo de cura de ordem crescente*/
	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int maior = -1;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e > maior && !USOU_OU_NAO[i]) {
				maior = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			/*______________________________________________________________________
			Aqui muda a prioridade do tamanho para escolher as vigas maiores primeiro
			______________________________________________________________________*/
			int tamanho_escolhido = Padrao_ATUAL.k - 1;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				/*Ainda cabe mais um? Se sim, incrementa*/
				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] > c_[FORMA_ATUAL] ||
					DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;
				/*______________________________________________________________________
				Aqui muda a prioridade do tamanho para escolher as vigas maiores primeiro
				______________________________________________________________________*/
				else
					tamanho_escolhido--;

			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}


list<OPERACAO> Problema_Vigas::HEURISTIQUE_MOINS_VITE_ALTERNE() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus petite'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}

	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int maior = -1;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e > maior && !USOU_OU_NAO[i]) {
				maior = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			int tamanho_escolhido_menor = 0;
			int tamanho_escolhido_maior = Viga[TIPO_ATUAL].k - 1;
			int iterador = 0;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				iterador++;
				int tamanho_escolhido = iterador % 2 ? tamanho_escolhido_maior : tamanho_escolhido_menor;
				/*Ainda cabe mais um? Se sim, incrementa*/

				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;

				if (maximal(Padrao_ATUAL, c_[FORMA_ATUAL])
					|| tamanho_escolhido_maior <= tamanho_escolhido_menor)
					break;
				else
					(iterador % 2) ? tamanho_escolhido_maior-- : tamanho_escolhido_menor++;
			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}

list<OPERACAO> Problema_Vigas::HEURISTIQUE_PLUS_VITE_ALTERNE() {
	//J'ai une demande de betons et je veux en mettre dans les formes 
	//dans l'ordre 'premier la plus petite'
	//Comment est-ce que je fais les structures?


	//Ordenar os tipos de viga em ordem crescente de tempo de cura
	int *ORDEM_TIPOS = new int[C];	//Guarda ordem dos tipos
	bool *USOU_OU_NAO = new bool[C];	/*Booleanos para dizer se o
										tipo de viga já foi alocado
										no vetor de ordens ou não*/
	for (int i = 0; i < C; i++) {/*Inicializar os dois vetores*/
		ORDEM_TIPOS[i] = -1;
		USOU_OU_NAO[i] = false;
	}

	for (int contador = 0; contador < C; contador++) {
		int tipo;
		int menor = 100;
		for (int i = 0; i < C; i++) {
			if (Viga[i].e < menor && !USOU_OU_NAO[i]) {
				menor = Viga[i].e;
				tipo = i;
			}/*Se tipo i tem tempo de cura menor e ainda não foi
			 usado*/
		}
		ORDEM_TIPOS[contador] = tipo; //Menor tipo não usado é alocado
		USOU_OU_NAO[tipo] = true;	//Atualizar tipo escolhido nos booleanos
	}
	delete[]USOU_OU_NAO; /*Vetor utilizado só na ordenação, então
						 não é mais necessário*/

						 /*Brainstorming de estruturas
						 Criar uma estrutura de utilização de formas
						 Lista para tempos acumulados das formas
						 Lista de operações
						 Estrutura para o padrao atual na forma escolhida
						 Estrutura para a demanda atual atendida*/
	vector<int> FORMAS_ACUM(M);
	for (int i = 0; i < M; i++) FORMAS_ACUM[i] = 0;



	list<OPERACAO> list_operacao;

	/*Para cada tipo*/
	for (int iterador_ordem = 0; iterador_ordem < C; iterador_ordem++) {
		/*Até ter a demanda atendida vamos iterar?
		ou até obter um padrão maximal*/

		/*Pegar o tipo de viga a ser trabalhado*/
		int TIPO_ATUAL = ORDEM_TIPOS[iterador_ordem];

		/*Estrutura para atualizar a demanda atual*/
		Padrao DEMANDA_ATUAL;
		DEMANDA_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

		//cout << "Tipo " << TIPO_ATUAL << endl;

		/*Itera nas formas*/
		while (true)/*Até a demanda do tipo ser atendida*/ {

			/*	Pegar a primeira forma livre argmin de FORMA_ACUM	*/
			int FORMA_ATUAL = distance(FORMAS_ACUM.begin(), min_element(FORMAS_ACUM.begin(),
				FORMAS_ACUM.end()));
			/*cout << "\n\t\t" << endl;
			for (int forma = 0; forma < M; forma++)
			{
			cout << FORMAS_ACUM[forma] << " ";
			}
			cout << endl;
			getchar();*/

			//------------------PRINT
			//cout << "\t Forma " << FORMA_ATUAL << endl;
			FORMAS_ACUM[FORMA_ATUAL] += Viga[TIPO_ATUAL].e;

			/*Inicia a forma com o padrão vazio*/
			Padrao Padrao_ATUAL;
			Padrao_ATUAL.alocar_PADRAO(Viga[TIPO_ATUAL].k, TIPO_ATUAL);

			/*Itera os tamanhos
			Encher a forma até o talo*/
			int tamanho_escolhido_menor = 0;
			int tamanho_escolhido_maior = Viga[TIPO_ATUAL].k - 1;
			int iterador = 0;
			while (true)/*Até a forma estar cheia vamos iterar*/ {
				iterador++;
				int tamanho_escolhido = iterador % 2 ? tamanho_escolhido_maior : tamanho_escolhido_menor;
				/*Ainda cabe mais um? Se sim, incrementa*/

				while (Padrao_ATUAL.cap + Viga[TIPO_ATUAL].l[tamanho_escolhido] <= c_[FORMA_ATUAL]) {
					if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL], tamanho_escolhido))
						break;
					Padrao_ATUAL.cap += Viga[TIPO_ATUAL].l[tamanho_escolhido];
					Padrao_ATUAL.tamanhos[tamanho_escolhido]++;

					/*Auxilar só para checar se a demanda foi atingida então não precisa criar a estrutura
					toda bonitinha*/
					DEMANDA_ATUAL.tamanhos[tamanho_escolhido]++;
				}


				/*cout << "\t\t";
				for (int iter_print = 0; iter_print < Padrao_ATUAL.k; iter_print++)
				cout << Padrao_ATUAL.tamanhos[iter_print] << " ";
				cout << endl;
				getchar();*/

				if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
					break;

				if (maximal(Padrao_ATUAL, c_[FORMA_ATUAL])
					|| tamanho_escolhido_maior <= tamanho_escolhido_menor)
					break;
				else
					(iterador % 2) ? tamanho_escolhido_maior-- : tamanho_escolhido_menor++;
			}//end while

			OPERACAO op_aux;
			op_aux.FORMA = FORMA_ATUAL;
			op_aux.PADRAO_OP = Padrao_ATUAL;
			op_aux.TEMPO = FORMAS_ACUM[FORMA_ATUAL] - Viga[Padrao_ATUAL.tipo].e;

			list_operacao.push_back(op_aux);

			if (DEMANDA_ATUAL.comparar_demandas(Viga[TIPO_ATUAL]))
				break;
		}// end while

	}

	for (auto &elemento : list_operacao)
		TRANSFORMAR_em_MAXIMAL(elemento.PADRAO_OP, c_[elemento.FORMA]);


	//for (auto elemento : list_operacao) {
	//	cout << "\t\t Forma: " << elemento.FORMA << " - ";
	//	for (int iter_print = 0; iter_print < elemento.PADRAO_OP.k; iter_print++)
	//		cout << elemento.PADRAO_OP.tamanhos[iter_print] << " ";
	//	cout << "------" << elemento.PADRAO_OP.cap << endl;
	//	//getchar();
	//}


	/*Vamos percorrer os tipos em ordem crescente de tempos de cura.
	*/
	//cout << "\n\tMakespan: " << CALCULAR_MAKESPAN_OP(list_operacao);
	//cout << "\n\tIddleness: " << CALCULAR_SOBRA_OP(list_operacao);
	//cout << "\n\tTotal Completion Time: " << CALCULAR_TOTALCT_OP(list_operacao);
	//getchar();
	delete[]ORDEM_TIPOS;

	return list_operacao;
}

void Problema_Vigas::imprimir_resultados_heuristicas()
{
	ofstream resultados("heuristicas.txt", fstream::app);

	list<OPERACAO> SOLUCOES;

	
	
	chrono::duration<double> elapsed;


	resultados << endl << instancia_nome;

	auto TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_PLUS_VITE_PLUS_PETITES();
	auto TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;

		resultados << "\n\tSTSL\t" << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "STSL");

	TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_PLUS_VITE_PLUS_GROSSES();
	TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;
	resultados << "\n\tSTGL\t " << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "STGL");

	TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_MOINS_VITE_PLUS_PETITES();
	TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;
	resultados << "\n\tGTSL\t" << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "GTSL");

	TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_MOINS_VITE_PLUS_GROSSES();
	TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;
	resultados << "\n\tGTGL\t" << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "GTGL");

	TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_PLUS_VITE_ALTERNE();
	TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;
	resultados << "\n\tSTAL\t" << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();;
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "STAL");

	TEMPO_COMECO = chrono::high_resolution_clock::now();
	SOLUCOES = HEURISTIQUE_MOINS_VITE_ALTERNE();
	TEMPO_FIM = chrono::high_resolution_clock::now();
	elapsed = TEMPO_FIM - TEMPO_COMECO;
	resultados << "\n\tGTAL\t" << CALCULAR_SOBRA_OP(SOLUCOES) << "\t"
		<< CALCULAR_MAKESPAN_OP(SOLUCOES) << "\t"
		<< CALCULAR_TOTALCT_OP(SOLUCOES) << "\t " << elapsed.count();
	function_Solucao_Arquivo_Heuristicas(SOLUCOES, "GTAL");


	resultados.close();
}

/*
Próximos passos:
Criar uma função que transforma os padrões retornados em maximais com preferências
em fazer vigas maiores; Ok
Visualizar o Gantt; Agora tô sem saco
Calcular as funções objetivo; OK para o makespan
*/
/*__________________________________________________________________________________________
HEURISTICAS
__________________________________________________________________________________________*/


list<Padrao> Problema_Vigas::gerar_conj(Padrao *Padroes_Par) {
	list<Padrao> conjunto;
	list<Padrao> conjunto2;

	for (int i = 0; i < P; i++) {
		Padroes_Par[i].n_cobre_naocobertos = cobre_naocobertos(conjunto, Padroes_Par[i], 10);
		conjunto2.push_back(Padroes_Par[i]);
	}
	conjunto2.sort(operador_padrao);	//ordena em ordem descrescente de numero de padroes cobertos

										//conjunto2.sort(operador_padrao_naocobertos);

	int contador = 0;

	Padrao *Padroes_ret;

	for (auto elemento : conjunto2) {
		if (elemento.n_cobertos == elemento.k) {
			conjunto.push_back(elemento);
			contador++;
		}
		/*if (contador > P / 10)
		break;*/
	}

	conjunto.push_front(Padroes_Par[0]);
	conjunto.unique();

	cout << conjunto.size() << endl;

	return conjunto;
}

void Problema_Vigas::Substituir_Padroes(list<Padrao> lista) {
	P_antigo = P;
	delete Pattern;
	Pattern = new Padrao[lista.size()];
	P = lista.size();
	int i = 0;
	for (auto elemento : lista) {
		Pattern[i] = elemento;
		i++;
	}
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//CPLEX PART------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
void Problema_Vigas::iniciar_variaveis() {

	//z = IloIntVar(env, 0, T);
	char strnum[30];

	z = IloBoolVarArray(env, T);
	for (int t = 0; t < T; t++) {
		sprintf(strnum, "z(%d)", t);
		z[t].setName(strnum);
	}

	int contador = 0;

	for (int i = 0; i < P; i++)
		for (int m = 0; m < M; m++)
			for (int t = 0; t < T; t++)
				if (maximal(Pattern[i], c_[m]) || i == 0)
					contador++;


	numvar = P*M*T;
	numvarP = contador;

	//system("pause");
	x = IloArray<IloArray<IloBoolVarArray> >(env, P);//P*M*T);
	int h = 0;
	//char strnum[30];

	//nome bonitinho para variavel
	for (IloInt i = 0; i < P; i++) {
		x[i] = IloArray<IloBoolVarArray>(env, M);
		for (IloInt m = 0; m < M; m++) {
			x[i][m] = IloBoolVarArray(env, T);
			for (IloInt t = 0; t < T; t++) {
				sprintf(strnum, "x(%d,%d,%d)", i, m, t);
				x[i][m][t].setName(strnum);
			}
		}
	}

}


void Problema_Vigas::CALCULAR_LB() {
	double soma = 0;

	for (int i = 0; i < C; i++) {
		for (int kachan = 0; kachan < Viga[i].k; kachan++) {
			soma += Viga[i].d[kachan] * Viga[i].l[kachan] * Viga[i].e;
		}
	}

	cout << "\n\t\t Minimo de tempos" << soma / 60 << endl;

}

//lembrete: c_ eh um vetor de double
void Problema_Vigas::funcao_objetivo() {
	IloInt m, i, t;

	IloExpr costSum(env);

	for (m = 0; m < M; m++)
		for (t = 0; t < T; t++)
			for (i = 1; i < P; i++)
				if (Pattern[i].cap <= c_[m] && maximal(Pattern[i], c_[m]))
					costSum += Viga[Pattern[i].tipo].e * (c_[m] - Pattern[i].cap) * x[i][m][t];


	model.add(IloMinimize(env, costSum)).setName("FO");
	costSum.end();
}


void Problema_Vigas::funcao_objetivo2() {
	IloInt t;

	IloExpr costSum(env);
	for (t = 0; t < T; t++)
		costSum += z[t];

	model.add(IloMinimize(env, costSum)).setName("FO#2");
	costSum.end();
	//model.add(IloMinimize(env, z)).setName("FO#2");
}

void Problema_Vigas::funcao_objetivo3() {
	IloInt m, t, i;

	IloExpr FO(env);
	for (m = 0; m < M; m++)
		for (t = 0; t < T; t++) {
			for (i = 1; i < P; i++)
				if (Pattern[i].cap <= c_[m] && maximal(Pattern[i], c_[m]))
					FO += x[i][m][t];
			FO += x[0][m][t];
		}

	model.add(IloMinimize(env, FO)).setName("FO");

	FO.end();
}
void Problema_Vigas::restricoes_onlyone() {
	IloInt m, t, i;
	//para cada forma m e periodo de tempo t
	for (m = 0; m < M; m++) {
		for (t = 0; t < T; t++) {
			IloExpr expr(env);
			expr += x[0][m][t];
			for (i = 1; i < P; i++) {
				if ((Pattern[i].cap <= c_[m]) && maximal(Pattern[i], c_[m])) {
					expr += x[i][m][t];
				}
			}
			model.add(expr <= 1).setName("Um Padrao");
			expr.end();
		}
	}
}
void Problema_Vigas::restricoes_demanda() {

	IloInt c, k, m, t, i;
	//Para cada tipo de viga e tamanhos dentro do tipo de viga
	for (c = 0; c < C; c++) {
		for (k = 0; k < Viga[c].k; k++) {
			//sum
			IloExpr expr(env);
			//problema: alguns problemas não estão gerando padrões com a última demanda
			for (m = 0; m < M; ++m) {
				for (i = 1; i < P; ++i) {
					for (t = 0; t < T - Viga[Pattern[i].tipo].e + 1; ++t) {
						if (Pattern[i].cap <= c_[m] && Pattern[i].tipo == c && maximal(Pattern[i], c_[m])) {
							expr += Pattern[i].tamanhos[k] * x[i][m][t];
						}
					}
				}
			}
			model.add(expr >= Viga[c].d[k]).setName("Demanda");
			expr.end();
		}
	}
}

void Problema_Vigas::restricoes_sequenciamento() {
	int i, j, m, a, t;
	for (m = 0; m < M; ++m)
		for (i = 1; i < P; ++i)
			if (Pattern[i].cap <= c_[m] && maximal(Pattern[i], c_[m]))
				for (t = 0; t < T - Viga[Pattern[i].tipo].e + 1; t++) {

					if (Viga[Pattern[i].tipo].e != 1) {
						IloExpr expr(env);
						for (a = 1; a <= Viga[Pattern[i].tipo].e - 1; a++)
							expr += x[0][m][t + a];
						model.add((Viga[Pattern[i].tipo].e - 1) * x[i][m][t] <= expr).setName("problema");
						expr.end();
					}

				}


	int R = 0;
	for (int c = 0; c < C; c++)
		if (Viga[c].e > R)
			R = Viga[c].e;

	for (t = 0; t < T; t++)
		for (m = 0; m < M; m++) {
			IloExpr expr(env);

			for (int beta = 2; beta <= R; beta++)
				for (j = beta; j <= R; j++)
					for (i = 0; i < P; i++)
						if (Pattern[i].cap <= c_[m] && maximal(Pattern[i], c_[m]) && Viga[Pattern[i].tipo].e == j && t - beta + 1 >= 0)
							expr += x[i][m][t - beta + 1];

			model.add(x[0][m][t] <= expr).setName("oi");
			expr.end();
		}
}

void Problema_Vigas::restricoes_continuidade() {
	IloInt m, t, i;

	for (m = 0; m < M; m++) {
		for (t = 0; t < T - 1; t++) {
			IloExpr expr1(env), expr2(env);

			for (i = 0; i < P; i++)
				if (maximal(Pattern[i], c_[m]) && (Pattern[i].cap <= c_[m]) || i == 0)
					expr1 += x[i][m][t];
			for (i = 0; i < P; i++)
				if (maximal(Pattern[i], c_[m]) && (Pattern[i].cap <= c_[m]) || i == 0)
					expr2 += x[i][m][t + 1];

			model.add(expr1 >= expr2);
			expr1.end();
			expr2.end();
		}
	}
}

void Problema_Vigas::restricoes_z() {
	IloInt t, m, i;

	for (t = 0; t < T; t++)
	{
		IloExpr expr(env);

		for (m = 0; m < M; m++) {
			for (i = 1; i < P; i++)
				if (maximal(Pattern[i], c_[m]) && (Pattern[i].cap <= c_[m]))
					expr += x[i][m][t];
			expr += x[0][m][t];
		}

		model.add(M*z[t] >= expr);
		expr.end();
	}
	/*for (t = 0; t < T; t++)
	{


	for (m = 0; m < M; m++) {
	IloExpr expr(env);
	for (i = 1; i < P; i++)
	if (maximal(Pattern[i], c_[m]) && (Pattern[i].cap <= c_[m]))
	expr += x[i][m][t];
	expr += x[0][m][t];

	model.add(z >= (t + 1) * expr);
	expr.end();
	}
	}*/
}

void Problema_Vigas::simetria() {
	IloInt i, m, t, alpha;
	IloExpr soma1(env), soma2(env);
	for (m = 0; m < M; m++)
		for (t = 0; t < T - 1; t++) {
			for (i = 1; i < P; i++)
				if ((Pattern[i].cap <= c_[m]) && maximal(Pattern[i], c_[m]))
					soma1 += i*x[i][m][t];



			for (alpha = 1; alpha < T - t; alpha++) {
				for (i = 1; i < P; i++)
					if ((Pattern[i].cap <= c_[m]) && maximal(Pattern[i], c_[m]))
						soma2 += i*x[i][m][t + alpha];

				model.add(soma1 <= soma2 + P * x[0][m][t + alpha]);
				soma2.clear();
			}

			soma1.clear();
		}


	for (m = 0; m < M - 1; m++) {
		for (t = 0; t < T; t++)
			for (i = 1; i < P; i++)
				if ((Pattern[i].cap <= c_[m]) && maximal(Pattern[i], c_[m]))
					soma1 += i*(t + 1)*x[i][m][t];
		for (alpha = 1; alpha < M - m; alpha++) {
			for (t = 0; t < T; t++)
				for (i = 1; i < P; i++)
					if ((Pattern[i].cap <= c_[m]) && maximal(Pattern[i], c_[m + alpha]))
						soma2 += i*(t + 1)*x[i][m + alpha][t];

			model.add(soma1 <= soma2);
			soma2.clear();
		}
		soma1.clear();
	}


}

void Problema_Vigas::exportar_lp() {
	cplex.exportModel("problema_vigas.lp");
}

void Problema_Vigas::resolver_linear() {
	relaxacaolinear = true;
	IloModel relax(env);
	relax.add(model);
	for (IloInt i = 0; i < P; i++) {
		for (IloInt m = 0; m < M; m++) {
			relax.add(IloConversion(env, x[i][m], ILOFLOAT));
		}
	}

	relax.add(IloConversion(env, z, ILOFLOAT));

	cplex = IloCplex(relax);
	//exportar_lp();
	if (!cplex.solve()) {
		env.error() << "Otimizacao do LP mal-sucedida." << endl;
		throw(-1);
	}
}
void Problema_Vigas::revolver_ppl() {
	//cplex.setParam(IloCplex::PreInd, 0); Desligar presolve(NAO FACA ISSO DE NOVO!)
	relaxacaolinear = false;
	cout << "Numero de padroes maximais: " << P_antigo << endl;
	cout << "Numero de padroes maximais que cobrem todos: " << P << endl << endl;
	cplex.setParam(IloCplex::TiLim, 600);
	//cplex.setParam(IloCplex::Param::MIP::Cuts::Cliques, -1);
	if (!cplex.solve()) {
		env.error() << "Otimizacao do LP mal-sucedida." << endl;
		throw(-1);
	}
}


void Problema_Vigas::imprimir_solucao(ofstream& resultados) {
	if (relaxacaolinear)
		resultados << "	" << P_antigo << "	" << P << "	" << cplex.getObjValue();
	else
		resultados << "	" << cplex.getObjValue() << "	" << cplex.getNnodes() << "	" << cplex.getMIPRelativeGap();

	double sobra = 0;



	//return;
	cplex.out() << "Status da solucao = " << cplex.getStatus() << endl;
	cplex.out() << "Valor Otimo  = " << cplex.getObjValue() << endl;
	cplex.out() << "#Iteracoes = " << cplex.getNiterations() << endl;
	cplex.out() << "#Nos de BB  = " << cplex.getNnodes() << endl;
	cout << "\n     Funcao Objetivo: " << cplex.getObjValue() << endl;

	bool imprimir = 0;
	if (imprimir) {
		int contador = 1;
		for (int t = 0; t < T; t++) {
			for (int m = 0; m < M; m++) {
				for (int i = 0; i < P; i++) {

					if (maximal(Pattern[i], c_[m]) || i == 0) {
						if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) > 0.00001) {
							cout << "     x(" << i << "," << m << "," << t << ") = " << cplex.getValue(x[i][m][t]) << "		";
							if (contador % 2 == 0)
								cout << endl;
							contador++;
						}
					}
				}
			}
		}
	}
	cout << "\n\n";
	//se for relaxação linear não imprimir Gantt
	if (relaxacaolinear)
		return;

	//cout << "Variaveis sem preprocessamento: " << numvar << endl;
	//cout << "Variaveis com preprocessamento: " << numvarP << endl << endl;

	// usando a biblioteca "#include <iomanip>"
	char xu[100];
	strcpy(xu, instancia_nome);
	strcat(xu, ".solu");
	ofstream txtsolu;
	txtsolu.open(xu, fstream::trunc);





	txtsolu << endl;
	//imprimir sobra = 1, padrao = 0, tipo do padrao = 2?
	int gantt = 0;
	//--------------
	
	txtsolu << 1 << "," << 0 << "," << T << ",Type 0" << endl;
	for (int m = 0; m < M; m++) {
		bool usou = false;
		for (int t = 0; t < T; t++)
			for (int i = 1; i < P; i++)
				if (maximal(Pattern[i], c_[m]) && cplex.isExtracted(x[i][m][t]) && (cplex.getValue(x[i][m][t]) == 1)) {
					txtsolu << m + 1 << "," << t + 0.01 << "," << t + Viga[Pattern[i].tipo].e - 0.01 << ",Type " << Pattern[i].tipo + 1 << endl;
					usou = true;
				}
		if (!usou)
			txtsolu << m + 1 << "," << 0 << "," << T << ",Type 0" << endl;
	}

	txtsolu << "  \"Gantt\" " << endl << endl;
	const char separator = ' ';
	const char separator2 = '_';
	const int nameWidth = 6;
	//const int numWidth      = 8;

	for (int t = 0; t <= T; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator2) << separator2;
	txtsolu << endl;
	txtsolu << internal << setw(nameWidth) << setfill(separator) << separator;
	for (int t = 0; t < T; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator) << t;
	txtsolu << endl;


	for (int m = 0; m < M; m++) {
		txtsolu << internal << setw(nameWidth) << setfill(separator) << m;
		for (int t = 0; t < T; t++) {
			bool nenhum = 1;
			for (int i = 0; i < P; i++) {
				if ((maximal(Pattern[i], c_[m]) || i == 0) && cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) > 0.00001) {
					nenhum = 0;
					if (i == 0) {
						txtsolu << internal << setw(nameWidth) << setfill(separator) << "*";
						break;
					}
					else {

						switch (gantt)
						{
						case 0:
							txtsolu << internal << setw(nameWidth) << setfill(separator) << Pattern[i].id; //imprimindo numero do padrao
							break;
						case 1:
							//imprimindo sobra
							if (i == 0)
								txtsolu << internal << setw(nameWidth) << setfill(separator) << "*";
							else
								txtsolu << internal << setw(nameWidth) << setfill(separator) << c_[m] - Pattern[i].cap;
							break;
						case 2:
							if (i == 0)
								txtsolu << internal << setw(nameWidth) << setfill(separator) << "*";
							else
								txtsolu << setprecision(2) << internal << setw(nameWidth) << setfill(separator) << Pattern[i].cap / c_[m];
							break;
						}
					}
				}
			}
			if (nenhum)
				txtsolu << internal << setw(nameWidth) << setfill(separator) << " ";
		}
		txtsolu << endl;
	}

	for (int t = 0; t <= T; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator2) << separator2;
	txtsolu << endl << endl;


	txtsolu << "Vigas excedentes: " << endl << endl;

	for (int c = 0; c < C; ++c) {
		txtsolu << "Tipo " << c << endl;
		for (int k_ = 0; k_ < Viga[c].k; k_++) {
			int soma = 0;

			for (int t = 0; t < T; t++)
				for (int m = 0; m < M; m++)
					for (int i = 1; i < P; i++)
						if (Pattern[i].tipo == c)
							if (maximal(Pattern[i], c_[m]))
								if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) == 1)
									soma += Pattern[i].tamanhos[k_];
			txtsolu << " " << soma - Viga[c].d[k_];
		}
		txtsolu << endl;
	}

	for (int t = 0; t < T; t++)
		for (int m = 0; m < M; m++)
			for (int i = 1; i < P; i++)
				if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) == 1)
					sobra += Viga[Pattern[i].tipo].e * (c_[m] - Pattern[i].cap) * cplex.getValue(x[i][m][t]);
	txtsolu << "\n sobra=" << sobra << endl;

	sobra = 0;
	for (int t = 0; t < T; t++)
		for (int m = 0; m < M; m++)
			for (int i = 1; i < P; i++)
				if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) == 1)
					sobra += (c_[m] - Pattern[i].cap) * cplex.getValue(x[i][m][t]);
	txtsolu << "\n sobra2=" << sobra << endl;

	double *sobra_dia = new double[T];
	for (int t = 0; t < T; t++)
		sobra_dia[t] = 0;


	for (int t = 0; t < T; t++) {
		for (int m = 0; m < M; m++) {
			for (int i = 1; i < P; i++)
				if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) == 1) {
					for (int a = 0; a < Viga[Pattern[i].tipo].e; a++)
						sobra_dia[t + a] += (c_[m] - Pattern[i].cap) * cplex.getValue(x[i][m][t]);
				}
		}
	}
	for (int t = 0; t < T; t++)
		txtsolu << "\n sobra dia" << t << "=" << sobra_dia[t] << endl;

	delete sobra_dia;
	txtsolu.close();


	if (verificacao())
		cout << "\n\nSolucao valida para instancia!\n\n";
	else
		cout << "\n\nSolucao nao válida para instancia!\n\n";
}


//----------------------------------------------------------------------
bool Problema_Vigas::maximal(const Padrao P, double C_FORMA) {
	//return true;
	double menor = 10000;
	for (int aux = 0; aux < Viga[P.tipo].k; aux++)
		if (Viga[P.tipo].l[aux] < menor) menor = Viga[P.tipo].l[aux];
	//cout << "------" <<C_FORMA << " - " << P.cap << " < ? " << menor << endl;
	return (C_FORMA - P.cap) < menor;


}



//pseudo-funcao de verificacao
bool Problema_Vigas::verificacao() {
	//----------------------------------------------------------------------
	//obedece sequenciamento?
	int h = 0;
	for (int m = 0; m < M; m++)
		for (int t = 0; t < T; t++)
			for (int i = 1; i < P; i++)
				if (maximal(Pattern[i], c_[m]))
					if (cplex.isExtracted(x[i][m][t]) && (cplex.getValue(x[i][m][t]) == 1))
						if (Viga[Pattern[i].tipo].e > 1 && (t + Viga[Pattern[i].tipo].e) < T)
							for (int a = 1; a < Viga[Pattern[i].tipo].e; a++)
								if (cplex.getValue(x[0][m][t + a]) != 1)
									return false;



	//apenas um padrão??
	for (int m = 0; m < M; m++) {
		for (int t = 0; t < T; t++) {
			int soma = 0;
			for (int i = 0; i < P; i++) {
				if (maximal(Pattern[i], c_[m]) || i == 0)
					if (cplex.isExtracted(x[i][m][t]) && cplex.getValue(x[i][m][t]) == 1)
						soma++;
			}
			if (soma > 1)
				return false;
		}
	}
	//demanda??


	//Para cada tipo de viga e tamanhos dentro do tipo de viga
	for (int c = 0; c < C; c++) {

		for (int k = 0; k < Viga[c].k; k++) {
			int soma = 0;
			//problema: alguns problemas não estão gerando padrões com a última demanda
			for (int m = 0; m < M; ++m)
				for (int i = 1; i < P; ++i)
					for (int t = 0; t < T - Viga[Pattern[i].tipo].e + 1; ++t)
						if (Pattern[i].cap <= c_[m] && Pattern[i].tipo == c && maximal(Pattern[i], c_[m]))
							soma += Pattern[i].tamanhos[k] * cplex.getValue(x[i][m][t]);

			//cout << "soma " << soma << " -- " << Viga[c].d[k] << endl;
			//getchar();
			if (soma < Viga[c].d[k])
				return false;

		}

	}


	return true;
}

void Problema_Vigas::iniciar_lp(int fo, ofstream& resultados) {
	try {
		relaxacaolinear = false;

		model = IloModel(env);

		//iniciar_variaveis();

		switch (fo)
		{
		case 1:
			funcao_objetivo();
			break;
		case 2:
			funcao_objetivo2();
			restricoes_z();
			restricoes_continuidade();
			break;
		case 3:
			funcao_objetivo3();
			restricoes_continuidade();
			break;
		}


		restricoes_onlyone();

		restricoes_demanda();

		restricoes_sequenciamento();

		//simetria();

		cout << "Olar" << endl;
		cplex = IloCplex(model);


	}
	catch (IloException& e) {
		cerr << "Erro: " << e.getMessage() << endl;
		cout << "\nErro ilocplex" << endl;
		return;
	}
	catch (...) {
		cerr << "Outra excecao" << endl;
		cout << "\noutra excecao" << endl;
	}
}


Problema_Vigas::~Problema_Vigas()
{
}


void Problema_Vigas::RODAR(int fo) {
	ofstream resultados;
	double time;
	resultados << std::fixed;
	resultados << std::setprecision(4);
	resultados.open("resultados.txt", fstream::app);
	resultados << endl;
	resultados << instancia_nome << "\t";
	try {
		iniciar_variaveis();

		
		

		iniciar_lp(fo, resultados);
		//exportar_lp();                   //criar arquivo .lp
		cout << "\n\n\nResolvendo Linear... \n\n";
	
		auto TEMPO_COMECO = chrono::high_resolution_clock::now();
		resolver_linear();                    //resolver problema
		auto TEMPO_FIM = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed = TEMPO_FIM - TEMPO_COMECO;
		cout << "\n\nTempo Resolucao do CPLEX gasto (Linear): " << elapsed.count()
			<< endl;

		imprimir_solucao(resultados);
		resultados << "	" << elapsed.count();
	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da linear" << endl;
	}


	resultados.close();
	//env.end();
	resultados.open("resultados.txt", fstream::app);

	try {
		iniciar_variaveis();

		iniciar_lp(fo, resultados);
		//exportar_lp();                   //criar arquivo .lp
		cout << "\n\n\nResolvendo Inteira... \n\n";
		auto TEMPO_COMECO = chrono::high_resolution_clock::now();
		revolver_ppl();                    //resolver problema
		auto TEMPO_FIM = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed = TEMPO_FIM - TEMPO_COMECO;

		imprimir_solucao(resultados);
		resultados << "	" << elapsed.count();
		cout << "\n\nTempo Resolucao do CPLEX gasto (Solucao Inteira): " << elapsed.count()
			<< endl;
	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da inteira" << endl;
	}


	resultados << endl;

	resultados.close();
}


/*Só funciona se for com todos os padrões*/
void Problema_Vigas::function_Solucao_Arquivo_Heuristicas(list<OPERACAO> solucao_heuristica,
	const char* nome_saida) {

	string arquivo_saida = ".solu";

	stringstream ss;
	ss << instancia_nome <<"_"<< nome_saida << arquivo_saida;
	arquivo_saida = ss.str();

	ofstream txtsolu;
	txtsolu.open(arquivo_saida, fstream::trunc);

	txtsolu << "  \"Gantt\" " << endl << endl;
	const char separator = ' ';
	const char separator2 = '_';
	const int nameWidth = 6;
	//const int numWidth      = 8;



	/*Associando o id dos padrões das heurísticas aos padrões que estão no arquivo de padrões*/

	for (auto &elemento : solucao_heuristica) {
		for (int i = 1; i < P; i++) {
			if (Pattern[i].tipo != elemento.PADRAO_OP.tipo)
				continue;

			int k;
			for (k = 0; k < elemento.PADRAO_OP.k; k++) {
				if (Pattern[i].tamanhos[k] != elemento.PADRAO_OP.tamanhos[k])
					break;
			}
			if (k == elemento.PADRAO_OP.k)
				elemento.PADRAO_OP.id = i;
		}
	}

	vector<int> FORMAS_ACUM(M);
	for (int m = 0; m < M; m++)
		FORMAS_ACUM[m] = 0;
	int T_folgado = T + .4*T;
	vector<vector<int>> Matrix_X(M);
	for (int i = 0; i < M; i++)
		Matrix_X[i].resize(T_folgado);

	for (int m = 0; m < M; m++) {
		for (int t = 0; t < T_folgado; t++)
			Matrix_X[m][t] = -1;
	}


	//povoar a matriz X
	for (auto &elemento : solucao_heuristica) {
		Matrix_X[elemento.FORMA][elemento.TEMPO] = elemento.PADRAO_OP.id;

		for (int t = 1; t < Viga[elemento.PADRAO_OP.tipo].e; t++)
			Matrix_X[elemento.FORMA][elemento.TEMPO + t] = 0;

	}

	//Imprimindo o Gantt
	for (int t = 0; t <= T_folgado; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator2) << separator2;
	txtsolu << endl;
	txtsolu << internal << setw(nameWidth) << setfill(separator) << separator;
	for (int t = 0; t < T_folgado; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator) << t;
	txtsolu << endl;
	for (int t = 0; t <= T_folgado; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator2) << separator2;
	txtsolu << endl << endl;


	for (int m = 0; m < M; m++) {
		txtsolu << internal << setw(nameWidth) << setfill(separator) << m;
		for (int t = 0; t < T_folgado; t++) {

			if(Matrix_X[m][t] != -1)
				txtsolu << internal << setw(nameWidth) << setfill(separator) << Matrix_X[m][t];
			else
				txtsolu << internal << setw(nameWidth) << setfill(separator) << " ";
		}
		txtsolu << endl;
	}


	for (int t = 0; t <= T_folgado; t++)
		txtsolu << internal << setw(nameWidth) << setfill(separator2) << separator2;
	txtsolu << endl << endl;




	txtsolu << "Vigas excedentes: " << endl << endl;

	for (int c = 0; c < C; ++c) {
		txtsolu << "Tipo " << c << endl;
		for (int k_ = 0; k_ < Viga[c].k; k_++) {
			int soma = 0;

			for (auto &elemento : solucao_heuristica) {
				if (elemento.PADRAO_OP.tipo == c && elemento.PADRAO_OP.id > 0)
					soma += elemento.PADRAO_OP.tamanhos[k_];
			}

			txtsolu << " " << soma - Viga[c].d[k_];
		}
		txtsolu << endl;
	}


	double sobra = 0;

	for (int t = 0; t < T_folgado; t++) {
		for (int m = 0; m < M; m++)
			if (Matrix_X[m][t] > 0)
				sobra += Viga[Pattern[Matrix_X[m][t]].tipo].e * (c_[m] - Pattern[Matrix_X[m][t]].cap);
	}
	txtsolu << "\n sobra=" << sobra << endl;

	sobra = 0;
	for (int t = 0; t < T_folgado; t++) {
		for (int m = 0; m < M; m++)
			if (Matrix_X[m][t] > 0)
				sobra += (c_[m] - Pattern[Matrix_X[m][t]].cap);
	}
	txtsolu << "\n sobra2=" << sobra << endl;

	double *sobra_dia = new double[T_folgado];
	for (int t = 0; t < T_folgado; t++)
		sobra_dia[t] = 0;


	for (int t = 0; t < T_folgado; t++) {
		for (int m = 0; m < M; m++) {
			if (Matrix_X[m][t] > 0) {
				for (int a = 0; a < Viga[Pattern[Matrix_X[m][t]].tipo].e; a++)
					sobra_dia[t + a] += (c_[m] - Pattern[Matrix_X[m][t]].cap);
			}
		}
	}
	for (int t = 0; t < T_folgado; t++)
		txtsolu << "\n sobra dia" << t << "=" << sobra_dia[t] << endl;


	txtsolu << 1 << "," << 0 << "," << T << ",Type 0" << endl;
	for (int m = 0; m < M; m++) {
		bool usou = false;
		for (int t = 0; t < T; t++)
			if (Matrix_X[m][t] > 0) {
				txtsolu << m + 1 << "," << t + 0.01 << "," << t + Viga[Pattern[Matrix_X[m][t]].tipo].e - 0.01
					<< ",Type " << Pattern[Matrix_X[m][t]].tipo + 1 << endl;
				usou = true;
			}

		if (!usou)
			txtsolu << m + 1 << "," << 0 << "," << T << ",Type 0" << endl;
	}


	txtsolu.close();
}
