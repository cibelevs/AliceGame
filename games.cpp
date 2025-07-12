#include "games.h"
#include <iostream>
#include <fstream>   // para std::ifstream
#include <string>
#include <sstream>
#include <cstdio>    // sscanf

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
        // Aqui pede o nome do jogador:
        std::string nome;
        std::cout << "\nDigite seu nome: ";
        std::cin >> nome;

        // Pergunta se venceu ou perdeu (você pode adaptar a pergunta)
        char resultado;
        std::cout << "Você venceu? (s/n): ";
        std::cin >> resultado;
        bool venceu = (resultado == 's' || resultado == 'S');

        // Atualiza o placar
        placar.adicionarOuAtualizar(nome, venceu);
    } else {
        std::cout << "\nAlgo deu errado. Caminho invalido.\n";
    }
}

void Jogo::primeiroMenu() {
    int op;
    do {
        std::cout << "\nBem-vindo ao País das Maravilhas!";
        std::cout << "\n1 - Conhecer a história do jogo";
        std::cout << "\n2 - Regras do jogo";
        std::cout << "\n3 - Verificar o Score do jogo";
        std::cout << "\n4 - Jogar";
        std::cout << "\n-1 - Sair";
        std::cout << "\nEscolha: ";
        std::cin >> op;

        switch (op) {
            case 1:
                historiaJogo("historia.txt");  // Exemplo de arquivo
                break;
            case 2:
                historiaJogo("regras.txt");
                break;
            case 3:
                placar.listar();
                break;
            case 4:
                iniciar();  // Começa o jogo
                segundoMenu();
                break;
            case -1:
                std::cout << "Saindo do jogo...\n";
                break;
            default:
                std::cout << "Opcao invalida!\n";
        }
    } while (op != -1);
}

void Jogo:: submenuTecnico() {
    int opcao;
    do {
        std::cout << "\n1 - Mostrar lista que criou a árvore"
                  << "\n2 - Listar árvore em ordem"
                  << "\n3 - Buscar jogador por nome"
                  << "\n4 - Buscar jogador por número de jogos"
                  << "\n5 - Voltar\nEscolha: ";
        std::cin >> opcao;

        switch(opcao) {
            case 1:
            mostrarListaOriginal();
                break;
            case 2:
            listarArvoreEmOrdem();
                break;
            case 3:
             buscarJogadorPorNome();
                break;
            case 4:
             buscarJogadorPorJogos();
                break;
            case 5:
                std::cout << "Voltando...\n";
                break;
            default:
                std::cout << "Opcao invalida!\n";
        }
    } while(opcao != 5);
}


void Jogo::segundoMenu() {
    int opcao;
    do {
        std::cout << "\n1 - Jogar novamente";
        std::cout << "\n2 - Informações técnicas";
        std::cout << "\n3 - Sair";
        std::cout << "\nEscolha: ";
        std::cin >> opcao;

        switch (opcao) {
            case 1:
                iniciar();
                break;
            case 2:
                submenuTecnico();
                break;
            case 3:
                std::cout << "Voltando ao menu principal...\n";
                break;
            default:
                std::cout << "Opcao invalida!\n";
        }
    } while (opcao != 3);
}


void Jogo::historiaJogo(const std::string& arq) {
    std::ifstream hist(arq);
    if (!hist) {
        std::cerr << "Erro ao abrir " << arq << '\n';
        return;
    }
    std::string linha;
    while (std::getline(hist, linha))
        std::cout << linha << '\n';
}

void Jogo::iniciar() {
    Lista list;
    list.lerArquivoLista("jogo.txt");   
    Arvore tree;
    tree.construirArvore(list.getInicio());
    jogar(tree.getRaiz());
}

// ========== SCORE ==========

ListaScore::ListaScore() : inicio(nullptr), fim(nullptr) {}

