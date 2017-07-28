#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include "poly.h"
#define ADD_OPERATION 1
#define SUBTRACT_OPERATION 2
#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


void PolyAddHelper(Poly* newP, Poly* q);


List* getNewNodeList() {

    Node* head = calloc(1, sizeof(Node));
    head->isDummy = true;

    Node* tail = calloc(1, sizeof(Node));
    tail->isDummy = true;

    head->right = tail;
    tail->left = head;
    head->left = tail;
    tail->right = head;

    List* list = calloc(1, sizeof(List));
    list->head = head;
    list->tail = tail;

    list->len = 0;
    return list;
}


void addFromTail(List* list, Mono m) {
    Node* node = malloc(sizeof(Node));
    node->isDummy = false;
    node->val = m;

    node->left = list->tail->left;
    list->tail->left->right = node;

    list->tail->left = node;
    node->right = list->tail;

    list->len++;
    return;
};


void addFromHead(List* list, Mono m) {

    Node* node = malloc(sizeof(Node));
    node->isDummy = false;
    node->val = m;

    list->head->right->left = node;
    node->right = list->head->right;

    list->head->right = node;
    node->left = list->head;

    list->len++;
    return;
}


void freeList(List* list) {
    if (list == NULL)
        return;
    while (list->head->right->isDummy != true) {
        Node* toDelete = list->head->right;
        list->head->right = list->head->right->right;
        free(toDelete);
    }
    free(list->head);
    free(list->tail);
    free(list);

}


static void clearFromZeros(Poly* p) {
    if (p->list == NULL) {
        return;
    }
    Node* curNode = p->list->head->right;

    while (!curNode->isDummy) {
        clearFromZeros(&curNode->val.p);
        if (curNode->val.p.isConst && curNode->val.p.coeff == 0) {

            assert(curNode->val.p.list == NULL);
            curNode->right->left = curNode->left;
            curNode->left->right = curNode->right;
            p->list->len--;
            Node* toDelete = curNode;
            curNode = curNode->right;

            free(toDelete);
        } else {
            curNode = curNode->right;
        }
    }
    if (p->list->head->right->isDummy) {
        assert(p->list->len == 0);
        freeList(p->list);

        p->list = NULL;
        p->coeff = 0;
        p->isConst = true;
    } else {
        if (p->list->head->right->val.exp == 0 &&//zeroth power
            p->list->head->right->val.p.isConst && //with constant polynomial
            p->list->head->right->right->isDummy) { //only element
            assert(p->list->len == 1);
            p->isConst = true;
            p->coeff = p->list->head->right->val.p.coeff;

            freeList(p->list);
            p->list = NULL;
        }
    }
}

Poly PolyClone(const Poly* p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    Poly newPoly = (Poly) {.list=getNewNodeList(), .isConst=false, .coeff=0};
    Node* curNode = p->list->head->right;
    while (!curNode->isDummy) {
        addFromTail(newPoly.list, MonoClone(&curNode->val));

        curNode = curNode->right;
    }
    return newPoly;
};


static Mono MonoNeg(const Mono* m) {
    return (Mono) {.p=PolyNeg(&m->p), .exp=m->exp};
}

Poly PolyNeg(const Poly* p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(-p->coeff);
    }
    Poly newPolyNeg = (Poly) {.list=getNewNodeList(),
            .isConst=false, .coeff=0};
    Node* curNode = p->list->head->right;
    while (!curNode->isDummy) {
        addFromTail(newPolyNeg.list, MonoNeg(&curNode->val));
        curNode = curNode->right;
    }
    return newPolyNeg;
};

bool PolyIsEq(const Poly* p, const Poly* q) {
    Poly diff = PolySub(p, q);

    bool ans = PolyIsZero(&diff);
    PolyDestroy(&diff);
    return ans;
};

poly_exp_t PolyDegBy(const Poly* p, unsigned int ind) {
    if (PolyIsCoeff(p) && p->coeff == 0) {
        return -1;
    }
    if (p->list == NULL) {
        return 0;
    }
    if (ind == 0) {
        int maxVal = -1;
        Node* curNode = p->list->head->right;
        while (!curNode->isDummy) {
            maxVal = max(curNode->val.exp, maxVal);
            curNode = curNode->right;
        }
        return maxVal;
    }
    int maxVal = -1;
    Node* curNode = p->list->head->right;
    while (!curNode->isDummy) {
        poly_coeff_t val = PolyDegBy(&curNode->val.p, ind - 1);
        maxVal = max((int) val, maxVal);
        curNode = curNode->right;
    }
    return maxVal;

}


