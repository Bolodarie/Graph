#include "graph.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <set> 
#include <queue>    
#include <limits>   
#include <new> 
#include <exception> 

using namespace std;
/*------------------------------------------------------------------------------
--------------------------------------------------------------------------------
    Funções de nó
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

int node::size() const { 
    return this->substantivo.size();
}
char node::operator[](int k) const { 
    return this->substantivo[k];
}
bool node::operator==(string S) const { 
    if (this->size()!=S.size()) 
        return false;
    for (size_t i=0; i<this->size(); i++){ 
        if (this->substantivo[i]!=S[i])
            return false;
    }
    return true;
}

/*------------------------------------------------------------------------------
--------------------------------------------------------------------------------
    Funções de grafo
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

int graph::size() const { 
    return this->nd.size();
}

bool graph::nodeIsIn(string S){
    /*
        Checa se existe algum nó com a string S.
    */
    for (size_t i=0; i<this->nd.size(); i++){ 
        if (this->nd[i]==S)
            return true;
    }
    return false;
}

void graph::nodeAppend(string S){
    /*
        Anexa novo nó, caso ainda não tenha sido adicionado.
    */
    if (!this->nodeIsIn(S)){
        node* n = new node;
        n->substantivo = S;
        this->nd.push_back(*n);
        vector<arc> v_arc;
        this->a.push_back(v_arc);
    }
}

void graph::arcAppend(string S1, string V, string S2){
    /*
        Insere novo arco.
        S1 e S2 são os substantivos envolvidos e V é o verbo.
    */
    arc* new_arc = new arc;
    new_arc->verbo = V;
    int pos_S1 = -1; 
    int pos_S2 = -1;

    // Encontrar os índices dos nó
    for (size_t k=0; k<this->size(); k++){ 
        if (this->nd[k]==S1){
            pos_S1 = k;
        }
        if (this->nd[k]==S2){
            pos_S2 = k;
        }
        if (pos_S1 != -1 && pos_S2 != -1) break;
    }
    
    // Apenas adiciona o arco se ambos os nó existirem
    if (pos_S1 != -1 && pos_S2 != -1) {
        new_arc->from = pos_S1;
        new_arc->to = pos_S2;
        this->a[pos_S1].push_back(*new_arc);
    } else {
        delete new_arc; 
    }
}

void graph::printRelations(string S){
    /*
        Imprime as relações que partem da string S.
    */
    for (size_t k=0; k<this->size(); k++){ 
        if (this->nd[k]==S){
            cout << "Relações para " << this->nd[k].substantivo << ":" << endl;
            for (size_t p=0; p<this->a[k].size(); p++){ 
                cout << this->nd[this->a[k][p].from].substantivo << " ";
                cout << this->a[k][p].verbo << " ";
                cout << this->nd[this->a[k][p].to].substantivo << endl;
            }
            return;
        }
    }
    cout << "String não encontrada!";
}

void graph::printSubs(){
    /*
        Imprime substantivos.
    */
    for (size_t k=0; k<this->size(); k++){ 
        cout << this->nd[k].substantivo << endl;
    }
}

void graph::load(ifstream& F){
    /*
        Carrega uma base de dados para um grafo.
    */
    string S1, V, S2;
    while (F >> S1 >> V >> S2){ 
        this->nodeAppend(S1);
        this->nodeAppend(S2);
        this->arcAppend(S1,V,S2);
    }
}

// Implementação da nova função para adicionar verbos hierárquicos
void graph::addHierarchicalVerb(string verb) {
    this->hierarchical_verbs.insert(verb);
}

