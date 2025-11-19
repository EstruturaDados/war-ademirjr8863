#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME 30
#define MAX_COR 10

typedef struct {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
} Territorio;

typedef struct {
    char cor[MAX_COR];
    char* missao;
    int mostrada;
} Player;

/* PROTÓTIPOS */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, char* corJogador);
void exibirMissao(const char* missao);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoria(Territorio* mapa, Player* players, int nPlayers);

/* IMPLEMENTAÇÃO — ATRIBUIR MISSÃO */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]);
}

/* EXIBIR MISSÃO — SÓ MOSTRA UMA VEZ */
void exibirMissao(const char* missao) {
    printf("Sua missão: %s\n", missao);
}

/* ATAQUE SIMPLES COM DADO 1..6 */
void atacar(Territorio* atacante, Territorio* defensor) {
    if (!atacante || !defensor) return;

    if (strcmp(atacante->cor, defensor->cor) == 0) return;

    int dadoA = (rand() % 6) + 1;
    int dadoD = (rand() % 6) + 1;

    if (dadoA > dadoD) {
        int transfer = atacante->tropas / 2;
        if (transfer < 1) transfer = 1;

        defensor->tropas = transfer;
        strcpy(defensor->cor, atacante->cor);

        atacante->tropas -= transfer;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        if (atacante->tropas > 0) atacante->tropas -= 1;
    }
}

/* VERIFICAR MISSÃO — LÓGICA SIMPLES */
int verificarMissao(char* missao, Territorio* mapa, int tamanho, char* corJogador) {

    /* 1 — Conquistar 3 territórios */
    if (strstr(missao, "3") && strstr(missao, "territ")) {
        int cont = 0;
        for (int i = 0; i < tamanho; i++)
            if (strcmp(mapa[i].cor, corJogador) == 0) cont++;
        return cont >= 3;
    }

    /* 2 — Eliminar cor vermelha */
    if (strstr(missao, "vermelha")) {
        for (int i = 0; i < tamanho; i++)
            if (strcmp(mapa[i].cor, "vermelha") == 0 && mapa[i].tropas > 0)
                return 0;
        return 1;
    }

    /* 3 — Controlar todo o mapa */
    if (strstr(missao, "todos os territorios")) {
        for (int i = 0; i < tamanho; i++)
            if (strcmp(mapa[i].cor, corJogador) != 0) return 0;
        return 1;
    }

    /* 4 — Conquistar 2 territórios seguidos */
    if (strstr(missao, "2 territorios")) {
        for (int i = 0; i < tamanho - 1; i++)
            if (strcmp(mapa[i].cor, corJogador) == 0 &&
                strcmp(mapa[i + 1].cor, corJogador) == 0)
                return 1;
        return 0;
    }

    /* 5 — Ter pelo menos 10 tropas */
    if (strstr(missao, "10 tropas")) {
        int soma = 0;
        for (int i = 0; i < tamanho; i++)
            if (strcmp(mapa[i].cor, corJogador) == 0)
                soma += mapa[i].tropas;
        return soma >= 10;
    }

    return 0;
}

/* LIBERAR MEMÓRIA */
void liberarMemoria(Territorio* mapa, Player* players, int nPlayers) {
    free(mapa);
    for (int i = 0; i < nPlayers; i++)
        free(players[i].missao);
    free(players);
}

/* MAIN */
int main() {
    srand(time(NULL));

    char* missoes[] = {
        "Conquistar 3 territorios seguidos",
        "Eliminar todas as tropas da cor vermelha",
        "Controlar todos os territorios do mapa",
        "Conquistar 2 territorios seguidos",
        "Ter pelo menos 10 tropas no total"
    };

    int totalMissoes = 5;

    int tamanhoMapa = 6;
    Territorio* mapa = calloc(tamanhoMapa, sizeof(Territorio));

    char* cores[] = {"vermelha", "azul", "amarela", "vermelha", "azul", "amarela"};
    int tropas[] = {3, 2, 4, 1, 5, 2};

    for (int i = 0; i < tamanhoMapa; i++) {
        sprintf(mapa[i].nome, "Territorio-%d", i + 1);
        strcpy(mapa[i].cor, cores[i]);
        mapa[i].tropas = tropas[i];
    }

    int nPlayers = 2;
    Player* players = malloc(nPlayers * sizeof(Player));

    strcpy(players[0].cor, "vermelha");
    strcpy(players[1].cor, "azul");

    for (int i = 0; i < nPlayers; i++) {
        players[i].missao = malloc(256);
        atribuirMissao(players[i].missao, missoes, totalMissoes);
        players[i].mostrada = 0;
    }

    for (int i = 0; i < nPlayers; i++) {
        printf("Player %d (%s): ", i + 1, players[i].cor);
        exibirMissao(players[i].missao);
        players[i].mostrada = 1;
    }

    int vencedor = -1;

    for (int turno = 1; turno <= 20 && vencedor == -1; turno++) {

        int a = rand() % tamanhoMapa;
        int d = rand() % tamanhoMapa;

        if (a != d && strcmp(mapa[a].cor, mapa[d].cor) != 0)
            atacar(&mapa[a], &mapa[d]);

        for (int p = 0; p < nPlayers; p++) {
            if (verificarMissao(players[p].missao, mapa, tamanhoMapa, players[p].cor)) {
                vencedor = p;
                break;
            }
        }
    }

    if (vencedor != -1)
        printf("\n=== O jogador %d venceu! ===\n", vencedor + 1);
    else
        printf("\nNinguém venceu em 20 turnos.\n");

    printf("\nEstado final do mapa:\n");
    for (int i = 0; i < tamanhoMapa; i++) {
        printf("%s - %s - Tropas: %d\n", mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }

    liberarMemoria(mapa, players, nPlayers);

    return 0;
}