static Poly PolyOperation(const Poly* p, const Poly* q, int operation) {
    if (p->isConst && q->isConst) {
        if (operation == ADD_OPERATION) {
            return PolyFromCoeff(p->coeff + q->coeff);
        } else {
            return PolyFromCoeff(p->coeff - q->coeff);
        }
    }
    Poly pClone = PolyClone(p);
    Poly qClone;
    if (operation == ADD_OPERATION) {
        qClone = PolyClone(q);
    } else {
        qClone = PolyNeg(q);
    }
    PolyAddHelper(&pClone, &qClone);
    clearFromZeros(&pClone);
    PolyDestroy(&qClone);
    return pClone;

}


Poly PolySub(const Poly* p, const Poly* q) {
    return PolyOperation(p, q, SUBTRACT_OPERATION);
};

Poly PolyAdd(const Poly* p, const Poly* q) {
    return PolyOperation(p, q, ADD_OPERATION);
};

poly_exp_t PolyDeg(const Poly* p) {
    if (p->isConst && p->coeff == 0) {
        return -1;
    }
    if (p->list == NULL) {
        return 0;
    }
    Node* curNode = p->list->head->right;
    poly_exp_t maxVal = 0;
    while (!curNode->isDummy) {
        poly_exp_t v1 = PolyDeg(&curNode->val.p);
        maxVal = max(maxVal, v1 + curNode->val.exp);
        curNode = curNode->right;
    }
    return maxVal;
}


void PolyDestroy(Poly* p) {
    if (p->list == NULL) {
        return;
    }
    Node* curIterator = p->list->head->right;
    while (!curIterator->isDummy) {

        PolyDestroy(&curIterator->val.p);

        Node* toDelete = curIterator;
        curIterator = curIterator->right;

        toDelete->left->right = toDelete->right;
        toDelete->right->left = toDelete->left;
        free(toDelete);
    }
    freeList(p->list);
    p->list = NULL;

};


static inline int comparator(const void* m1, const void* m2) {
    Mono* mono1 = (Mono*) m1;
    Mono* mono2 = (Mono*) m2;
    if (mono1->exp - mono2->exp > 0) {
        return 1;
    }
    if (mono1->exp - mono2->exp < 0) {
        return -1;
    }
    return 0;
}

Poly PolyAddMonos(unsigned count, const Mono monos[]) {
    Mono* monoList = malloc(sizeof(Mono) * count);
    bool isconstPol = true;
    for (unsigned int i = 0; i < count; ++i) {
        monoList[i] = monos[i];
        if (!(monos[i].exp == 0 && PolyIsCoeff(&monos[i].p))) {
            isconstPol = false;
        }
    }
    if (isconstPol) {
        Poly res = PolyZero();
        for (unsigned int i = 0; i < count; ++i) {
            res.coeff = res.coeff + monos[i].p.coeff;
        }
        free(monoList);
        return res;
    }
    qsort(monoList, count, sizeof(Mono), comparator);
    poly_exp_t prev = -1;
    Poly p = (Poly) {.list=getNewNodeList(), .isConst=false, .coeff=0};
    for (unsigned int k = 0; k < count; ++k) {
        assert(prev <= monoList[k].exp);
        if (monoList[k].p.isConst && monoList[k].p.coeff == 0) {
            MonoDestroy(&monoList[k]);
            continue;
        }
        if (prev == monoList[k].exp) {
            Poly secArg = monoList[k].p;
            PolyAddHelper(&p.list->tail->left->val.p, &secArg);
            if (p.list->tail->left->val.p.isConst &&
                p.list->tail->left->val.p.coeff == 0 &&
                p.list->tail->left->val.p.list == NULL) {
                Node* toDelete = p.list->tail->left;
                toDelete->left->right = toDelete->right;
                toDelete->right->left = toDelete->left;
                free(toDelete);
                p.list->len--;
                prev = p.list->tail->left->val.exp;
            }


            PolyDestroy(&secArg);
        } else {
            addFromTail(p.list, monoList[k]);
            prev = monoList[k].exp;
        }
    }
    if (p.list != NULL && p.list->len == 0) {
        free(p.list->head);
        free(p.list->tail);
        free(p.list);
        p.list = NULL;
        p.coeff = 0;
        p.isConst = true;

        free(monoList);
        return p;
    }
    if (p.list->head->right->right == p.list->tail &&
        p.list->head->right->val.exp == 0 &&
        p.list->head->right->val.p.isConst) {
        p.coeff = p.list->head->right->val.p.coeff;
        p.isConst = true;
        freeList(p.list);
        p.list = NULL;
    }

    free(monoList);
    return p;


}

