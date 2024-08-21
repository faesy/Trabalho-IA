#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <chrono>
#include <stack>
#include <fstream> // Para manipulação de arquivos
#include <algorithm> // Para sort

using namespace std;
using namespace std::chrono;

// Defina o número de jarros
const int NUM_JARROS = 4;

// Função para verificar se atingimos o objetivo
bool objetivoAtingido(const vector<int>& estado, const vector<int>& objetivo) {
    return estado == objetivo;
}

// Função para gerar novos estados a partir de um estado atual
vector<pair<vector<int>, string>> gerarNovosEstados(const vector<int>& estadoAtual, const vector<int>& maximos) {
    vector<pair<vector<int>, string>> novosEstados;

    // Operação 1: Encher completamente qualquer jarro
    for (int i = 0; i < NUM_JARROS; i++) {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = maximos[i];
        novosEstados.push_back({novoEstado, "Encher jarro " + to_string(i + 1)});
    }

    // Operação 2: Esvaziar completamente qualquer jarro
    for (int i = 0; i < NUM_JARROS; i++) {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = 0;
        novosEstados.push_back({novoEstado, "Esvaziar jarro " + to_string(i + 1)});
    }

    // Operação 3: Transferir água de um jarro para outro
    for (int i = 0; i < NUM_JARROS; i++) {
        for (int j = 0; j < NUM_JARROS; j++) {
            if (i != j && estadoAtual[i] > 0 && estadoAtual[j] < maximos[j]) {
                vector<int> novoEstado = estadoAtual;
                int transferir = min(estadoAtual[i], maximos[j] - estadoAtual[j]);
                novoEstado[i] -= transferir;
                novoEstado[j] += transferir;
                novosEstados.push_back({novoEstado, "Transferir de jarro " + to_string(i + 1) + " para jarro " + to_string(j + 1)});
            }
        }
    }

    return novosEstados;
}

