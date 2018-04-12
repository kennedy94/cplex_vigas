
#include "Problema_Vigas.h"
void timeused(double *time)
{
	static double tstart, tend, tprev;

	if (time == NULL) {
		clock(); /* one extra call to initialize clock */
		tstart = tprev = clock();
	}
	else {
		tend = clock();
		if (tend < tprev)
			tstart -= ULONG_MAX; /* wraparound occured */
		tprev = tend;
		*time = (tend - tstart) / CLOCKS_PER_SEC; /* convert to seconds */
	}
}


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

list<Padrao> Problema_Vigas::gerar_conj(Padrao *Padroes_Par) {
	list<Padrao> conjunto;
	list<Padrao> conjunto2;
	
	for (int i = 0; i < P; i++)
		conjunto2.push_back(Padroes_Par[i]);
		
	conjunto2.sort(operador_padrao);	//ordena em ordem descrescente de numero de padroes cobertos

	Padrao *Padroes_ret;
	int contador = 0;
	//for (auto elemento : conjunto2) {
	//	if (elemento.n_cobertos == elemento.k){
	//		conjunto.push_back(elemento);
	//		contador++;
	//	}
	//	/*if (contador > P / 10)
	//		break;*/
	//}

	for (int c = 0; c < C; c++){
		for (int tam = 0; tam < Viga[c].k; tam++){
			for (auto elemento: conjunto2){
				if (cobre(conjunto, c, tam) < 50) {
					if (elemento.contem(tam) && elemento.tipo == c) {
						conjunto.push_back(elemento);
						contador++;
						//break;
					}
				}
			}
		}
	}
	

	/*for (int c = 0; c < C; c++) {
		for (int tam = 0; tam < Viga[c].k; tam++) {
			contador = 0;
			for (auto elemento : conjunto2) {
				if (elemento.contem(tam) && elemento.tipo == c) {
					bool contido = false;
					for (auto padrao_dentro : conjunto)
						if (elemento.id == padrao_dentro.id)
							contido = true;
					if (!contido) {
						conjunto.push_back(elemento);
						contador++;
					}
					if(contador > Viga[c].k*P/100)
						break;
				}
			}
		}
	}*/
	
	conjunto.push_front(Padroes_Par[0]);
	conjunto.unique();
	for (auto elemento : conjunto) {
		cout << elemento.tipo << "\t";
		for (int i = 0; i < elemento.k; i++)
			cout << elemento.tamanhos[i] << " ";
		cout << endl;
	}

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
	z = IloBoolVarArray(env, T);
	char strnum[30];
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
}

void Problema_Vigas::funcao_objetivo3() {
	IloInt m, t, i;

	IloExpr FO(env);
	for (m = 0; m < M; m++)
		for (t = 0; t < T; t++) {
			for (i = 1; i < P; i++)
				if (Pattern[i].cap <= c_[m] && maximal(Pattern[i], c_[m]))
					FO += x[i][m][t];
			FO + x[0][m][t];
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

	cplex = IloCplex(relax);
	exportar_lp();
	if (!cplex.solve()) {
		env.error() << "Otimizacao do LP mal-sucedida." << endl;
		throw(-1);
	}
}
void Problema_Vigas::revolver_ppl() {
	//cplex.setParam(IloCplex::PreInd, 0); Desligar presolve(NAO FACA ISSO DE NOVO!)

	cout << "Numero de padroes maximais: " << P_antigo << endl;
	cout << "Numero de padroes maximais que cobrem todos: " << P << endl << endl;
	cplex.setParam(IloCplex::TiLim, 3600);
	//cplex.setParam(IloCplex::Param::MIP::Cuts::Cliques, -1);
	if (!cplex.solve()) {
		env.error() << "Otimizacao do LP mal-sucedida." << endl;
		throw(-1);
	}
}
void Problema_Vigas::imprimir_solucao(ofstream& resultados) {
	if (relaxacaolinear)
		resultados << "	" << numvar << "	" << numvarP << "	" << cplex.getObjValue();
	else
		resultados << "	" << cplex.getObjValue() << "	" << cplex.getNnodes() << "	" << cplex.getMIPRelativeGap();


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




	//imprimir sobra = 1, padrao = 0, tipo do padrao = 2?
	int gantt = 0;
	//--------------
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
							txtsolu << internal << setw(nameWidth) << setfill(separator) << i; //imprimindo numero do padrao
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
						if (Viga[Pattern[i].tipo].e  > 1)
							for (int a = 1; a < Viga[Pattern[i].tipo].e - 1; t++)
								if (T >= t + a)
									if (cplex.getValue(x[0][m][t + a]) == 0)
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
			break;
		case 3:
			funcao_objetivo3();
			break;
		}


		restricoes_onlyone();

		restricoes_demanda();

		restricoes_sequenciamento();
		if (fo >= 2) {
			restricoes_continuidade();
			if(fo == 2)
				restricoes_z();
		}
		
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
	resultados.open("resultados.txt", fstream::app);
	double time;
	resultados << std::fixed;
	resultados << std::setprecision(4);
	resultados.open("resultados.txt", fstream::app);
	try {
		iniciar_variaveis();
		cout << "\n\n\nResolvendo Inteira... \n\n";
		iniciar_lp(fo, resultados);
		exportar_lp();                   //criar arquivo .lp

		timeused(NULL);
		revolver_ppl();                    //resolver problema
		timeused(&time);

		cout << "\n\nTempo Resolucao do CPLEX gasto (Solucao Inteira): " << time << endl;
		imprimir_solucao(resultados);
		resultados << "	" << time;
	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da inteira" << endl;
	}
	resultados << endl;

	resultados.close();
}
