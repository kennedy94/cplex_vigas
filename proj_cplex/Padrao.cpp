#include "Padrao.h"

bool operator== (const Padrao &c1, const Padrao &c2) {
	if (c1.tipo != c2.tipo) return false;

	for (int i = 0; i < c1.k; i++)
		if (c1.tamanhos[i] != c2.tamanhos[i]) return false;
	
	return true;
}
bool operator< (const Padrao &c1, const Padrao &c2) {
	return (c1.id < c2.id);
}

//Calcula o numero de tamanhos cobertos pelo padrao
void Padrao::gerar_cobertos(int k) {
	n_cobertos = 0;
	for (int i = 0; i < k; i++)
		if(tamanhos[i] > 0) n_cobertos++;
	
}
//Recebe o indice do tamanho e retorna se o Padrao o contem
bool Padrao::contem(int tam) {
	return (tamanhos[tam] > 0);
}


