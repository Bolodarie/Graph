#include <stdio.h>
#include <stdbool.h>
#include "agent.h"
#include "env.h"

int main(){

    enviroment E = newEnviroment(5,5);
    int numBuraco = 3; int numMonstro = 1;
    initEnviroment(E,numBuraco,numMonstro);
    agent A = newAgent(E);

    printf("Você está em uma sala com dimensões %d x %d. \n",E.h,E.w);
    printf("Nesta sala há %d buracos, %d monstros e uma barra de ouro. \n", 
        numBuraco,numMonstro);
    printf("Seu objetivo é chegar na posição (%d,%d) em posse do ouro.",
        E.h-1,E.w-1);
    printf("Fazendo a menor quantidade de movimentos possível. \n");
    printf("Você começa na posição (0,0). Boa sorte! \n");

    char mov;
    while(!(A.comOuro && A.onde->row==E.h-1 && A.onde->col==E.w-1)){
        printf("Score atual: %d \n", A.score);
        printSimulation(A,E);
        sense(A);
        printf("Para onde você deseja se mover? \n");
        scanf(" %c",&mov);
        if (mov=='b'){
            if (A.onde->row<E.h-1){
                move(&A,E,&E.grid[A.onde->row+1][A.onde->col]);
            }
        }
        if (mov=='c'){
            if (A.onde->row>0){
                move(&A,E,&E.grid[A.onde->row-1][A.onde->col]);
            }
        }
        if (mov=='d'){
            if (A.onde->col<E.w-1){
                move(&A,E,&E.grid[A.onde->row][A.onde->col+1]);
            }
        }
        if (mov=='e'){
            if (A.onde->col>0){
                move(&A,E,&E.grid[A.onde->row][A.onde->col-1]);
            }
        }
    }
    printf("Score final: %d \n",A.score);

    return 0;
}