/*------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

QueueGraph* createQueueGraph(int capacity) {
    QueueGraph* q = nullptr; 
    try {
        q = new QueueGraph; 
        q->capacity = capacity;
        q->front = 0;
        q->rear = -1;
        q->array = new QueueNodeGraph*[q->capacity]; 
    } catch (const std::bad_alloc& e) {
        cerr << "Erro fatal de alocacao: " << e.what() << " (Struct ou Array de Ponteiros da fila)" << endl;
        if (q != nullptr) delete q; 
        return nullptr;
    }
    return q;
}

void enqueueGraph(QueueGraph* q, QueueNodeGraph* node) {
    if (q == nullptr) { cerr << "ERRO: Fila q é nullptr!" << endl; return; } 
    // Condição de fila cheia
    if (q->rear == q->capacity - 1) { 
        // cerr << "ERRO FATAL: Fila cheia. Capacidade: " << q->capacity << ", Tentando adicionar no: " << node->node_idx << endl;
        return; 
    }
    q->array[++q->rear] = node; // Incrementa rear e depois usa
}

QueueNodeGraph* dequeueGraph(QueueGraph* q) {
    if (q == nullptr) { cerr << "ERRO: Fila q é nullptr!" << endl; return nullptr; } 
    if (q->front > q->rear) {
        return NULL; 
    }
    QueueNodeGraph* node = q->array[q->front++];
    return node;
}

bool isQueueEmptyGraph(QueueGraph* q) {
    if (q == nullptr) { cerr << "ERRO: Fila q é nullptr!" << endl; return true; } 
    return q->front > q->rear;
}

void freeQueueGraph(QueueGraph* q) {
    if (q == NULL) return;
    if (q->array == nullptr) { delete q; return; } 
    // REMOVIDO: Desalocação dos QueueNodeGraph individuais aqui,
    // pois eles são gerenciados pelo vetor 'all_q_nodes' nas funções BFS.
    /*
    for (size_t i = q->front; i <= q->rear; ++i) { 
        if (q->array[i] != nullptr) { 
            delete q->array[i];
            q->array[i] = nullptr;
        }
    }
    */
    delete[] q->array; 
    delete q; 
}

