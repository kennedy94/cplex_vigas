#include "Padrao.h"




bool operator== (const Padrao &c1, const Padrao &c2) {
	if (c1.tipo != c2.tipo) return false;

	for (int i = 0; i < c1.k; i++)
		if (c1.tamanhos[i] != c2.tamanhos[i]) return false;
	
	return true;
}
bool operator< (const Padrao &c1, const Padrao &c2) {
	return (c1.n_cobertos < c2.n_cobertos);
}

bool operator> (const Padrao &c1, const Padrao &c2) {
	return (c1.n_cobertos > c2.n_cobertos);
}

bool operador_padrao (const Padrao &c1, const Padrao &c2) {
	return (c1.n_cobertos > c2.n_cobertos);
}

bool operador_padrao_naocobertos(const Padrao &c1, const Padrao &c2) {
	return (c1.n_cobre_naocobertos > c2.n_cobre_naocobertos);
}




Padrao::Padrao() {
	id = -1;
	tipo = -1;
	n_cobertos = 0;
	k = 0;
	cap = 0;
}

void Padrao::contar() {
	n_cobertos = 0;
	for (int i = 0; i < k; i++)
		if (tamanhos[i] > 0) { n_cobertos++; }
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


void Padrao::alocar_PADRAO(int k, int tipo) {
	this->k = k;
	this->tipo = tipo;
	tamanhos = new int[k];
	for (int i = 0; i < k; i++)
		tamanhos[i] = 0;

}

bool Padrao::comparar_demandas(const Tipo_Viga & c1)
{
	if (c1.k != k)	return false;

	for (int i = 0; i < c1.k; i++) {
		if (c1.d[i] > tamanhos[i])
			return false;
		else
			continue;
	}
	return true;
}

bool Padrao::comparar_demandas(const Tipo_Viga & c1, int IND_TAMANHO)
{
	if (c1.k != k)	return false;

	if(c1.d[IND_TAMANHO] > tamanhos[IND_TAMANHO])
		return false;

	return true;
}


