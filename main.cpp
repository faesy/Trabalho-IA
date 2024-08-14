#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <chrono>
#include <stack>

using namespace std;
using namespace std::chrono;

// Função para verificar se atingimos o objetivo
bool objetivoAtingido(const vector<int>& estado, const vector<int>& objetivo) {
    return estado == objetivo;
}

// Função para gerar novos estados a partir de um estado atual
vector<pair<vector<int>, string>> gerarNovosEstados(const vector<int>& estadoAtual, const vector<int>& maximos) {
    vector<pair<vector<int>, string>> novosEstados;

    // Operação 1: Encher completamente qualquer jarro
    for (int i = 0; i < 8; i++) {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = maximos[i];
        novosEstados.push_back({novoEstado, "Encher jarro " + to_string(i + 1)});
    }

    // Operação 2: Esvaziar completamente qualquer jarro
    for (int i = 0; i < 8; i++) {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = 0;
        novosEstados.push_back({novoEstado, "Esvaziar jarro " + to_string(i + 1)});
    }

    // Operação 3: Transferir água de um jarro para outro
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
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

// Função busca por largura para encontrar a solução
bool BL(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos) {
    set<vector<int>> visitados;
    queue<pair<vector<int>, vector<string>>> fila;

    fila.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!fila.empty()) {
        auto [estadoAtual, caminho] = fila.front();
        fila.pop();

        if (objetivoAtingido(estadoAtual, objetivo)) {
            cout << "Solução encontrada!" << endl;
            cout << "Movimentos realizados:" << endl;
            for (const auto& movimento : caminho) {
                cout << movimento << endl;
            }
            return true;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        for (const auto& [novoEstado, movimento] : novosEstados) {
            if (visitados.find(novoEstado) == visitados.end()) {
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                fila.push({novoEstado, novoCaminho});
                visitados.insert(novoEstado);
            }
        }
    }

    return false;
}

int main() {
    // Volumes máximos dos 8 jarros
    vector<int> maximos = {3, 8, 6, 9, 4, 2, 1, 13};

    // Volumes iniciais dos 8 jarros (todos vazios)
    vector<int> estadoInicial(8, 0);

    // Volumes objetivos dos 8 jarros
    vector<int> objetivo = {1, 5, 2, 8, 3, 2, 0, 9};

    // Medir o tempo de execução
    auto inicio = high_resolution_clock::now();

    if (BL(estadoInicial, objetivo, maximos)) {
        auto fim = high_resolution_clock::now();
        auto duracao = duration_cast<milliseconds>(fim - inicio);
        cout << "Tempo de execucaoo: " << duracao.count() << " ms" << endl;
    } else {
        cout << "Nenhuma solucao encontrada." << endl;
    }

    return 0;
}
