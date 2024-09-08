#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <chrono>
#include <stack>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;
using namespace std::chrono;

// Defina o número de jarros
const int NUM_JARROS = 4;

// Função para verificar se atingimos o objetivo
bool objetivoAtingido(const vector<int> &estado, const vector<int> &objetivo)
{
    return estado == objetivo;
}

// Função para gerar novos estados a partir de um estado atual
vector<pair<vector<int>, string>> gerarNovosEstados(const vector<int> &estadoAtual, const vector<int> &maximos)
{
    vector<pair<vector<int>, string>> novosEstados;

    // Operação 1: Encher completamente qualquer jarro
    for (int i = 0; i < NUM_JARROS; i++)
    {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = maximos[i];
        novosEstados.push_back({novoEstado, "Encher jarro " + to_string(i + 1)});
    }

    // Operação 2: Esvaziar completamente qualquer jarro
    for (int i = 0; i < NUM_JARROS; i++)
    {
        vector<int> novoEstado = estadoAtual;
        novoEstado[i] = 0;
        novosEstados.push_back({novoEstado, "Esvaziar jarro " + to_string(i + 1)});
    }

    // Operação 3: Transferir água de um jarro para outro
    for (int i = 0; i < NUM_JARROS; i++)
    {
        for (int j = 0; j < NUM_JARROS; j++)
        {
            if (i != j && estadoAtual[i] > 0 && estadoAtual[j] < maximos[j])
            {
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

struct No {
    vector<int> estado;
    vector<string> caminho;
    int custoG;
    int custoF;

    bool operator>(const No& outro) const {
        return custoF > outro.custoF;
    }
};


// Função de heurística (por exemplo, soma das diferenças entre os volumes atuais e os objetivos)
int heuristica(const vector<int>& estado, const vector<int>& objetivo) {
    int somaDiferencas = 0;
    for (int i = 0; i < NUM_JARROS; i++) {
        somaDiferencas += abs(estado[i] - objetivo[i]);
    }
    return somaDiferencas;
}


// Função para resolver o problema usando A*
void resolverAEstrela(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    set<vector<int>> visitados;
    int maiorJarro = *max_element(maximos.begin(), maximos.end());
    priority_queue<No, vector<No>, greater<No>> pq;
    int nosExpandidos = 0;

    pq.push({estadoInicial, {}, 0, heuristica(estadoInicial, objetivo)});
    visitados.insert(estadoInicial);

    while (!pq.empty()) {
        No noAtual = pq.top();
        pq.pop();
        nosExpandidos++;

        if (objetivoAtingido(noAtual.estado, objetivo)) {
            outputFile << "Solução encontrada com A*!" << endl;
            outputFile << "Número de movimentos: " << noAtual.caminho.size() << endl;
            outputFile << "Nós visitados: " << visitados.size() << endl;
            outputFile << "Nós expandidos: " << nosExpandidos << endl;
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(noAtual.estado, maximos);
        for (const auto& [novoEstado, movimento] : novosEstados) {
            if (visitados.find(novoEstado) == visitados.end()) {
                visitados.insert(novoEstado);
                vector<string> novoCaminho = noAtual.caminho;
                novoCaminho.push_back(movimento);
                int novoCustoG = noAtual.custoG + maiorJarro;
                int novoCustoF = novoCustoG + heuristica(novoEstado, objetivo);
                pq.push({novoEstado, novoCaminho, novoCustoG, novoCustoF});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com A*." << endl;
    outputFile << "Nós visitados: " << visitados.size() << endl;
    outputFile << "Nós expandidos: " << nosExpandidos << endl;
}

// Função para resolver o problema usando busca em largura (BFS)
void resolverBFS(const vector<int> &estadoInicial, const vector<int> &objetivo, const vector<int> &maximos, ofstream &outputFile)
{
    set<vector<int>> visitados;
    queue<pair<vector<int>, vector<string>>> fila;
    int nosExpandidos = 0;

    fila.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!fila.empty())
    {
        auto [estadoAtual, caminho] = fila.front();
        fila.pop();
        nosExpandidos++;

        if (objetivoAtingido(estadoAtual, objetivo))
        {
            outputFile << "Solução encontrada com BFS!" << endl;
            outputFile << "Número de movimentos: " << caminho.size() << endl;
            outputFile << "Nós visitados: " << visitados.size() << endl;
            outputFile << "Nós expandidos: " << nosExpandidos << endl;
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        sort(novosEstados.begin(), novosEstados.end(), [](const auto &a, const auto &b)
             { return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end()); });

        for (const auto &[novoEstado, movimento] : novosEstados)
        {
            if (visitados.find(novoEstado) == visitados.end())
            {
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                fila.push({novoEstado, novoCaminho});
                visitados.insert(novoEstado);
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com BFS." << endl;
    outputFile << "Nós visitados: " << visitados.size() << endl;
    outputFile << "Nós expandidos: " << nosExpandidos << endl;
}

// Função de backtracking iterativo
void resolverBacktrackingIterativo(const vector<int> &estadoInicial, const vector<int> &objetivo, const vector<int> &maximos, ofstream &outputFile)
{
    stack<pair<vector<int>, vector<string>>> pilha;
    set<vector<int>> visitados;
    int nosExpandidos = 0;

    pilha.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!pilha.empty())
    {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();
        nosExpandidos++;

        if (objetivoAtingido(estadoAtual, objetivo))
        {
            outputFile << "Solução encontrada com Backtracking Iterativo!" << endl;
            outputFile << "Número de movimentos: " << caminho.size() << endl;
            outputFile << "Nós visitados: " << visitados.size() << endl;
            outputFile << "Nós expandidos: " << nosExpandidos << endl;
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        sort(novosEstados.begin(), novosEstados.end(), [](const auto &a, const auto &b)
             { return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end()); });

        for (const auto &[novoEstado, movimento] : novosEstados)
        {
            if (visitados.find(novoEstado) == visitados.end())
            {
                visitados.insert(novoEstado);
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com Backtracking Iterativo." << endl;
    outputFile << "Nós visitados: " << visitados.size() << endl;
    outputFile << "Nós expandidos: " << nosExpandidos << endl;
}

// Função de busca em profundidade (DFS)
void resolverDFS(const vector<int> &estadoInicial, const vector<int> &objetivo, const vector<int> &maximos, ofstream &outputFile)
{
    stack<pair<vector<int>, vector<string>>> pilha;
    set<vector<int>> visitados;
    int nosExpandidos = 0;

    pilha.push({estadoInicial, {}});
    visitados.insert(estadoInicial);

    while (!pilha.empty())
    {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();
        nosExpandidos++;

        if (objetivoAtingido(estadoAtual, objetivo))
        {
            outputFile << "Solução encontrada com DFS!" << endl;
            outputFile << "Número de movimentos: " << caminho.size() << endl;
            outputFile << "Nós visitados: " << visitados.size() << endl;
            outputFile << "Nós expandidos: " << nosExpandidos << endl;
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        sort(novosEstados.begin(), novosEstados.end(), [](const auto &a, const auto &b)
             { return lexicographical_compare(a.first.begin(), a.first.end(), b.first.begin(), b.first.end()); });

        for (const auto &[novoEstado, movimento] : novosEstados)
        {
            if (visitados.find(novoEstado) == visitados.end())
            {
                visitados.insert(novoEstado);
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com DFS." << endl;
    outputFile << "Nós visitados: " << visitados.size() << endl;
    outputFile << "Nós expandidos: " << nosExpandidos << endl;
}

void resolverDFSComMelhorSolucao(const vector<int> &estadoInicial, const vector<int> &objetivo, const vector<int> &maximos, ofstream &outputFile)
{
    stack<pair<vector<int>, vector<string>>> pilha;
    map<vector<int>, int> visitados; // Armazena o número de movimentos para cada estado
    vector<string> melhorCaminho;
    int menorNumeroMovimentos = numeric_limits<int>::max(); // Valor máximo possível para comparações

    pilha.push({estadoInicial, {}});
    visitados[estadoInicial] = 0;

    while (!pilha.empty())
    {
        auto [estadoAtual, caminho] = pilha.top();
        pilha.pop();
        int movimentosAtuais = caminho.size();

        if (objetivoAtingido(estadoAtual, objetivo))
        {
            if (movimentosAtuais < menorNumeroMovimentos)
            {
                menorNumeroMovimentos = movimentosAtuais;
                melhorCaminho = caminho;
            }
            continue; // Continue procurando outras soluções possíveis
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estadoAtual, maximos);
        // Ordenar os novos estados para garantir que a busca siga sempre o caminho mais à esquerda
        sort(novosEstados.begin(), novosEstados.end(), [](const pair<vector<int>, string> &a, const pair<vector<int>, string> &b)
             { return a.second < b.second; });

        for (const auto &[novoEstado, movimento] : novosEstados)
        {
            if (visitados.find(novoEstado) == visitados.end() || movimentosAtuais + 1 < visitados[novoEstado])
            {
                visitados[novoEstado] = movimentosAtuais + 1;
                auto novoCaminho = caminho;
                novoCaminho.push_back(movimento);
                pilha.push({novoEstado, novoCaminho});
            }
        }
    }

    if (menorNumeroMovimentos < numeric_limits<int>::max())
    {
        outputFile << "Melhor solução encontrada!" << endl;
        outputFile << "Número de movimentos: " << menorNumeroMovimentos << endl;
    }
    else
    {
        outputFile << "Nenhuma solução encontrada." << endl;
    }
}

// Função de busca gulosa
// Função de heurística gulosa (distância de Manhattan entre o estado atual e o objetivo)
int heuristicaGulosa(const vector<int>& estado, const vector<int>& objetivo) {
    int somaDiferencas = 0;
    for (int i = 0; i < estado.size(); i++) {
        somaDiferencas += abs(estado[i] - objetivo[i]);
    }
    return somaDiferencas;
}

// Função para resolver o problema usando o algoritmo Guloso
void resolverGuloso(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    set<vector<int>> visitados;  // Conjunto para armazenar os estados já visitados
    priority_queue<No, vector<No>, greater<No>> pq;  // Fila de prioridade com base na heurística (menor primeiro)
    int nosExpandidos = 0;

    pq.push({estadoInicial, {}, heuristicaGulosa(estadoInicial, objetivo)});
    visitados.insert(estadoInicial);

    while (!pq.empty()) {
        No noAtual = pq.top();
        pq.pop();
        nosExpandidos++;

        if (objetivoAtingido(noAtual.estado, objetivo)) {
            outputFile << "Solução encontrada com o algoritmo Guloso!" << endl;
            outputFile << "Número de movimentos: " << noAtual.caminho.size() << endl;
            outputFile << "Nós visitados: " << visitados.size() << endl;
            outputFile << "Nós expandidos: " << nosExpandidos << endl;
            return;
        }

        vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(noAtual.estado, maximos);
        for (const auto& [novoEstado, movimento] : novosEstados) {
            if (visitados.find(novoEstado) == visitados.end()) {
                visitados.insert(novoEstado);
                vector<string> novoCaminho = noAtual.caminho;
                novoCaminho.push_back(movimento);
                int heuristica = heuristicaGulosa(novoEstado, objetivo);
                pq.push({novoEstado, novoCaminho, heuristica});
            }
        }
    }

    outputFile << "Nenhuma solução encontrada com o algoritmo Guloso." << endl;
    outputFile << "Nós visitados: " << visitados.size() << endl;
    outputFile << "Nós expandidos: " << nosExpandidos << endl;
}

// Função de busca limitada por custo com IDA*
bool buscaIDA(const vector<int>& estado, const vector<int>& objetivo, const vector<int>& maximos, int limite, int& custoLimite, vector<string>& caminho, ofstream& outputFile) {
    int custoMovimento = *max_element(maximos.begin(), maximos.end());
    int custoAtual = heuristica(estado, objetivo) + caminho.size() * custoMovimento;

    //outputFile << "Custo Atual: " << custoAtual << ", Limite: " << limite << endl;

    if (custoAtual > limite) {
        custoLimite = custoAtual;
        return false;
    }

    if (objetivoAtingido(estado, objetivo)) {
        outputFile << "Solução encontrada com IDA*!" << endl;
        outputFile << "Número de movimentos: " << caminho.size() << endl;
        return true;
    }

    bool encontrado = false;
    vector<pair<vector<int>, string>> novosEstados = gerarNovosEstados(estado, maximos);
    for (const auto& [novoEstado, movimento] : novosEstados) {
        caminho.push_back(movimento);
        encontrado = buscaIDA(novoEstado, objetivo, maximos, limite, custoLimite, caminho, outputFile);
        if (encontrado) break;
        caminho.pop_back();
    }

    return encontrado;
}

// Função para resolver o problema usando IDA*
void resolverIDAEstrela(const vector<int>& estadoInicial, const vector<int>& objetivo, const vector<int>& maximos, ofstream& outputFile) {
    int limite = heuristica(estadoInicial, objetivo) + 0; // Inicia o limite com a heurística inicial
    vector<string> caminho;
    int custoLimite;
    int iteracoes = 0;

    //outputFile << "Iniciando IDA* com limite inicial: " << limite << endl;

    while (true) {
        iteracoes++;
        bool encontrado = buscaIDA(estadoInicial, objetivo, maximos, limite, custoLimite, caminho, outputFile);
        if (encontrado) break;
        //outputFile << "Limite aumentado para: " << custoLimite << endl;
        limite = custoLimite;
    }

    outputFile << "Número de iterações do limite: " << iteracoes << endl;
}


int main() {
    vector<int> maximos;
    vector<int> estadoInicial;
    vector<int> objetivo;
    bool usarEntradaCustomizada;

    cout << "Deseja usar entrada customizada? (1 para sim, 0 para não): ";
    cin >> usarEntradaCustomizada;

    if (usarEntradaCustomizada) {
        int numJarro;
        cout << "Quantos jarros serão usados? ";
        cin >> numJarro;

        maximos.resize(numJarro);
        estadoInicial.resize(numJarro);
        objetivo.resize(numJarro);

        cout << "Digite o volume máximo de cada jarro em ordem: ";
        for (int i = 0; i < numJarro; ++i) {
            cin >> maximos[i];
        }

        cout << "Digite o estado inicial de cada jarro em ordem: ";
        for (int i = 0; i < numJarro; ++i) {
            cin >> estadoInicial[i];
        }

        cout << "Digite o estado objetivo de cada jarro em ordem: ";
        for (int i = 0; i < numJarro; ++i) {
            cin >> objetivo[i];
        }
    } else {
        maximos = {1, 8, 6, 9}; // Exemplo com 4 jarros
        estadoInicial = vector<int>(4, 0);
        objetivo = {1, 5, 2, 8}; // Exemplo com 4 jarros
    }

    cout << "Escolha o algoritmo para resolver o problema:\n";
    cout << "1. Busca em Largura (BFS)\n";
    cout << "2. Backtracking Iterativo\n";
    cout << "3. A*\n";
    cout << "4. IDA*\n";
    cout << "5. Busca em Profundidade\n";
    cout << "6. Guloso\n";
    cout << "7. Executar todos\n";
    int escolha;
    cin >> escolha;

    if (escolha == 7)
    {
        for (escolha = 1; escolha < 7; escolha++)
        {

            string nomeArquivo;
            switch (escolha)
            {
            case 1:
                nomeArquivo = "resultado_BFS.txt";
                break;
            case 2:
                nomeArquivo = "resultado_Backtracking.txt";
                break;
            case 3:
                nomeArquivo = "resultado_AEstrela.txt";
                break;
            case 4:
                nomeArquivo = "resultado_IDAEstrela.txt";
                break;
            case 5:
                nomeArquivo = "resultado_DFS.txt";
                break;
            case 6:
                nomeArquivo = "resultado_Guloso.txt";
                break;
            default:
                cerr << "Escolha inválida." << endl;
                return 1;
            }

            // Abre o arquivo para escrita
            ofstream outputFile(nomeArquivo);
            if (!outputFile)
            {
                cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << endl;
                return 1;
            }

            auto inicio = high_resolution_clock::now();

            if (escolha == 1)
            {
                resolverBFS(estadoInicial, objetivo, maximos, outputFile);
            }
            else if (escolha == 2)
            {
                resolverBacktrackingIterativo(estadoInicial, objetivo, maximos, outputFile);
            }
            else if (escolha == 3)
            {
                resolverAEstrela(estadoInicial, objetivo, maximos, outputFile);
            }
            else if (escolha == 4)
            {
                resolverIDAEstrela(estadoInicial, objetivo, maximos, outputFile);
            }
            else if (escolha == 5)
            {
                resolverDFS(estadoInicial, objetivo, maximos, outputFile);
            }
            else if (escolha == 6)
            {
                resolverGuloso(estadoInicial, objetivo, maximos, outputFile);
            }
            else
            {
                outputFile << "Escolha inválida." << endl;
            }

            auto fim = high_resolution_clock::now();
            auto duracao = duration_cast<milliseconds>(fim - inicio);
            outputFile << "Tempo de execução: " << duracao.count() << " ms" << endl;

            // Fecha o arquivo
            outputFile.close();
        }
    }else{
    string nomeArquivo;
    switch (escolha)
    {
    case 1:
        nomeArquivo = "resultado_BFS.txt";
        break;
    case 2:
        nomeArquivo = "resultado_Backtracking.txt";
        break;
    case 3:
        nomeArquivo = "resultado_AEstrela.txt";
        break;
    case 4:
        nomeArquivo = "resultado_IDAEstrela.txt";
        break;
    case 5:
        nomeArquivo = "resultado_DFS.txt";
        break;
    case 6:
        nomeArquivo = "resultado_Guloso.txt";
        break;
    default:
        cerr << "Escolha inválida." << endl;
        return 1;
    }

    // Abre o arquivo para escrita
    ofstream outputFile(nomeArquivo);
    if (!outputFile)
    {
        cerr << "Erro ao abrir o arquivo para escrita: " << nomeArquivo << endl;
        return 1;
    }

    auto inicio = high_resolution_clock::now();

    if (escolha == 1)
    {
        resolverBFS(estadoInicial, objetivo, maximos, outputFile);
    }
    else if (escolha == 2)
    {
        resolverBacktrackingIterativo(estadoInicial, objetivo, maximos, outputFile);
    }
    else if (escolha == 3)
    {
        resolverAEstrela(estadoInicial, objetivo, maximos, outputFile);
    }
    else if (escolha == 4)
    {
        resolverIDAEstrela(estadoInicial, objetivo, maximos, outputFile);
    }
    else if (escolha == 5)
    {
        resolverDFS(estadoInicial, objetivo, maximos, outputFile);
    }
    else if (escolha == 6)
    {
        resolverGuloso(estadoInicial, objetivo, maximos, outputFile);
    }
    else
    {
        outputFile << "Escolha inválida." << endl;
    }

    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<milliseconds>(fim - inicio);
    outputFile << "Tempo de execução: " << duracao.count() << " ms" << endl;

    // Fecha o arquivo
    outputFile.close();
    }

    return 0;
}