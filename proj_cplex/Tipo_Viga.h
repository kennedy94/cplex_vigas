#pragma once
#include <iostream>
using namespace std;
class Tipo_Viga {
protected:
	const Tipo_Viga &operator=(const Tipo_Viga &obj);
public:
	int e, k;
	double *l;
	int *d;
};