#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "agent.h"

agent newAgent(enviroment E){
    agent A;
    A.score = 0;
    A.temFlecha = true;
    A.onde = &E.grid[0][0];
    return A;
}

void sense(agent A){
    if (A.onde->S.cheiro){
        printf("Aqui há um cheiro monstruoso. \n");
    }
    if (A.onde->S.vento){
        printf("Aqui está batendo um vento estranho. \n");
    }
}

bool move(agent* A, enviroment E, place* target){
/*
    Movimenta o agente para a posição place, desde que esta seja uma vizinha de
    cleaner.ondeCleaner e que a bateria de C não esteja vazia.
*/
    if (isNeighbor(*A->onde,*target)){
        A->onde = target;
        A->score--;
        if (A->onde->monstro){
            if (A->temFlecha){
                printf("Matou o monstro. \n");
                A->score -= 10;
                A->onde->monstro = false;
            }
            else{
                printf("Pego pelo monstro.\n");
                A->score -= 1000;
            }
        }
        if (A->onde->buraco){
            printf("Caiu no buraco. \n");
            A->score -= 1000;
        }
        if (A->onde->ouro){
            printf("O ouro está aqui! \n");
            A->comOuro = true;
        }
        if (A->comOuro && A->onde->row==E.h-1 && A->onde->col==E.w-1){
            printf("Escapou com o ouro! Parabéns. \n");
        }
        return true;
    }
    return false;
}

void printSimulation(agent A, enviroment E){
    for (int i=0; i<E.h; i++){
        for (int j=0; j<E.w; j++){
            if (&E.grid[i][j]==A.onde){
                printf("O ");
            }
            else{
                printf("_ ");
            }
        }
        printf("\n");
    }
}