Poly PolyMul(const Poly* p, const Poly* q) {
    if (p->isConst && q->isConst) {
        return (Poly) {.isConst=true, .list=NULL, .coeff=p->coeff * q->coeff};
    }
    if (p->isConst) {
        if (p->coeff == 0) {
            return PolyZero();
        }
        List* l = getNewNodeList();
        Node* curNode = q->list->head->right;
        while (!curNode->isDummy) {
            addFromTail(l, (Mono) {.p=PolyMul(p, &curNode->val.p), .exp=curNode->val.exp});
            curNode = curNode->right;
        }
        return (Poly) {.list=l, .isConst=false, .coeff=0};
    }
    if (q->isConst) {
        if (q->coeff == 0) {
            return PolyZero();
        }
        List* l = getNewNodeList();
        Node* curNode = p->list->head->right;
        while (!curNode->isDummy) {
            addFromTail(l, (Mono) {.p=PolyMul(q, &curNode->val.p), .exp=curNode->val.exp});
            curNode = curNode->right;
        }
        return (Poly) {.list=l, .isConst=false, .coeff=0};
    }

    long pqlen = (p->list->len + 1) * (q->list->len + 1);

    Mono* monoList = malloc(sizeof(Mono) * (pqlen + 1));
    Node* curIteratorP = p->list->head->right;
    Node* curIteratorQ = q->list->head->right;
    int i = 0;
    while (!curIteratorP->isDummy) {
        Mono curMonoP = curIteratorP->val;

        while (!curIteratorQ->isDummy) {
            Mono curMonoQ = curIteratorQ->val;
            monoList[i] = (Mono) {.exp=curMonoP.exp + curMonoQ.exp,
                    .p=PolyMul(&curMonoP.p, &curMonoQ.p)};
            i++;
            curIteratorQ = curIteratorQ->right;
        }
        curIteratorQ = q->list->head->right;
        curIteratorP = curIteratorP->right;
    }
    Poly res = PolyAddMonos((unsigned int) i, monoList);
    free(monoList);
    return res;

};


