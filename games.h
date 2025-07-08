#ifndef GAMES_H            // ----- include guard --------------
#define GAMES_H

#include <string>
#include <memory>          // std::unique_ptr  (opcional, se quiser modernizar)
#include <fstream>

// ---------- Estruturas ----------

struct Texto {
    int     id;
    int     idPai;
    std::string text;
    char    lado;
    Texto*  prox;
};

struct Nodo {
    int     v;
    std::string text;
    Nodo*  dir;
    Nodo*  esq;
};

// ---------- Classe Lista ----------

class Lista {
public:
    Lista();
    void inserir(int id, int idPai, const std::string& text, char lado);
    void lerArquivoLista(const std::string& nomeArquivo = "jogo.txt");
    Texto* getInicio() const;

private:
    bool listaVazia(const Texto* p) const;

    Texto* inicio;
    Texto* fim;
};

// ---------- Classe Arvore ----------

class Arvore {
public:
    Arvore();
    void construirArvore(Texto* lista);
    Nodo* getRaiz() const;

private:
    Nodo* raiz;

    Nodo* buscarNodo(Nodo* raiz, int id);
};

// ---------- Classe Jogo ----------

class Jogo {
public:
    void iniciar();

private:
    void jogar(Nodo* atual);
    void historiaJogo(const std::string& arq = "historia.txt");
    void menu();
    void segundoMenu();
};

#endif  // GAMES_H
