#include "env.h"
#include <time.h>
#include <stdio.h>
#include <unistd.h>
//#include <windows.h> função sleep no windows

void setSensation(place* p, place* v){

    if (isNeighbor(*v,*p)){
        if (v->buraco)
            p->S.vento = true;
        if (v->monstro)
            p->S.cheiro = true;
    }
}

/*------------------------------------------------------------------------------
    Funções básicas do ambiente
------------------------------------------------------------------------------*/

enviroment newEnviroment(int h, int w){
    enviroment E;
    E.h = h; E.w = w;
    E.grid = malloc(E.h*sizeof(place*));
    if (E.grid!=NULL){
        for (int i=0; i<h; i++){
            E.grid[i] = malloc(E.w*sizeof(place));
            if (E.grid[i]!=NULL)
                for (int j=0; j<w; j++){
                    E.grid[i][j].buraco = false;
                    E.grid[i][j].monstro = false;
                    E.grid[i][j].ouro = false;
                    E.grid[i][j].S.cheiro = false;
                    E.grid[i][j].S.vento = false;
                    E.grid[i][j].row = i;
                    E.grid[i][j].col = j;
                }
        }
    }
    return E;
}

void delEnviroment(enviroment* E){
    if (E!=NULL){
        for (int i=0; i<E->h; i++){
            free(E->grid[i]);
        }
        free(E->grid);
    }
}

void initEnviroment(enviroment E, int numBuraco, int numMonstro){
/*
    Inicializa sujeiras no ambiente.
*/
    srand(time(NULL));
    //Inicializa buracos
    while (numBuraco>0){
        int i, j;
        i = rand()%E.h;
        j = rand()%E.w;
        if (i!=0 && i!=E.h-1 && j!=0 && j!=E.w-1){
            if (!E.grid[i][j].buraco){
                E.grid[i][j].buraco = true;
                numBuraco--;
            }
        }
    }
    //Inicializa monstros
    while (numMonstro>0){
        int i, j;
        i = rand()%E.h;
        j = rand()%E.w;
        if (i!=0 && i!=E.h-1 && j!=0 && j!=E.w-1){
            if (!E.grid[i][j].buraco && !E.grid[i][j].monstro){
                E.grid[i][j].monstro = true;
                numMonstro--;
            }
        }
    }
    //Inicializa ouro
    bool gold=false;
    while (!gold){
        int i, j;
        i = rand()%E.h;
        j = rand()%E.w;
        if (i!=0 && i!=E.h-1 && j!=0 && j!=E.w-1){
            if (!E.grid[i][j].buraco && !E.grid[i][j].monstro ){
                E.grid[i][j].ouro = true;
                gold = true;
            }
        }
    }
    initSensations(E);
}

void initSensations(enviroment E){
    //Itera sobre posições no grid
    for (int i=0; i<E.h; i++)
        for (int j=0; j<E.w; j++){
            //Itera sobre vizinhos
            for (int k=0; k<E.h; k++)
                for (int l=0; l<E.w; l++){
                    setSensation(&E.grid[i][j],&E.grid[k][l]);
                }
        }
}

bool isNeighbor(place p, place q){
/*
    Considerando vizinhança-4 (em cruz). No exemplo abaixo, as posições marcadas
    como v são vizinhas de o, enquanto aquelas marcadas como u não são.
    u v u
    v o v
    u v u
*/
    if (p.row==q.row || p.col==q.col){
        if (p.row==q.row+1 || p.row==q.row-1)
            return true;
        if (p.col==q.col+1 || p.col==q.col-1)
            return true;
    }
    return false;
}

place* getPlace(enviroment E, int i, int j){
/*
    Retorna ponteiro para place com indices i e j no grid.
*/
    return &E.grid[i][j];
}

