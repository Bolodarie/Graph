#include "env.h"

#ifndef AGENT_H
#define AGENT_H

typedef struct {
    place* onde;
    bool comOuro;
    bool temFlecha;
    int score;
}agent;

void printSimulation(agent A, enviroment E);
bool move(agent* A, enviroment E, place* target);
agent newAgent(enviroment E);
void sense(agent A);

#endif 