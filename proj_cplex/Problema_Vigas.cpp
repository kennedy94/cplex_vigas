
#include "Problema_Vigas.h"


Problema_Vigas::Problema_Vigas(const char* filename, const char* filename2) {

	//leitura da instancia ---------------------------------------------------
	ifstream instancia(filename, ifstream::in);
	if (instancia.fail()) {
		cerr << "     Arquivo \"" << filename << "\" nao encontrado." << endl;
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

bool Problema_Vigas::cobre(list<Padrao> conj, int tipo, int tamanho) {
	if (conj.empty())
		return false;
	for (auto elemento : conj)
		if (elemento.tipo == tipo & elemento.tamanhos[tamanho] > 0) 
			return true;
		
	return false;
}

list<Padrao> Problema_Vigas::gerar_conj(Padrao *Padroes_Par) {
	list<Padrao> conjunto;
	Padrao *Padroes_ret;
	for (int c = 0; c < C; c++){
		for (int tam = 0; tam < Viga[c].k; tam++){
			if (!cobre(conjunto, c, tam)) {
				for (int i = 0; i < P; i++){
					if (Padroes_Par[i].contem(tam) && Padroes_Par[i].tipo == c) {
						conjunto.push_back(Padroes_Par[i]);
					}
				}
			}
		}
	}

	conjunto.unique();
	cout << conjunto.size() << endl;
	
	return conjunto;
}

void Problema_Vigas::Substituir_Padroes(list<Padrao> lista) {
	delete Pattern;
	Pattern = new Padrao[lista.size()];
	int i = 0;
	for (auto elemento : lista) {
		Pattern[i] = elemento;
		i++;
	}
}

Problema_Vigas::~Problema_Vigas()
{
}
