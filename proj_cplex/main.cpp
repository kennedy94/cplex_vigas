#include "Problema_Vigas.h"
#include "Gerador_P.h"
#include <iostream>
using namespace std;

void gerar_padroes(char* inst);

int main(int argc, char *argv[]) {
	char *inst;
	if (argc < 2)
		inst = "Instances_P1_2020\\hbp1_15_1";
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
	//gerar_padroes(inst);

	Problema_Vigas	Prob(inst, xu);

	//Fazer size reduction
	Prob.Substituir_Padroes(Prob.gerar_conj(Prob.get_Padroes()));
	Prob.RODAR(2);
	Prob.~Problema_Vigas();

	//Problema_Vigas	Prob(inst, xu);
	//Prob.imprimir_resultados_heuristicas();

	
	//getchar();
	return 0;
}


void gerar_padroes(char* inst) {
	try
	{
		Gerador_P Prob(inst);
		Prob.Rodar_Packing();
		Prob.ImprimirPadrao_Packing();
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}

}