// Função para resolver o problema usando busca em largura (BFS)
void resolverBFS(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    set<vector<int>> visitados;
    queue<pair<vector<int>, vector<string>>> fila;

    fila.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!fila.empty()) {
        auto [estadoAtual, caminho] = fila.front();
        fila.pop();

        if (objetivoAtingido(estadoAtual, objetivo)) {
            outputFile << "Solução encontrada com BFS!" << endl;
            outputFile << "Movimentos realizados:" << endl;
            for (const auto& movimento : caminho) {
                outputFile << movimento << endl;
            }
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        // Ordena os novos estados para garantir que o caminho à esquerda seja seguido
        sort(novosEstados.begin(), novosEstados.end(), [](const auto& a, const auto& b) {
            return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end());
        });

        for (const auto& [novoEstado, movimento] : novosEstados) {
            if (visitados.find(novoEstado) == visitados.end()) {
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                fila.push({novoEstado, novoCaminho});
                visitados.insert(novoEstado);
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com BFS." << endl;
}

// Função de backtracking iterativo
void resolverBacktrackingIterativo(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    stack<pair<vector<int>, vector<string>>> pilha;
    set<vector<int>> visitados;

    pilha.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!pilha.empty()) {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();

        if (objetivoAtingido(estadoAtual, objetivo)) {
            outputFile << "Solução encontrada com Backtracking Iterativo!" << endl;
            outputFile << "Movimentos realizados:" << endl;
            for (const auto& movimento : caminho) {
                outputFile << movimento << endl;
            }
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        // Ordena os novos estados para seguir sempre o caminho à esquerda
        sort(novosEstados.begin(), novosEstados.end(), [](const auto& a, const auto& b) {
            return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end());
        });

        for (const auto& [novoEstado, movimento] : novosEstados) {
            // Verifica se o novo estado já foi visitado
            if (visitados.find(novoEstado) == visitados.end()) {
                visitados.insert(novoEstado);
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com Backtracking Iterativo." << endl;
}

// Função de busca em profundidade (DFS)
void resolverDFS(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    stack<pair<vector<int>, vector<string>>> pilha;
    set<vector<int>> visitados;

    pilha.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!pilha.empty()) {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();

        if (objetivoAtingido(estadoAtual, objetivo)) {
            outputFile << "Solução encontrada com DFS!" << endl;
            outputFile << "Movimentos realizados:" << endl;
            for (const auto& movimento : caminho) {
                outputFile << movimento << endl;
            }
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        // Ordena os novos estados para seguir sempre o caminho à esquerda
        sort(novosEstados.begin(), novosEstados.end(), [](const auto& a, const auto& b) {
            return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end());
        });

        for (const auto& [novoEstado, movimento] : novosEstados) {
            // Verifica se o novo estado já foi visitado
            if (visitados.find(novoEstado) == visitados.end()) {
                visitados.insert(novoEstado);
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com DFS." << endl;
}

void resolverDFSComMelhorSolucao(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    stack<pair<vector<int>, vector<string>>> pilha;
    set<vector<int>> visitados;
    vector<string> melhorCaminho;
    int menorNumeroMovimentos = numeric_limits<int>::max(); // Valor máximo possível para comparações

    pilha.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!pilha.empty()) {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();

        if (objetivoAtingido(estadoAtual, objetivo)) {
            if (caminho.size() < menorNumeroMovimentos) {
                menorNumeroMovimentos = caminho.size();
                melhorCaminho = caminho;
            }
            continue; // Continue procurando outras soluções possíveis
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        // Ordenar os novos estados para garantir que a busca siga sempre o caminho mais à esquerda
        sort(novosEstados.begin(), novosEstados.end(), [](const pair<vector<int>, string>& a, const pair<vector<int>, string>& b) {
            return a.second < b.second;
        });

        for (const auto& [novoEstado, movimento] : novosEstados) {
            if (visitados.find(novoEstado) == visitados.end()) {
                visitados.insert(novoEstado);
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    if (menorNumeroMovimentos < numeric_limits<int>::max()) {
        outputFile << "Melhor solução encontrada!" << endl;
        outputFile << "Número de movimentos: " << menorNumeroMovimentos << endl;
        outputFile << "Movimentos realizados:" << endl;
        for (const auto& movimento : melhorCaminho) {
            outputFile << movimento << endl;
        }
    } else {
        outputFile << "Nenhuma solução encontrada." << endl;
    }
}

int main() {
    vector<int> maximos = {1, 8, 6, 9};  // Exemplo com 6 jarros
    vector<int> estadoInicial(NUM_JARROS, 0);
    vector<int> objetivo = {1, 5, 2, 8};  // Exemplo com 6 jarros

    cout << "Escolha o algoritmo para resolver o problema:\n";
    cout << "1. Busca em Largura (BFS)\n";
    cout << "2. Backtracking Iterativo\n";
    cout << "3. Busca em Profundidade com Melhor Solução\n";
    cout << "4. Busca em Profundidade\n";
    int escolha;
    cin >> escolha;

    // Abre o arquivo para escrita
    ofstream outputFile("resultado.txt");
    if (!outputFile) {
        cerr << "Erro ao abrir o arquivo para escrita." << endl;
        return 1;
    }

    auto inicio = high_resolution_clock::now();

    if (escolha == 1) {
        resolverBFS(estadoInicial, objetivo, maximos, outputFile);
    } else if (escolha == 2) {
        resolverBacktrackingIterativo(estadoInicial, objetivo, maximos, outputFile);
    } else if (escolha == 3) {
        resolverDFSComMelhorSolucao(estadoInicial, objetivo, maximos, outputFile);
    } else if (escolha == 4) {
        resolverDFS(estadoInicial, objetivo, maximos, outputFile);
    } else {
        outputFile << "Escolha inválida." << endl;
    }

    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<milliseconds>(fim - inicio);
    outputFile << "Tempo de execução: " << duracao.count() << " ms" << endl;

    // Fecha o arquivo
    outputFile.close();

    return 0;
}