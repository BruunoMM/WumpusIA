#pragma once
#include <stdbool.h>

#define MAP_SIZE 12
#define MAX_POCOS 8
#define MAX_INIM_20 2
#define MAX_INIM_50 2

typedef struct inimigo {
	int vida;
	int ataque;
}Inimigo;
typedef struct cell {
	Inimigo *inimigo;
	bool poco;
	bool brisa;
	bool brilho;
	bool cheiro;
	bool wumpus;
}Cell;

typedef struct pos {
	int i;
	int j;
} Pos;

// Função que gera o mapa de forma aleatória, respeitando todas as regras.
int gera_mapa();
// Função que adiciona o ouro no mapa.
int add_gold();
// Função que adiciona o Wumpus no mapa.
int add_wumpus();
// Função que adiciona o poço no mapa.
int add_pit();
// Função que adiciona um inimigo do referido tipo no mapa.
int add_enemy(int tipoInimigo);
// Função que diz se existe ouro na referida posição.
bool isGold(Pos pos);
// Função que diz se existe poço na referida posição.
bool isPit(Pos pos);
// Função que diz se existe inimigo/wumpus na referida posição.
bool isEnemy(Pos pos);
