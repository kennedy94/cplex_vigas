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

	//Fazer size reduction
	//Prob.Substituir_Padroes(Prob.gerar_conj(Prob.get_Padroes()));
	Prob.RODAR(3);
	cout << "Instância \t" << inst << endl;
	Prob.~Problema_Vigas();

	/*Problema_Vigas	Prob(inst, xu);
	Prob.imprimir_resultados_heuristicas();*/

	
	//getchar();
	return 0;
}

