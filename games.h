#ifndef GAMES_H
#define GAMES_H

#include <string>

// Estruturas
struct Texto {
    int id;
    int idPai;
    std::string text;
    char lado;
    Texto* prox;
};

struct Nodo {
    int v;
    std::string text;
    Nodo* dir;
    Nodo* esq;
};

struct Score {
    std::string nome;
    int jogos;
    int vitorias;
    int derrotas;
    Score* ant;
    Score* prox;
};

// Lista de Texto (para construir árvore)
class Lista {
private:
    Texto* inicio;
    Texto* fim;

public:
    Lista();
    bool listaVazia(const Texto* p) const;
    void inserir(int id, int idPai, const std::string& text, char lado);
    void lerArquivoLista(const std::string& nomeArquivo = "jogo.txt");
    Texto* getInicio() const;
};

// Árvore de decisões
class Arvore {
private:
    Nodo* raiz;
    Nodo* buscarNodo(Nodo* r, int id);

public:
    Arvore();
    void construirArvore(Texto* lista);
    Nodo* getRaiz() const;
};

// Lista de pontuação
class ListaScore {
private:
    Score* inicio;
    Score* fim;

public:
    ListaScore();
    void adicionarOuAtualizar(const std::string& nome, bool venceu);
    void listar() const;
    Score* buscarPorNome(const std::string& nome) const;
    Score* buscarPorJogos(int jogos) const;
};

// Lógica principal do jogo
class Jogo {
private:
    ListaScore placar;

public:
    void iniciar();
    void jogar(Nodo* atual);
    void historiaJogo(const std::string& arq);
    void segundoMenu();
    void primeiroMenu();
    void submenuTecnico();
    ListaScore& getPlacar() { return placar; }
};

#endif




