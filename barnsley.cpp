#include <stdlib.h>
#include <string.h>
#include "barnsley.h"

Barnsley::Barnsley(int size_v) {
    this->size = size_v;
}

Barnsley::Barnsley(const Barnsley& orig) {
}

Barnsley::~Barnsley() {
}

double** Barnsley::generate() {
    double x;
    double y;    
    double *xtab = (double*) malloc(size * sizeof (double));
    double *ytab = (double*) malloc(size * sizeof (double));
    double *tab = (double*) malloc(2 * sizeof (double));
    x = 10;
    y = 10;
    //Generacja danych fraktala
    for (int i = 0; i < size; i++) {
        xtab[i] = x;
        ytab[i] = y;
        int los = rand() % 100;
        if (los < 85) {
            tab = f(x, y);
        }
        if (los > 85 && los < 92) {
            tab = g(x, y);
        }
        if (los > 92 && los < 99) {
            tab = h(x, y);
        }
        if (los == 99) {
            tab = j(x, y);
        }
        x = tab[0];
        y = tab[1];
    }
    double** output = (double**)malloc(2*sizeof(double*));
    output[0] = xtab;
    output[1] = ytab;
    return output;
}

double* Barnsley::f(double x,double y) {
        double* out;
        out = (double*)malloc(2*sizeof(double));
        out[0] = 0.85*x + 0.04*y; 
        out[1] = -0.04*x + 0.85*y + 1.6;
        return out;
}

double* Barnsley::g(double x,double y) {
        double* out;
        out = (double*)malloc(2*sizeof(double));
        out[0] = -0.15*x + 0.28*y; 
        out[1] = 0.26*x + 0.24*y + 0.44;
        return out;
}

double* Barnsley::h(double x,double y) {
        double* out;
        out = (double*)malloc(2*sizeof(double));
        out[0] = 0.2*x - 0.26*y; 
        out[1] = 0.23*x + 0.22*y + 1.6;
        return out;
}

double* Barnsley::j(double x,double y) {
        double* out;
        out = (double*)malloc(2*sizeof(double));
        out[0] = 0;
        out[1] = 0.16*y;
        return out;
}