/*------------------------------------------------------------------------------
--------------------------------------------------------------------------------
    Implementação da Busca em Largura (BFS)
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

vector<int> graph::bfs(int start_node_idx, int end_node_idx) {
    vector<int> path; 

    if (start_node_idx < 0 || start_node_idx >= (int)this->size() || 
        end_node_idx < 0 || end_node_idx >= (int)this->size()) { 
        cerr << "Erro: Indice de no inicial ou final invalido na BFS." << endl; // Reativado para debug
        return path; 
    }

    if (start_node_idx == end_node_idx) {
        path.push_back(start_node_idx);
        return path; 
    }

    bool* visited = nullptr; 
    try {
        visited = new bool[this->size()]; 
    } catch (const std::bad_alloc& e) {
        cerr << "Erro fatal de alocacao para 'visited' array: " << e.what() << endl; // Reativado para debug
        return path;
    }

    for (size_t i = 0; i < this->size(); ++i) { 
        visited[i] = false;
    }

    QueueGraph* q = createQueueGraph(this->size()); 
    if (q == nullptr) { 
       cerr << "Erro na criacao da fila (createQueueGraph retornou nullptr)!" << endl; // Reativado para debug
       delete[] visited;
       return path;
    }

    // Usar um vetor de ponteiros para QueueNodeGraph para gerenciar a memória dos nós da fila
    // Isso é necessário porque o 'parent' aponta para um nó na fila que pode ser desalocado
    // se não for gerenciado cuidadosamente.
    vector<QueueNodeGraph*> all_q_nodes; 

    QueueNodeGraph* start_q_node = nullptr; 
    try {
        start_q_node = new QueueNodeGraph; 
    } catch (const std::bad_alloc& e) {
        cerr << "Erro fatal de alocacao para 'start_q_node': " << e.what() << endl; // Reativado para debug
        delete[] visited;
        freeQueueGraph(q); 
        return path;
    }
    start_q_node->node_idx = start_node_idx;
    start_q_node->parent = nullptr;
    start_q_node->dist = 0;

    enqueueGraph(q, start_q_node); 
    all_q_nodes.push_back(start_q_node); // Adicionar à lista de todos os nós criados
    visited[start_node_idx] = true;

    QueueNodeGraph* found_node = nullptr;

    try { 
        while (!isQueueEmptyGraph(q)) {
            QueueNodeGraph* current_q_node = dequeueGraph(q);

            if (current_q_node == nullptr) { 
                cerr << "ERRO: current_q_node eh nullptr apos dequeue!" << endl; // Reativado para debug
                break; 
            }

            if (current_q_node->node_idx == end_node_idx) {
                found_node = current_q_node;
                break; 
            }

            if (current_q_node->node_idx < 0 || current_q_node->node_idx >= (int)this->a.size()) {
                cerr << "ERRO: current_q_node->node_idx (" << current_q_node->node_idx << ") fora dos limites de 'a' (tamanho: " << this->a.size() << ")!" << endl; // Reativado para debug
                break; 
            }

            for (const auto& arc : this->a[current_q_node->node_idx]) {
                int neighbor_idx = arc.to;

                if (neighbor_idx < 0 || neighbor_idx >= (int)this->size()) {
                    cerr << "ERRO: neighbor_idx (" << neighbor_idx << ") fora dos limites para 'visited' (tamanho: " << this->size() << ")!" << endl; // Reativado para debug
                    continue; 
                }

                if (!visited[neighbor_idx]) {
                    visited[neighbor_idx] = true;
                    QueueNodeGraph* new_q_node = nullptr; 
                    try {
                        new_q_node = new QueueNodeGraph; 
                    } catch (const std::bad_alloc& e) {
                        cerr << "Erro fatal de alocacao para 'new_q_node' (vizinho " << nd[neighbor_idx].substantivo << "): " << e.what() << endl; // Reativado para debug
                        // Limpeza robusta em caso de erro de alocação
                        delete[] visited;
                        // Desalocar todos os QueueNodeGraph criados até agora
                        for (QueueNodeGraph* node_ptr : all_q_nodes) {
                            delete node_ptr;
                        }
                        freeQueueGraph(q);
                        return path;
                    }
                    new_q_node->node_idx = neighbor_idx;
                    new_q_node->parent = current_q_node; 
                    new_q_node->dist = current_q_node->dist + 1;
                    enqueueGraph(q, new_q_node);
                    all_q_nodes.push_back(new_q_node); // Adicionar à lista de todos os nós criados
                }
            }
        }
    } catch (const std::bad_alloc& e) {
        cerr << "ERRO FATAL: std::bad_alloc capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (const std::exception& e) {
        cerr << "ERRO FATAL: Excecao std::exception capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (...) {
        cerr << "ERRO FATAL: Excecao desconhecida capturada no loop principal." << endl; // Reativado para debug
    }
    
    // Reconstruir o caminho
    if (found_node != nullptr) {
        QueueNodeGraph* temp = found_node;
        while (temp != nullptr) {
            path.push_back(temp->node_idx);
            temp = temp->parent; 
        }
        reverse(path.begin(), path.end()); 
    } else {
        // Nenhum caminho encontrado
    }
    
    // Desalocar todos os QueueNodeGraph criados.
    for (QueueNodeGraph* node_ptr : all_q_nodes) {
        delete node_ptr;
    }

    delete[] visited;
    freeQueueGraph(q); 

    return path;
}

/*------------------------------------------------------------------------------
--------------------------------------------------------------------------------
    Implementação da Busca em Largura Hierárquica (BFS Hierárquica)
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

vector<int> graph::bfsHierarchical(int start_node_idx, int end_node_idx) {
    vector<int> path;

    if (start_node_idx < 0 || start_node_idx >= (int)this->size() || 
        end_node_idx < 0 || end_node_idx >= (int)this->size()) { 
        cerr << "Erro: Indice de no inicial ou final invalido na BFS Hierarquica." << endl; // Reativado para debug
        return path;
    }

    if (start_node_idx == end_node_idx) {
        path.push_back(start_node_idx);
        return path;
    }

    // visited array para controle de nós já visitados
    bool* visited = nullptr; 
    try {
        visited = new bool[this->size()]; 
    } catch (const std::bad_alloc& e) {
        cerr << "Erro fatal de alocacao para 'visited' array: " << e.what() << endl; // Reativado para debug
        return path;
    }
    for (size_t i = 0; i < this->size(); ++i) { 
        visited[i] = false;
    }

    // Fila para a BFS
    QueueGraph* q = createQueueGraph(this->size());
    if (q == nullptr) { 
       cerr << "Erro na criacao da fila (createQueueGraph retornou nullptr)!" << endl; // Reativado para debug
       delete[] visited;
       return path;
    }

    // Vetor para gerenciar a memória de todos os QueueNodeGraph criados
    vector<QueueNodeGraph*> all_q_nodes; 

    // Nó inicial
    QueueNodeGraph* start_q_node = nullptr; 
    try {
        start_q_node = new QueueNodeGraph; 
    } catch (const std::bad_alloc& e) {
        cerr << "Erro fatal de alocacao para 'start_q_node': " << e.what() << endl; // Reativado para debug
        delete[] visited;
        freeQueueGraph(q);
        return path;
    }
    start_q_node->node_idx = start_node_idx;
    start_q_node->parent = nullptr;
    start_q_node->dist = 0;

    enqueueGraph(q, start_q_node);
    all_q_nodes.push_back(start_q_node); // Adicionar à lista para gerenciamento de memória
    visited[start_node_idx] = true;

    QueueNodeGraph* found_node = nullptr;

    try {
        while (!isQueueEmptyGraph(q)) {
            QueueNodeGraph* current_q_node = dequeueGraph(q);

            if (current_q_node == nullptr) { 
                cerr << "ERRO: current_q_node eh nullptr apos dequeue!" << endl; // Reativado para debug
                break; 
            }

            // Se o nó atual é o destino, o caminho foi encontrado
            if (current_q_node->node_idx == end_node_idx) {
                found_node = current_q_node;
                break;
            }

            if (current_q_node->node_idx < 0 || current_q_node->node_idx >= (int)this->a.size()) {
                cerr << "ERRO: current_q_node->node_idx (" << current_q_node->node_idx << ") fora dos limites de 'a' (tamanho: " << this->a.size() << ")!" << endl; // Reativado para debug
                break; 
            }

            // Iterar sobre as arestas a partir do nó atual
            for (const auto& arc : this->a[current_q_node->node_idx]) {
                int neighbor_idx = arc.to;
                string verb = arc.verbo;

                if (neighbor_idx < 0 || neighbor_idx >= (int)this->size()) {
                    cerr << "ERRO: neighbor_idx (" << neighbor_idx << ") fora dos limites para 'visited': " << this->size() << ")!" << endl; // Reativado para debug
                    continue; 
                }

                // Lógica da BFS normal: visita se não foi visitado
                if (!visited[neighbor_idx]) {
                    visited[neighbor_idx] = true;
                    QueueNodeGraph* new_q_node = nullptr; 
                    try {
                        new_q_node = new QueueNodeGraph; 
                    } catch (const std::bad_alloc& e) {
                        cerr << "Erro fatal de alocacao para 'new_q_node' (vizinho " << nd[neighbor_idx].substantivo << "): " << e.what() << endl; // Reativado para debug
                        delete[] visited;
                        for (QueueNodeGraph* node_ptr : all_q_nodes) {
                            delete node_ptr;
                        }
                        freeQueueGraph(q);
                        return path;
                    }
                    new_q_node->node_idx = neighbor_idx;
                    new_q_node->parent = current_q_node; 
                    new_q_node->dist = current_q_node->dist + 1;
                    enqueueGraph(q, new_q_node);
                    all_q_nodes.push_back(new_q_node); // Gerenciar memória
                }

                // Lógica de inferência hierárquica:
                if (hierarchical_verbs.count(verb)) {
                    // Crie um nó "virtual" para o vizinho hierárquico
                    for (const auto& sub_arc : this->a[neighbor_idx]) {
                        int sub_neighbor_idx = sub_arc.to;
                        string sub_verb = sub_arc.verbo;

                        if (hierarchical_verbs.count(sub_verb) && !visited[sub_neighbor_idx]) {
                            visited[sub_neighbor_idx] = true;
                            QueueNodeGraph* new_q_node_hierarchical = nullptr;
                            try {
                                new_q_node_hierarchical = new QueueNodeGraph;
                            } catch (const std::bad_alloc& e) {
                                cerr << "Erro fatal de alocacao para 'new_q_node_hierarchical' (vizinho " << nd[sub_neighbor_idx].substantivo << "): " << e.what() << endl; // Reativado para debug
                                delete[] visited;
                                for (QueueNodeGraph* node_ptr : all_q_nodes) {
                                    delete node_ptr;
                                }
                                freeQueueGraph(q);
                                return path;
                            }
                            new_q_node_hierarchical->node_idx = sub_neighbor_idx;
                            // O pai é o nó que levou à inferência hierárquica original (current_q_node)
                            // A distância é a distância do current_q_node + 2 (um salto para neighbor_idx, um salto para sub_neighbor_idx)
                            new_q_node_hierarchical->parent = current_q_node;
                            new_q_node_hierarchical->dist = current_q_node->dist + 2; // Considera dois "saltos" lógicos
                            enqueueGraph(q, new_q_node_hierarchical);
                            all_q_nodes.push_back(new_q_node_hierarchical); // Gerenciar memória
                        }
                    }
                }
            }
        }
    } catch (const std::bad_alloc& e) {
        cerr << "ERRO FATAL: std::bad_alloc capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (const std::exception& e) {
        cerr << "ERRO FATAL: Excecao std::exception capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (...) {
        cerr << "ERRO FATAL: Excecao desconhecida capturada no loop principal." << endl; // Reativado para debug
    }
    
    // Reconstruir o caminho
    if (found_node != nullptr) {
        QueueNodeGraph* temp = found_node;
        while (temp != nullptr) {
            path.push_back(temp->node_idx);
            temp = temp->parent; 
        }
        reverse(path.begin(), path.end()); 
    } else {
        // Nenhum caminho encontrado
    }
    
    // Desalocar todos os QueueNodeGraph criados.
    for (QueueNodeGraph* node_ptr : all_q_nodes) {
        delete node_ptr;
    }

    delete[] visited;
    freeQueueGraph(q);

    return path;
}

/*------------------------------------------------------------------------------
--------------------------------------------------------------------------------
    Implementação do Algoritmo de Dijkstra
--------------------------------------------------------------------------------
------------------------------------------------------------------------------*/

