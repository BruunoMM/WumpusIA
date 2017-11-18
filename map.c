#include <stdio.h>
#include <stdlib.h>
#include "map.h"

bool agentAdj = false; // se alguma adjacencia do agente possui poço, wumpus ou inimigo.
Cell mapa[MAP_SIZE][MAP_SIZE];

int gera_mapa() {
	int i;
	add_gold();
	add_wumpus(mapa); // adiciono o wumpus num local aleatório
	i = 0;
	while (i<MAX_POCOS) {
		add_pit(mapa); // adiciono um poço.
		i++;
	}
	i = 0;
	while (i<MAX_INIM_20) {
		add_enemy(1);
		i++;
	}
	i = 0;
	while (i < MAX_INIM_50) {
		add_enemy(2);
		i++;
	}
	return 0;
}

int add_gold() {
	Pos pos;
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while (pos.i == 0 && pos.j == 0); // escolho novamente, caso o ouro caia no espaço inicial do agente.

	mapa[pos.i][pos.j].brilho = true;

	if ((pos.i == 0 && pos.j == 1) || (pos.i == 1 && pos.j == 0))
		agentAdj = true;	// se o wumpus for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}

int add_wumpus() {
	Pos pos;
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((pos.i == 0 && pos.j == 0) || isGold(pos)); // escolho novamente, caso o wumpus caia no espaço inicial do agente.

	mapa[pos.i][pos.j].wumpus = true;
	mapa[pos.i - 1][pos.j].cheiro = true;
	mapa[pos.i + 1][pos.j].cheiro = true;
	mapa[pos.i][pos.j - 1].cheiro = true;
	mapa[pos.i][pos.j + 1].cheiro = true;

	if ((pos.i == 0 && pos.j == 1) || (pos.i == 1 && pos.j == 0))
		agentAdj = true;	// se o wumpus for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}

int add_pit() {
	Pos pos;
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((((pos.i == 0 && pos.j == 0) || isEnemy(pos))
		|| isPit(pos) || (((pos.i == 1 && pos.j == 0)
			|| (pos.i == 0 && pos.j == 1)) && agentAdj == true))); // escolho novamente, caso o poço caia no espaço inicial do agente, onde ja tenha poço ou onde já tenha wumpus.

	mapa[pos.i][pos.j].poco = true;
	mapa[pos.i - 1][pos.j].brisa = true;
	mapa[pos.i + 1][pos.j].brisa = true;
	mapa[pos.i][pos.j - 1].brisa = true;
	mapa[pos.i][pos.j + 1].brisa = true;

	if ((pos.i == 0 && pos.j == 1) || (pos.i == 1 && pos.j == 0))
		agentAdj = true;	// se o poço for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}

int add_enemy(int tipoInimigo) {
	Pos pos;
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((((pos.i == 0 && pos.j == 0) || isEnemy(pos))
		|| isPit(pos) || (((pos.i == 1 && pos.j == 0)
			|| (pos.i == 0 && pos.j == 1)) && agentAdj == true) || (mapa[pos.i][pos.j].inimigo != NULL))); // escolho novamente, caso o poço caia no espaço inicial do agente, onde ja tenha poço, onde já tenha wumpus ou onde ja tenha inimigo.

	mapa[pos.i][pos.j].inimigo = (Inimigo*)malloc(sizeof(Inimigo));
	if (mapa[pos.i][pos.j].inimigo == NULL)
		return 1; // erro de memoria
	if (tipoInimigo == 1)
		mapa[pos.i][pos.j].inimigo->ataque = 20;	// inimigo 1
	else
		mapa[pos.i][pos.j].inimigo->ataque = 50;	// inimigo 2

	mapa[pos.i][pos.j].inimigo->vida = 100;

	mapa[pos.i - 1][pos.j].cheiro = true;
	mapa[pos.i + 1][pos.j].cheiro = true;
	mapa[pos.i][pos.j - 1].cheiro = true;
	mapa[pos.i][pos.j + 1].cheiro = true;

	if ((pos.i == 0 && pos.j == 1) || (pos.i == 1 && pos.j == 0))
		agentAdj = true;	// se o poço for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}

bool isGold(Pos pos) {
	if (mapa[pos.i][pos.j].brilho)
		return true;
	return false;
}

bool isPit(Pos pos) {
	if (mapa[pos.i][pos.j].poco)
		return true;
	return false;
}

bool isEnemy(Pos pos) {
	if (mapa[pos.i][pos.j].inimigo || mapa[pos.i][pos.j].wumpus == true)
		return true;
	return false;
}