void ListaScore::adicionarOuAtualizar(const std::string& nome, bool venceu) {
    Score* atual = buscarPorNome(nome);
    if (atual) {
        atual->jogos++;
        if (venceu) atual->vitorias++;
        else        atual->derrotas++;
    } else {
        Score* novo = new Score{nome, 1, venceu ? 1 : 0, venceu ? 0 : 1, nullptr, nullptr};
        if (!inicio) {
            inicio = fim = novo;
        } else {
            fim->prox = novo;
            novo->ant = fim;
            fim = novo;
        }
    }
}

void ListaScore::listar() const {
    std::cout << "\n--- SCORE ---\n";
    Score* atual = inicio;
    while (atual) {
        std::cout << "Nome: " << atual->nome
                  << " | Jogos: " << atual->jogos
                  << " | Vitórias: " << atual->vitorias
                  << " | Derrotas: " << atual->derrotas << '\n';
        atual = atual->prox;
    }
    std::cout << "--------------\n";
}

Score* ListaScore::buscarPorNome(const std::string& nome) const {
    Score* atual = inicio;
    while (atual) {
        if (atual->nome == nome) return atual;
        atual = atual->prox;
    }
    return nullptr;
}

Score* ListaScore::buscarPorJogos(int jogos) const {
    Score* atual = inicio;
    while (atual) {
        if (atual->jogos == jogos) return atual;
        atual = atual->prox;
    }
    return nullptr;
}

// ==========INFORMAÇÕES TÉCNICAS==========

void Jogo::mostrarListaOriginal() {
    Lista list;
    list.lerArquivoLista("jogo.txt");
    Texto* atual = list.getInicio();
    std::cout << "\n--- LISTA ORIGINAL ---\n";
    while (atual) {
        std::cout << "ID: " << atual->id 
                  << " | Pai: " << atual->idPai 
                  << " | Lado: " << atual->lado 
                  << " | Texto: " << atual->text << "\n";
        atual = atual->prox;
    }
}





void Arvore::imprimirEmOrdemRecursivo(Nodo* nodo) {
    if (nodo == nullptr) return;
    imprimirEmOrdemRecursivo(nodo->esq);
    std::cout << "ID: " << nodo->v << " | Texto: " << nodo->text << "\n";
    imprimirEmOrdemRecursivo(nodo->dir);
}

void Arvore::imprimirEmOrdem() {
    std::cout << "\n--- ARVORE EM ORDEM ---\n";
    imprimirEmOrdemRecursivo(raiz);
}

void Jogo::listarArvoreEmOrdem() {
    // Usa a mesma árvore que já existe no jogo
    Lista list;
    list.lerArquivoLista("jogo.txt");
    
    Arvore tree;  // Esta é a árvore que você já tem
    tree.construirArvore(list.getInicio());
    
    // Chama o método da árvore
    tree.imprimirEmOrdem();
}

void Jogo::buscarJogadorPorNome() {
    std::string nome;
    std::cout << "Digite o nome do jogador: ";
    std::cin.ignore(); // Limpa o buffer antes de ler linha completa
    std::getline(std::cin, nome); // Permite nomes com espaços
    
    Score* jogador = placar.buscarPorNome(nome);
    
    if (jogador) {
        std::cout << "\n--- JOGADOR ENCONTRADO ---\n"
                  << "Nome: " << jogador->nome << "\n"
                  << "Jogos: " << jogador->jogos << "\n"
                  << "Vitórias: " << jogador->vitorias << "\n"
                  << "Derrotas: " << jogador->derrotas << "\n";
    } else {
        std::cout << "Jogador não encontrado!\n";
    }
}

void Jogo::buscarJogadorPorJogos() {
    int jogos;
    std::cout << "Digite o número de jogos: ";
    std::cin >> jogos;
    
    Score* jogador = placar.buscarPorJogos(jogos);
    
    if (jogador) {
        std::cout << "\n--- JOGADOR ENCONTRADO ---\n"
                  << "Nome: " << jogador->nome << "\n"
                  << "Jogos: " << jogador->jogos << "\n"
                  << "Vitórias: " << jogador->vitorias << "\n"
                  << "Derrotas: " << jogador->derrotas << "\n";
    } else {
        std::cout << "Nenhum jogador encontrado com " << jogos << " jogos!\n";
    }
}