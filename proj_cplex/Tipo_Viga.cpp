#include "Tipo_Viga.h"


const Tipo_Viga & Tipo_Viga::operator=(const Tipo_Viga &obj) {
	if (this == &obj) return *this;

	delete l;
	delete d;

	e = obj.e;
	k = obj.k;

	l = new double[k];
	d = new int[k];

	memcpy(l, obj.l, sizeof(double) * k);
	memcpy(d, obj.d, sizeof(int) * k);
	return *this;
}