typedef pair<int, int> PairInt; 

vector<int> graph::dijkstra(int start_node_idx, int end_node_idx) {
    vector<int> path;

    if (start_node_idx < 0 || start_node_idx >= (int)this->size() ||
        end_node_idx < 0 || end_node_idx >= (int)this->size()) {
        cerr << "Erro: Indice de no inicial ou final invalido no Dijkstra." << endl; // Reativado para debug
        return path;
    }

    if (start_node_idx == end_node_idx) {
        path.push_back(start_node_idx);
        return path;
    }

    // Vetores para armazenar distâncias e predecessores
    vector<int> dist(this->size(), numeric_limits<int>::max()); 
    vector<int> prev(this->size(), -1); 

    // Fila de prioridade
    priority_queue<PairInt, vector<PairInt>, greater<PairInt>> pq;

    dist[start_node_idx] = 0; 
    pq.push({0, start_node_idx}); 

    try {
        while (!pq.empty()) {
            int d = pq.top().first; 
            int u = pq.top().second; 
            pq.pop(); 

            if (d > dist[u]) {
                continue;
            }

            if (u == end_node_idx) {
                break;
            }

            if (u < 0 || u >= (int)this->a.size()) {
                cerr << "ERRO: u fora dos limites de 'a': " << u << endl; // Reativado para debug
                break; 
            }

            for (const auto& arc : this->a[u]) {
                int v = arc.to; 
                int weight = 1; // Para Dijkstra de caminho mais curto não ponderado, peso 1 é OK.

                if (v < 0 || v >= (int)this->size()) {
                    cerr << "ERRO: v fora dos limites para 'dist'/'prev': " << v << endl; // Reativado para debug
                    continue; 
                }

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u; 
                    pq.push({dist[v], v}); 
                }
            }
        }
    } catch (const std::bad_alloc& e) {
        cerr << "ERRO FATAL: std::bad_alloc capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (const std::exception& e) {
        cerr << "ERRO FATAL: Excecao std::exception capturada no loop principal: " << e.what() << endl; // Reativado para debug
    } catch (...) {
        cerr << "ERRO FATAL: Excecao desconhecida capturada no loop principal." << endl; // Reativado para debug
    }

    if (dist[end_node_idx] == numeric_limits<int>::max()) {
        return path; 
    }

    int curr = end_node_idx;
    while (curr != -1) {
        path.push_back(curr);
        curr = prev[curr];
    }
    reverse(path.begin(), path.end()); 

    return path;
}