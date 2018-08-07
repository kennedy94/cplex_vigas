#include "Problema_Vigas.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
	char *inst;
	if (argc < 2)
		inst = "problema.txt";
	else {
		if (argc < 3)
			inst = argv[1];
		else {
			cout << "Argumentos demais" << endl;
			exit(0);
		}
	}
	char xu[100];
	strcpy(xu, inst);
	strcat(xu, ".pat");

	Problema_Vigas	Prob(inst, xu);

	//Fazer reduction size
	Prob.Substituir_Padroes(Prob.gerar_conj(Prob.get_Padroes()));
	Prob.RODAR(3);
	cout << "Instância \t" << inst << endl;
	Prob.imprimir_resultados();
	/*Prob.CALCULAR_LB();
	Prob.HEURISTIQUE_PLUS_VITE_PLUS_PETITES();
	Prob.HEURISTIQUE_PLUS_VITE_PLUS_GROSSES();
	Prob.HEURISTIQUE_MOINS_VITE_PLUS_PETITES();
	Prob.HEURISTIQUE_MOINS_VITE_PLUS_GROSSES();
	Prob.HEURISTIQUE_PLUS_VITE_ALTERNE();
	Prob.HEURISTIQUE_MOINS_VITE_ALTERNE();*/
	
	//getchar();
	return 0;
}

