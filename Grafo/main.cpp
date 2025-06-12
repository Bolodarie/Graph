#include "graph.h"
#include <iostream>
#include <vector> 
#include <string> 
#include <random> 
#include <chrono> 
#include <limits> 
#include <cmath>  
#include <iomanip> 

using namespace std;

// Função auxiliar para encontrar o índice de um nó dado seu substantivo
int getNodeIndex(const graph& g, const string& s) { 
    for (size_t i = 0; i < g.size(); ++i) { 
        if (g.nd[i] == s) { 
            return i;
        }
    }
    return -1; 
}

// Função auxiliar para gerar um grafo com um número específico de arestas
// Alterada para usar os substantivos já carregados para evitar duplicação.
void generateRandomGraph(graph& g, int num_edges, const vector<string>& all_substantives, const set<string>& hierarchical_verbs_list) {
    // Limpa o grafo existente
    g.nd.clear();
    g.a.clear();
    g.hierarchical_verbs.clear(); // Limpa e adiciona novamente para cada geração

    // Adiciona os verbos hierárquicos passados como parâmetro
    for (const string& verb : hierarchical_verbs_list) {
        g.addHierarchicalVerb(verb);
    }

    // Adiciona todos os substantivos da lista ao grafo (garante que temos nós)
    for (const string& s : all_substantives) {
        g.nodeAppend(s);
    }

    // Gerador de números aleatórios para escolher nós e verbos
    random_device rd;
    mt19937 gen(rd());
    // Garante que a distribuição não tente acessar um índice fora dos limites se o grafo tiver 0 nós.
    uniform_int_distribution<> distrib_node(0, g.size() > 0 ? g.size() - 1 : 0); 
    
    // Vetor de verbos para facilitar a escolha aleatória.
    vector<string> verbs_vec;
    // Hierárquicos (já serão adicionados ao set do grafo)
    verbs_vec.push_back("eh"); 
    verbs_vec.push_back("e");  
    // Outros verbos (não hierárquicos)
    verbs_vec.push_back("faz"); 
    verbs_vec.push_back("vive"); 
    verbs_vec.push_back("come");
    verbs_vec.push_back("monta");
    verbs_vec.push_back("caça");
    verbs_vec.push_back("minera");
    verbs_vec.push_back("vale");
    verbs_vec.push_back("roubou");
    verbs_vec.push_back("ama");
    verbs_vec.push_back("dirige");

    // Adiciona arestas aleatoriamente
    int edges_added = 0;
    // Garante que haja pelo menos 2 nós para criar uma aresta válida
    if (g.size() < 2 || verbs_vec.empty()) return; // Se não houver nós ou verbos, não pode adicionar arestas

    while (edges_added < num_edges) { 
        int from_idx = distrib_node(gen);
        int to_idx = distrib_node(gen);
        
        // Evita arestas para o mesmo nó e garante que os índices são válidos
        if (from_idx == to_idx || from_idx >= g.size() || to_idx >= g.size()) continue;

        string random_verb = verbs_vec[uniform_int_distribution<>(0, verbs_vec.size() - 1)(gen)];

        g.arcAppend(g.nd[from_idx].substantivo, random_verb, g.nd[to_idx].substantivo);
        edges_added++; 
    }
}


