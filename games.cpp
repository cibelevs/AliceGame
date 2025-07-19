#include "games.h"
#include <iostream>
#include <fstream>   // para std::ifstream
#include <string>
#include <sstream>
#include <limits>
#include <cstdio>    // sscanf
#include <algorithm>  // para o std::transform
#include <cctype>     //  para o ::tolower


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

bool entradaInvalida() {
    if (std::cin.fail()) {
        std::cin.clear(); // limpa o estado de erro
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // descarta o restante da linha
        std::cout << "Entrada invalida! Digite um numero.\n";
        return true;
    }
    return false;
}


Nodo* Arvore::getRaiz() const { return raiz; }

// ========== JOGO ==========

void Jogo::jogar(Nodo* atual) {
    while (atual && (atual->esq || atual->dir)) {
        std::cout << "\n" << atual->text << "\n(Digite 's' para SIM, 'n' para NAO, e 'e' para encerrar a sua jornada): ";
        char op; std::cin >> op;

       if (op == 's' || op == 'S') {
            atual = atual->dir;
        } else if (op == 'n' || op == 'N') {
            atual = atual->esq;
        } else if (op == 'e' || op == 'E') {
            std::string nome;
            std::cout << "\nVoce desistiu da jornada. Sera registrado como derrota.\n";
            std::cout << "Digite seu nome: ";
            std::cin.ignore();
            std::getline(std::cin, nome);

            placar.adicionarOuAtualizar(nome, false); // derrota
            std::cout << "\nEncerrando a jornada...\n";
            return;
        } else {
            std::cout << "Opcao invalida!\n";
        }
    }

    if (atual) {
        std::cout << "\nFim da aventura: " << atual->text << "\n";
        // Aqui pede o nome do jogador:
        std::string nome;
        std::cout << "\nDigite seu nome: ";
        std::cin >> nome;

        // se venceu ou perdeu automaticamente
        std::string textoFinal = atual->text;
        std::transform(textoFinal.begin(), textoFinal.end(), textoFinal.begin(), ::tolower); // deixa tudo minúsculo

        bool venceu = textoFinal.find("#venceu") != std::string::npos;
        bool perdeu = textoFinal.find("#perdeu") != std::string::npos;

        if (venceu || perdeu) {
            placar.adicionarOuAtualizar(nome, venceu); 
        } 
        else {
            std::cout << "Nao foi possivel determinar se venceu ou perdeu.\n";
        }
        
    } else {
        std::cout << "\n Opção invalida!\n";
    }
}

void Jogo::primeiroMenu() {
    int op;
    const std::string RESET = "\033[0m";
    const std::string AZUL = "\033[34m";
    const std::string VERDE = "\033[32m";
    const std::string AMARELO = "\033[33m";
    const std::string VERMELHO = "\033[31m";
    do {


         // Desenho de flor decorativa
      
        std::cout << "\n" << AMARELO << "================================================"       << RESET << "\n";
        std::cout << AZUL << "       ★ Bem-vindo ao Pais das Maravilhas ★ " << RESET <<       "\n";
        std::cout << AMARELO         << "================================================"        << RESET << "\n\n";
 



        std::cout << VERDE << " 1 - " << RESET << "Conhecer a historia do jogo\n";
        std::cout << VERDE << " 2 - " << RESET << "Regras do jogo\n";
        std::cout << VERDE << " 3 - " << RESET << "Verificar o Score do jogo\n";
        std::cout << VERDE << " 4 - " << RESET << "Jogar\n";
        std::cout << VERMELHO <<"-1 - " << RESET << "Sair\n" << RESET;
        std::cin >> op;

        if (entradaInvalida()) continue;

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
                break;
        }
    } while (op != -1);
}

void Jogo:: submenuTecnico() {
    int opcao;

    const std::string RESET = "\033[0m";
    const std::string VERDE = "\033[32m";
    const std::string VERMELHO = "\033[31m";
    do {
        std::cout << VERDE << "1- " << RESET << " Mostrar lista que criou a arvore\n";
        std::cout << VERDE << "2- " << RESET << " Listar arvore em ordem\n";
        std::cout << VERDE << "3- " << RESET << "-Buscar jogador por nome\n";
        std::cout << VERDE << "4- " << RESET << " Buscar jogador por número de jogos\n";
        std::cout << VERMELHO << "5- " << RESET << " Voltar\nEscolha: \n" << RESET;;
        std::cin >> opcao;
        if (entradaInvalida()) continue;

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
                break;
        }
    } while(opcao != 5);
}


void Jogo::segundoMenu() {
    int opcao;
    const std::string RESET = "\033[0m";
    const std::string VERDE = "\033[32m";
    const std::string VERMELHO = "\033[31m";
    do {
        std::cout << VERDE << "1 - " << RESET << " Jogar novamente\n";
        std::cout << VERDE << "2 - " << RESET << " Informações tecnicas\n";
        std::cout << VERMELHO << "3 - " << RESET<< " Sair" << RESET ;
        std::cout << "\nEscolha: ";
        std::cin >> opcao;
        if (entradaInvalida()) continue;

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
                break;
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
    std::cout << "\n**** SCORE *****\n";
    Score* atual = inicio;
    while (atual) {
        std::cout << "Nome: " << atual->nome
                  << " | Jogos: " << atual->jogos
                  << " | Vitorias: " << atual->vitorias
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

void Jogo::imprimeJogador(Score * jogador){
    if (jogador != nullptr){
        std::cout << "\n--- JOGADOR ENCONTRADO ---\n"
                  << "Nome: " << jogador->nome << "\n"
                  << "Jogos: " << jogador->jogos << "\n"
                  << "Vitorias: " << jogador->vitorias << "\n"
                  << "Derrotas: " << jogador->derrotas << "\n";
    }
}

void Jogo::buscarJogadorPorNome() {
    std::string nome;
    std::cout << "Digite o nome do jogador: ";
    std::cin.ignore(); // Limpa o buffer antes de ler linha completa
    std::getline(std::cin, nome); // Permite nomes com espaços
    
    Score* jogador = placar.buscarPorNome(nome);
    
    if (jogador) {
        imprimeJogador(jogador);
    } else {
        std::cout << "Jogador não encontrado!\n";
    }
}

void Jogo::buscarJogadorPorJogos() {
    int jogos;
    std::cout << "Digite o numero de jogos: ";
    std::cin >> jogos;
    
    Score* jogador = placar.buscarPorJogos(jogos);
    
    if (jogador) {
        imprimeJogador(jogador);
    } else {
        std::cout << "Nenhum jogador encontrado com " << jogos << " jogos!\n";
    }
}