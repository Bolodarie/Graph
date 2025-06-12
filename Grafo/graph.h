#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set> 
#include <queue> // NOVO: Para std::priority_queue
#include <limits> // Para numeric_limits (infinito)

using namespace std;

// --- Estruturas para BFS (mantidas) ---
struct QueueNodeGraph; 

typedef struct {
    QueueNodeGraph** array; 
    int front, rear, capacity;
} QueueGraph;

struct QueueNodeGraph {
    int node_idx; 
    struct QueueNodeGraph* parent; 
    int dist; 
};
// --- Fim Estruturas para BFS ---

class node {
public: 
    string substantivo;

    int size() const; 
    char operator[](int k) const; 
    bool operator==(string S) const; 
};

class arc {
public: 
    
    string verbo;
    int from;
    int to;
};

// Declarações das funções da fila (mantidas)
QueueGraph* createQueueGraph(int capacity);
void enqueueGraph(QueueGraph* q, QueueNodeGraph* node);
QueueNodeGraph* dequeueGraph(QueueGraph* q);
bool isQueueEmptyGraph(QueueGraph* q);
void freeQueueGraph(QueueGraph* q);


class graph {
public: 
    vector<node> nd;
    vector< vector<arc> > a;
    set<string> hierarchical_verbs; 


    int size() const; 
    bool nodeIsIn(string S);
    void nodeAppend(string S);
    void arcAppend(string S1, string V, string S2);

    void printRelations(string S);
    void printSubs();

    void load(ifstream& F);

    // --- Funções para o Trabalho B ---
    void addHierarchicalVerb(string verb);
    vector<int> bfs(int start_node_idx, int end_node_idx);
    vector<int> bfsHierarchical(int start_node_idx, int end_node_idx);
    
    // NOVO: Declaração do Dijkstra
    vector<int> dijkstra(int start_node_idx, int end_node_idx);
    // --- Fim Funções para o Trabalho B ---
};