int main(){
    // Setup para medição de tempo
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;

    // --- Parâmetros de Avaliação do Trabalho B ---
    vector<int> num_edges_to_test = {10, 20, 30, 40, 50}; // Grafos com 10, 20, 30, 40 e 50 arestas
    const int num_queries_per_config = 1000;             // VOLTANDO PARA 1000 CONSULTAS

    // Definir os verbos hierárquicos. Estes serão passados para a função generateRandomGraph
    // e também definidos no grafo principal para as buscas.
    set<string> hierarchical_verbs_list;
    hierarchical_verbs_list.insert("eh"); 
    hierarchical_verbs_list.insert("e");  
    
    // Obter todos os substantivos possíveis do data.txt original para consultas aleatórias
    ifstream initial_F;
    initial_F.open("data.txt");
    if (!initial_F.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo data.txt original para carregar substantivos." << endl;
        return 1;
    }
    graph temp_g_for_subs;
    temp_g_for_subs.load(initial_F);
    initial_F.close();

    vector<string> all_substantives;
    for (size_t i = 0; i < temp_g_for_subs.size(); ++i) {
        all_substantives.push_back(temp_g_for_subs.nd[i].substantivo);
    }
    
    // Configurar gerador de números aleatórios para queries UMA ÚNICA VEZ
    random_device rd_queries;
    mt19937 gen_queries(rd_queries()); 
    
    cout << "Iniciando Avaliacao de Performance do Trabalho B." << endl;
    cout << "------------------------------------------------" << endl;

    // Loop para cada tamanho de grafo (número de arestas)
    for (int num_edges : num_edges_to_test) {
        cout << "\n--- Testando Grafo com " << num_edges << " Arestas ---" << endl;

        graph G;
        generateRandomGraph(G, num_edges, all_substantives, hierarchical_verbs_list);

        // Comentar linhas de DEBUG se a saída estiver muito grande
        // cout << "  DEBUG: Grafo gerado com " << G.size() << " nos." << endl;
        // if (G.size() > 0) {
        //     cout << "  DEBUG: Primeiro no: " << G.nd[0].substantivo << endl;
        //     if (G.size() > 1) { 
        //         cout << "  DEBUG: Segundo no: " << G.nd[1].substantivo << endl;
        //     }
        // }
        
        if (G.size() < 2) {
            cout << "  Grafo muito pequeno para testes de busca (" << G.size() << " nos). Pulando." << endl;
            continue;
        }

        // A distribuição de nó para as queries deve ser baseada no tamanho do grafo atual
        uniform_int_distribution<> distrib_query_node(0, G.size() - 1); 

        // --- Medição de Performance para BFS ---
        cout << "  --- Performance BFS ---" << endl;
        long long total_bfs_duration_ns = 0; 
        vector<long long> bfs_durations_ns; 

        for (int i = 0; i < num_queries_per_config; ++i) {
            int start_idx = distrib_query_node(gen_queries);
            int end_idx = distrib_query_node(gen_queries);

            // Comentar linhas de DEBUG se a saída estiver muito grande
            // cout << "    DEBUG Query BFS " << i+1 << ": Buscando de " 
            //      << G.nd[start_idx].substantivo << " (" << start_idx << ")"
            //      << " para " << G.nd[end_idx].substantivo << " (" << end_idx << ")" << endl;
            
            auto start = high_resolution_clock::now();
            G.bfs(start_idx, end_idx); 
            auto end = high_resolution_clock::now();

            long long duration_ns = duration_cast<nanoseconds>(end - start).count();
            total_bfs_duration_ns += duration_ns;
            bfs_durations_ns.push_back(duration_ns);
        }

        double avg_bfs_ns = (double)total_bfs_duration_ns / num_queries_per_config;
        double std_dev_bfs_ns = 0.0;
        if (num_queries_per_config > 1) {
            double sum_sq_diff_bfs = 0.0;
            for (long long dur : bfs_durations_ns) {
                sum_sq_diff_bfs += (dur - avg_bfs_ns) * (dur - avg_bfs_ns);
            }
            std_dev_bfs_ns = std::sqrt(sum_sq_diff_bfs / (num_queries_per_config - 1));
        }
        cout << "    Tempo Medio (BFS): " << fixed << setprecision(2) << avg_bfs_ns << " ns" << endl;
        cout << "    Desvio Padrao (BFS): " << fixed << setprecision(2) << std_dev_bfs_ns << " ns" << endl;

        // --- Medição de Performance para BFS Hierárquica ---
        // DESCOMENTADO: Ativando a medição para BFS Hierárquica
        cout << "\n  --- Performance BFS Hierarquica ---" << endl;
        long long total_bfs_hierarchical_duration_ns = 0;
        vector<long long> bfs_hierarchical_durations_ns;

        for (int i = 0; i < num_queries_per_config; ++i) {
            int start_idx = distrib_query_node(gen_queries);
            int end_idx = distrib_query_node(gen_queries);

            auto start = high_resolution_clock::now();
            G.bfsHierarchical(start_idx, end_idx); 
            auto end = high_resolution_clock::now();

            long long duration_ns = duration_cast<nanoseconds>(end - start).count();
            total_bfs_hierarchical_duration_ns += duration_ns;
            bfs_hierarchical_durations_ns.push_back(duration_ns);
        }

        double avg_bfs_hierarchical_ns = (double)total_bfs_hierarchical_duration_ns / num_queries_per_config;
        double std_dev_bfs_hierarchical_ns = 0.0;
        if (num_queries_per_config > 1) {
            double sum_sq_diff_bfs_hierarchical = 0.0;
            for (long long dur : bfs_hierarchical_durations_ns) {
                sum_sq_diff_bfs_hierarchical += (dur - avg_bfs_hierarchical_ns) * (dur - avg_bfs_hierarchical_ns);
            }
            std_dev_bfs_hierarchical_ns = std::sqrt(sum_sq_diff_bfs_hierarchical / (num_queries_per_config - 1));
        }
        cout << "    Tempo Medio (BFS Hierarquica): " << fixed << setprecision(2) << avg_bfs_hierarchical_ns << " ns" << endl;
        cout << "    Desvio Padrao (BFS Hierarquica): " << fixed << setprecision(2) << std_dev_bfs_hierarchical_ns << " ns" << endl;
        
        // --- Medição de Performance para Dijkstra ---
        // DESCOMENTADO: Ativando a medição para Dijkstra
        cout << "\n  --- Performance Dijkstra ---" << endl;
        long long total_dijkstra_duration_ns = 0;
        vector<long long> dijkstra_durations_ns;

        for (int i = 0; i < num_queries_per_config; ++i) {
            int start_idx = distrib_query_node(gen_queries);
            int end_idx = distrib_query_node(gen_queries);

            auto start = high_resolution_clock::now();
            G.dijkstra(start_idx, end_idx); 
            auto end = high_resolution_clock::now();

            long long duration_ns = duration_cast<nanoseconds>(end - start).count();
            total_dijkstra_duration_ns += duration_ns;
            dijkstra_durations_ns.push_back(duration_ns);
        }

        double avg_dijkstra_ns = (double)total_dijkstra_duration_ns / num_queries_per_config;
        double std_dev_dijkstra_ns = 0.0;
        if (num_queries_per_config > 1) {
            double sum_sq_diff_dijkstra = 0.0;
            for (long long dur : dijkstra_durations_ns) {
                sum_sq_diff_dijkstra += (dur - avg_dijkstra_ns) * (dur - avg_dijkstra_ns);
            }
            std_dev_dijkstra_ns = std::sqrt(sum_sq_diff_dijkstra / (num_queries_per_config - 1));
        }
        cout << "    Tempo Medio (Dijkstra): " << fixed << setprecision(2) << avg_dijkstra_ns << " ns" << endl;
        cout << "    Desvio Padrao (Dijkstra): " << fixed << setprecision(2) << std_dev_dijkstra_ns << " ns" << endl;
    }
    cout << "\n------------------------------------------------" << endl;
    cout << "Avaliacao de Performance Concluida." << endl;

    return 0;
}