

#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>
#include "../test/utility_mocks.h"
typedef long long poly_coeff_t;

typedef int poly_exp_t;


typedef struct Poly {
    struct List *list;
    bool isConst;
    long long coeff;
} Poly;


typedef struct Mono {
    Poly p; 
    poly_exp_t exp; 
} Mono;


typedef struct Node {
    struct Mono val;
    struct Node *right;
    struct Node *left;
    bool isDummy;
} Node;

typedef struct List {
    Node *head;
    Node *tail;
    int len;
} List;



static inline Poly PolyFromCoeff(poly_coeff_t c);



static inline Poly PolyZero() {
    return (Poly) {.list=NULL, .coeff=0, .isConst=true};
}

static inline Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly) {.list=NULL, .coeff=c, .isConst=true};
    
}


static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e) {
    return (Mono) {.p = *p, .exp = e};
}

static inline bool PolyIsCoeff(const Poly *p) {
    return p->isConst;
}


static inline bool PolyIsZero(const Poly *p) {
    return p->isConst && p->coeff == 0;
}


void PolyDestroy(Poly *p);


static inline void MonoDestroy(Mono *m){
    PolyDestroy(&m->p);
}

Poly PolyClone(const Poly *p);


static inline Mono MonoClone(const Mono *m){
    return (Mono) {.p=PolyClone(&m->p),.exp=m->exp};
};


Poly PolyAdd(const Poly *p, const Poly *q);


Poly PolyAddMonos(unsigned count, const Mono monos[]);


Poly PolyMul(const Poly *p, const Poly *q);


Poly PolyNeg(const Poly *p);


Poly PolySub(const Poly *p, const Poly *q);


poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);


 poly_exp_t PolyDeg(const Poly *p);


bool PolyIsEq(const Poly *p, const Poly *q);


Poly PolyAt(const Poly *p, poly_coeff_t x);


void printPolynomial(Poly p);



void PolyAddDest(Poly* newP, Poly* q);

#endif 


void PolyNegDest( Poly* p);


Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);
Poly PolyToExponent(Poly p, poly_exp_t exp);
