#include "includes.h"

bool isEmptyListP(tListP L){
    return (L == LPNULL);
}

void createEmptyListP(tListP *L) {
    *L = LPNULL;
}

bool createNodeLP(tPosLP *p){
    *p = malloc(sizeof(struct tNodeLP));
    return *p != LPNULL;
}

bool insertItemP(tItemLP d, tPosLP p, tListP *L){
    tPosLP q, r;
    if (!createNodeLP(&q)){
        return false;
    }
    else{
        q->data = d;
        q->next = LPNULL;
        if (isEmptyListP(*L)){
            *L = q;
        }
        else if (p == LPNULL){
            for (r = *L; r->next != LPNULL; r = r->next)
                ; //We move to the end of the list
            r->next = q;
        }
        else if (p == *L){
            q->next = p;
            *L = q;
        }
        else{

            q->data = p->data; // data exchange
            p->data = d;
            q->next = p->next;
            p->next = q;
        }
        return true;
    }
}

void updateItemP(tItemLP d, tPosLP p, tListP *L){
    p->data = d;
}

tPosLP findItemP(tItemLP d, tListP L){
    tPosLP p;
    //for (p = L; (p != LNULL) && (p->data != d); p = p->next);
    return p;
}

tItemLP getItemP(tPosLP p, tListP L) { return p->data; }

tPosLP firstP(tListP L){
    return L;
}

tPosLP lastP(tListP L){
    tPosLP p;
    for (p = L; p->next != LPNULL; p = p->next);
    return p;
}

tPosLP previousP(tPosLP p, tListP L){
    tPosLP q;
    if (p == L){
        return LPNULL;
    }
    else{
        for (q = L; q->next != p; q = q->next);
        return q;
    }
}

tPosLP nextP(tPosLP p, tListP L) { return p->next; }

void deleteAtPositionP(tPosLP p, tListP *L){
    tPosLP q;
    if (p == *L){
        *L = (*L)->next;
    }
    else if (p->next == LPNULL){
        for (q = *L; q->next->next != LPNULL; q = q->next);
        q->next = LPNULL;
    }
    else{
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}

void deleteListP(tListP *L){
    tPosLP p;
    while (!isEmptyListP(*L))
    {
        p = *L;
        *L = (*L)->next;
        free(p);
    }
}
