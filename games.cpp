#include "games.h"
#include <iostream>
#include <sstream>
#include <cstdio>   // sscanf

// ========== LISTA ==========

Lista::Lista() : inicio(nullptr), fim(nullptr) {}

bool Lista::listaVazia(const Texto* p) const {
    return p == nullptr;
}

void Lista::inserir(int id, int idPai, const std::string& text, char lado) {
    auto* novo = new Texto{id, idPai, text, lado, nullptr};
    if (listaVazia(inicio)) {
        inicio = fim = novo;
    } else {
        fim->prox = novo;
        fim = novo;
    }
}

void Lista::lerArquivoLista(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        std::cerr << "Nao foi possivel abrir " << nomeArquivo << '\n';
        return;
    }

    std::string linha;
    while (std::getline(arquivo, linha)) {
        int id, idPai; char lado; char textoBuffer[1000];
        if (std::sscanf(linha.c_str(), "%d %d %c %[^\n]", &id, &idPai, &lado, textoBuffer) == 4) {
            inserir(id, idPai, textoBuffer, lado);
        } else {
            std::cerr << "Linha invalida: " << linha << '\n';
        }
    }
}

Texto* Lista::getInicio() const { return inicio; }

// ========== ARVORE ==========

Arvore::Arvore() : raiz(nullptr) {}

Nodo* Arvore::buscarNodo(Nodo* r, int id) {
    if (!r) return nullptr;
    if (r->v == id) return r;
    if (auto* e = buscarNodo(r->esq, id)) return e;
    return buscarNodo(r->dir, id);
}

void Arvore::construirArvore(Texto* lista) {
    while (lista) {
        auto* novo = new Nodo{lista->id, lista->text, nullptr, nullptr};
        if (lista->idPai == -1) {
            raiz = novo;
        } else {
            if (auto* pai = buscarNodo(raiz, lista->idPai)) {
                (lista->lado == 'E' ? pai->esq : pai->dir) = novo;
            }
        }
        lista = lista->prox;
    }
}

Nodo* Arvore::getRaiz() const { return raiz; }

// ========== JOGO ==========

void Jogo::jogar(Nodo* atual) {
    while (atual && (atual->esq || atual->dir)) {
        std::cout << "\n" << atual->text << "\n(s para SIM, n para NAO): ";
        char op; std::cin >> op;

        if (op == 's' || op == 'S')      atual = atual->dir;
        else if (op == 'n' || op == 'N') atual = atual->esq;
        else  std::cout << "Opcao invalida!\n";
    }

    if (atual) {
        std::cout << "\nFim da aventura: " << atual->text << "\n";
        segundoMenu();
    } else {
        std::cout << "\nAlgo deu errado. Caminho invalido.\n";
    }
}

void Jogo::segundoMenu() {
    std::cout << "\n1‑ Jogar novamente\n2‑ Informacoes tecnicas\n3‑ Sair\n";
}

void Jogo::historiaJogo(const std::string& arq) {
    std::ifstream hist(arq);
    if (!hist) { std::cerr << "Erro ao abrir " << arq << '\n'; return; }
    std::string linha;
    while (std::getline(hist, linha)) std::cout << linha << '\n';
}

void Jogo::menu() {
    /* ... mesmo código, opcional de mover para cá ... */
}

void Jogo::iniciar() {
    Lista list;
    list.lerArquivoLista();                 // usa caminho default
    Arvore tree;
    tree.construirArvore(list.getInicio());
    jogar(tree.getRaiz());
}