static unsigned long ipow(unsigned long  base, unsigned long  exp) {
    unsigned long result = 1ULL;
    while (exp) {
        if (exp & 1) {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }
    return result;
}

Poly PolyAt(const Poly* p, poly_coeff_t x) {
    if (p->isConst) {
        return PolyFromCoeff(p->coeff);
    }

    assert(p->list != NULL);
    Node* curIteratorP = p->list->head->right;
    Poly* polyList = malloc(sizeof(Poly) * p->list->len);
    int i = 0;
    while (!curIteratorP->isDummy) {
        poly_coeff_t multiplier = (poly_coeff_t)
                ipow((unsigned long long int) x, (unsigned long long int) curIteratorP->val.exp);
        Poly mul2 = PolyFromCoeff(multiplier);
        polyList[i] = PolyMul(&curIteratorP->val.p, &mul2);
        i++;
        curIteratorP = curIteratorP->right;
    }
    Poly ans = polyList[0];
    int j = 1;
    while (j < i) {
        Poly toDelete = ans;
        ans = PolyAdd(&ans, &polyList[j]);
        PolyDestroy(&toDelete);
        PolyDestroy(&polyList[j]);
        j++;
    }
    free(polyList);
    return ans;

};

void PolyAddHelper(Poly* newP, Poly* q) {
    if (newP->isConst && q->isConst) {
        newP->coeff = newP->coeff + q->coeff;
        return;
    }
    if (newP->isConst) {
        newP->list = getNewNodeList();
        newP->isConst = false;
        addFromHead(newP->list, (Mono) {.exp=0, .p=PolyFromCoeff(newP->coeff)});
        newP->coeff = 0;
    }
    if (q->isConst) {
        q->list = getNewNodeList();
        q->isConst = false;
        addFromHead(q->list, (Mono) {.exp=0, .p=PolyFromCoeff(q->coeff)});
        q->coeff = 0;
    }
    Node* nodeNewP = newP->list->head->right;
    Node* nodeQ = q->list->head->right;
    while (!nodeQ->isDummy) {
        if (nodeNewP->isDummy) {
            addFromTail(newP->list, nodeQ->val);
            nodeQ->val.p.list = NULL;
            nodeQ->val.p.isConst = true;
            nodeQ = nodeQ->right;

            continue;
        }
        if (nodeNewP->val.exp == nodeQ->val.exp) {
            PolyAddHelper(&nodeNewP->val.p, &nodeQ->val.p);

            if (nodeNewP->val.p.isConst && nodeNewP->val.p.coeff == 0) {
                Node* toDelete = nodeNewP;

                nodeNewP = nodeNewP->right;
                nodeQ = nodeQ->right;

                toDelete->right->left = toDelete->left;
                toDelete->left->right = toDelete->right;
                free(toDelete);
                newP->list->len--;
            } else {
                nodeNewP = nodeNewP->right;
                nodeQ = nodeQ->right;
            }
            continue;
        }
        if (nodeNewP->val.exp > nodeQ->val.exp) {

            newP->list->len++;
            Node* toKeep = nodeQ->right;
            nodeQ->right->left = nodeQ->left;
            nodeQ->left->right = nodeQ->right;
            q->list->len--;

            Node* prev = nodeNewP->left;

            prev->right = nodeQ;
            nodeQ->left = prev;

            nodeQ->right = nodeNewP;
            nodeNewP->left = nodeQ;

            nodeQ = toKeep;

            continue;
        }

        if (nodeNewP->val.exp < nodeQ->val.exp) {
            nodeNewP = nodeNewP->right;
            continue;
        }
    }
    if (newP->list != NULL && newP->list->len == 0) {
        free(newP->list->head);
        free(newP->list->tail);
        free(newP->list);
        newP->list = NULL;
        newP->isConst = true;
        newP->coeff = 0;
    }


    PolyDestroy(q);
    return;


}

void printMonomial(Mono m) {
    putchar('(');
    printPolynomial(m.p);
    putchar(',');
    printf("%d", m.exp);
    putchar(')');
}

void printPolynomial(Poly p) {
    if (p.isConst) {
        printf("%lld", p.coeff);
        return;
    }
    Node* curNode = p.list->head->right;
    while (!curNode->right->isDummy) {
        printMonomial(curNode->val);
        putchar('+');
        curNode = curNode->right;
    }
    printMonomial(curNode->val);

}

void MonoNegDest(Mono* m) {
    PolyNegDest(&m->p);
}

void PolyNegDest(Poly* p) {
    if (PolyIsCoeff(p)) {
        p->coeff = -p->coeff;
        return;
    }

    Node* curNode = p->list->head->right;
    while (!curNode->isDummy) {
        MonoNegDest(&curNode->val);
        curNode = curNode->right;
    }
};


void PolyAddDest(Poly* p, Poly* q) {
    PolyAddHelper(p, q);
    clearFromZeros(p);
}


Poly PolyToExponent(Poly p, poly_exp_t exp) {
    Poly base=PolyClone(&p);
    if (exp == 1) {
        return base;
    }
    Poly result = PolyFromCoeff(1);
    Poly temp;
    while (exp>0) {
        if (exp % 2 == 1) {
            temp = PolyMul(&result, &base);
            PolyDestroy(&result);
            result = temp;
        }
        exp = exp>>1;
        temp = PolyMul(&base, &base);
        PolyDestroy(&base);
        base = temp;
    }
    PolyDestroy(&base);
    return result;
}

static Poly PolyComposeHelper(const Poly* p, unsigned count, const Poly x[]) {
    if (p->isConst) {
        return PolyClone(p);
    }
    if (count == 0) {
        return PolyZero();
    }

    Mono* array = malloc(1000 * (sizeof(Mono)));
    size_t arrSize = 1000;
    Node* iterator = p->list->head->right;
    unsigned int i = 0;
    while (!iterator->isDummy) {
        if (i > arrSize + 2) {
            array = realloc(array, arrSize * 2);
            arrSize *= 2;
        }
        Poly p1 = PolyToExponent(x[0], iterator->val.exp);
        Poly p2 = PolyCompose(&iterator->val.p, count - 1, x + 1);
        Poly p3 = PolyMul(&p1, &p2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        array[i] = MonoFromPoly(&p3, 1);
        iterator = iterator->right;
        i++;
    }

    Poly resSum = PolyAddMonos(i, array);
    if(PolyIsZero(&resSum)){
        free(array);
        return resSum;
    }
    assert(resSum.list->len == 1);
    Poly finRes = resSum.list->head->right->val.p;
    freeList(resSum.list);
    free(array);
    return finRes;

}

 Poly PolyCompose(const Poly* p, unsigned count, const Poly x[]) {
    if(count==0){
        return PolyClone(p);
    } else{
        return PolyComposeHelper(p, count,x);
    }
}
