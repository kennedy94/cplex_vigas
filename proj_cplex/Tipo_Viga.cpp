#include "Tipo_Viga.h"


const Tipo_Viga & Tipo_Viga::operator=(const Tipo_Viga &obj) {
	if (this == &obj) return *this;

	comprimentos.clear();
	demandas.clear();

	tempo_cura = obj.tempo_cura;
	n_comprimentos = obj.n_comprimentos;

	comprimentos = vector<double>(n_comprimentos);
	demandas = vector<int>(n_comprimentos);

	comprimentos = obj.comprimentos;
	demandas = obj.demandas;

	return *this;
}
