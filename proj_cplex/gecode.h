#include <iostream>
#include <stdio.h>
#include <limits.h>
#include <list>
#include <cstdlib>

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/gist.hh>
#include <gecode/minimodel.hh>
#include <gecode/float.hh>


using namespace Gecode;
using namespace std;

class Tipo_Viga{

    const Tipo_Viga &operator=(const Tipo_Viga &obj){
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
public:
    int e, k;
    double *l;
    int *d;

    
};

class Problem : public Space {
 
protected:
    //Variables
    //Dados da instancia
    int C, M, T;
    double *c;
    
    int maior_k;        //Tamanho do X;
    int maior_demanda, x;   //Upper bound de Tamanhos;
    double maior_capacidade, menor_capacidade;
    double *menores_tamanhos;
    //Estrutura para guardar vetor de vigas
    
    //Gecode variables
    IntVarArray Padrao;
     
 
public:
    Tipo_Viga *Viga;
    
    int get_k0(){
        return Viga[0].k;
    }

    Problem(const char *filename) {
        // -----------------------------------------------------
        ler(filename);
        
        //Iniciar Variáveis
        initiate_variables();
        

        //Primeira variável do Padrao eh o tipo de viga
        rel(*this, Padrao[0] < C);
        
        //So existe vigas se ate o q_k do tipo de viga c
        for(int i = 0; i < C; i++){
            BoolVar lhs(*this, 0, 1);
			rel(*this, lhs == (Padrao[0] == i));
			Reify r(lhs, RM_IMP);

			IntVarArgs vars;
            for(int j = 1; j < maior_k+1; j++)
                if( j > Viga[i].k)
					vars << Padrao[j];
			linear(*this, vars, IRT_EQ, 0, r);
        }

        //Variavel auxiliar 
        FloatVarArray aux = FloatVarArray(*this, maior_k+1, 0, x);
 
        for(int i = 0; i < C; i++){
            FloatValArgs coefs;
            FloatVarArgs vars;

            for(int j = 1; j < Viga[i].k+1; j++){
                channel(*this, Padrao[j], aux[j]);
                vars << aux[j];
                coefs << Viga[i].l[j-1];
            }
            BoolVar lhs(*this, 0, 1);
            rel(*this, lhs == (Padrao[0] == i));

            Reify r(lhs, RM_IMP);
            //Upper bound da capacidade dos padroes
            //cout << menor_capacidade - menores_tamanhos[i] << endl;
            linear(*this,  coefs, vars, FRT_LQ, maior_capacidade, r);
            //Lower bound da capacidade dos padroes
            //cout << menor_capacidade - menores_tamanhos[i] << endl;
            linear(*this,  coefs, vars, FRT_GR, FLT_EPSILON, r); //FLT_EPSILON
        }
        

        branch(*this, Padrao, INT_VAR_SIZE_MIN(), INT_VAL_MIN());

    }
 
    void ler(const char *filename){
        // Leitura da instancia --------------------------------
        ifstream input(filename, ifstream::in);
        //Nao havera input fail aqui
        if (input.fail()) {
            cerr << "     File \"" << filename << "\" not found." << endl;
            system("pause");
            exit(0);
        }
        
        input >> C >> M >> T;
        //cout << C << " " << M << " " << T << endl;
        c = new double[M];
        Viga = new Tipo_Viga[C];
        maior_k = 0;
        maior_demanda = 0;
        menores_tamanhos = new double[C];
        double menor_tamanho = double(INT_MAX);

        for(int i = 0; i < C; ++i){
            input >> Viga[i].e >> Viga[i].k;
            //cout  << Viga[i].e << " " << Viga[i].k << endl;
            if(Viga[i].k > maior_k)
                maior_k = Viga[i].k;
            menores_tamanhos[i] = double(INT_MAX);
            Viga[i].l = new double[Viga[i].k];
            Viga[i].d = new int[Viga[i].k];
            for (int j = 0; j < Viga[i].k; j++){
                
                input >> Viga[i].l[j];


                //pega menor tamanho do tipo i
                if(menores_tamanhos[i] >= Viga[i].l[j]){
                    menores_tamanhos[i] = Viga[i].l[j];
                    if(menores_tamanhos[i] < menor_tamanho)
                        menor_tamanho = menores_tamanhos[i];
                }
                //cout << Viga[i].l[j] << " ";
            }//cout << endl;
            for (int j = 0; j < Viga[i].k; j++){
                input >> Viga[i].d[j];
                //cout << Viga[i].d[j] <<  " ";
                if(maior_demanda < Viga[i].d[j])
                    maior_demanda = Viga[i].d[j];
            }
            //cout << endl;
        }
        
        maior_capacidade = 0;
        menor_capacidade = double(INT_MAX);
        for (int i = 0; i < M; i++){
            input >> c[i];
            //cout << c[i] << " ";
            if(maior_capacidade < c[i])
                maior_capacidade = c[i];
            if(menor_capacidade > c[i])
                menor_capacidade = c[i];
         
        }
        x = int(maior_capacidade/menor_tamanho) + 1;
 
        input.close();
    }
 
    void initiate_variables(){
        Padrao = IntVarArray(*this, maior_k+1, 0, x);
    }
 
    Problem(bool share, Problem& s) : Space(share, s) {
        Padrao.update(*this, share, s.Padrao);
        C = s.C; M = s.M; T = s.T;
        c = new double[C];
        for(int i = 0; i < C; ++i)
            c[i] = s.c[i];
        Viga = s.Viga; //fazer construtor de copia
 
        maior_k = s.maior_k;
        maior_demanda = s.maior_demanda;
        x = s.x;    
        maior_capacidade = s.maior_capacidade;
        menor_capacidade = s.menor_capacidade;
 
    }
 
    ~Problem() {    
    }
 
    virtual Space * copy(bool share) {
        return new Problem(share, *this);
    }
    /*
    void imprimir() {
        cout << "Padrao: " << Padrao;
        double cap_usada = 0;
        for(int j = 0; j < Viga[Padrao[0].val()].k; j++)
            cap_usada += Viga[Padrao[0].val()].l[j] * Padrao[j+1].val();
        cout << " capacidade usada: " << cap_usada << endl;
    }*/

    void imprimir_lista(list<double> &LIST){
        for(int i = 0; i < Viga[Padrao[0].val()].k+1; ++i)
            LIST.push_back(double(Padrao[i].val()));
        /*
        double cap_usada = 0;

        for(int j = 0; j < Viga[Padrao[0].val()].k; j++)
            cap_usada += Viga[Padrao[0].val()].l[j] * Padrao[j+1].val();
            
        LIST.push_back(cap_usada);
        */
    }
    void imprimir_solu(ofstream &arq){
        for(int i = 0; i < Viga[Padrao[0].val()].k+1; ++i)
            arq << Padrao[i].val() << " ";

    }
};
