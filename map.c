#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "agente.h"

bool agentAdj = false; // se alguma adjacencia do agente possui poço, wumpus ou inimigo.

// função que inicializa todos os campos com false / null
void inicializaMapa();

// Essa função deve seguir essa ordem de chamada das funções auxiliares para gerar o mapa
// As funções auxiliares verificam as regras assumindo essa ordem de chamada:
// Adiciona Ouro -> Adiciona Wumpus -> Adiciona Poços -> Adiciona Inimigos
int gera_mapa() {
	int i;

	//inicializando o mapa
	inicializaMapa();
	//adicionando os ouros
	i = 0;
	while (i<MAX_OURO){
		add_gold();
		i++;
	}
	//adicionando o Wumpus
	add_wumpus();
	//adicionando poços
	i = 0;
	while (i<MAX_POCOS) {
		add_pit(); // adiciono um poço.
		i++;
	}
	//adicionando inimigos de dano 20
	i = 0;
	while (i<MAX_INIM_20) {
		add_enemy(1);
		i++;
	}
	//adicionando inimigos de dano 50
	i = 0;
	while (i < MAX_INIM_50) {
		add_enemy(2);
		i++;
	}
	#ifdef _DEBUG
		printf("\n\n");
	#endif
	return 0;
}

int add_gold() {
	Pos pos;

//	srand(time(NULL));
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((pos.i == 0 && pos.j == 0 ) || isGold(pos)); // escolho novamente, caso o ouro caia no espaço inicial do agente ou onde já tenha ouro.
	#ifdef _DEBUG
		printf("Ouro Posicao: %d\t%d\n", pos.i, pos.j);
	#endif
	mapa[pos.i][pos.j].brilho = true;

	if ((pos.i == 0 && pos.j == 1) || (pos.i == 1 && pos.j == 0))
		agentAdj = true;	// se o wumpus for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}

int add_wumpus() {
	Pos pos;

//	srand(time(NULL));
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((pos.i == 0 && pos.j == 0) || isGold(pos)); // escolho novamente, caso o wumpus caia no espaço inicial do agente.
	#ifdef _DEBUG
		printf("Wumpus Posicao: %d\t%d\n", pos.i, pos.j);
	#endif
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

//	srand(time(NULL));
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((((pos.i == 0 && pos.j == 0) || isEnemy(pos))
		|| isPit(pos) || isGold(pos) || (((pos.i == 1 && pos.j == 0)
			|| (pos.i == 0 && pos.j == 1)) && agentAdj == true))); // escolho novamente, caso o poço caia no espaço inicial do agente, onde ja tenha poço ou onde já tenha wumpus.
	#ifdef _DEBUG
		printf("Pocos Posicao: %d\t%d\n", pos.i, pos.j);
	#endif
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

//	srand(time(NULL));
	do {
		pos.i = rand() % MAP_SIZE;
		pos.j = rand() % MAP_SIZE;
	} while ((((pos.i == 0 && pos.j == 0) || isEnemy(pos))
		|| isPit(pos) || isGold(pos) || (((pos.i == 1 && pos.j == 0)
			|| (pos.i == 0 && pos.j == 1)) && agentAdj == true) || (mapa[pos.i][pos.j].inimigo != NULL))); // escolho novamente, caso o poço caia no espaço inicial do agente, onde ja tenha poço, onde já tenha wumpus ou onde ja tenha inimigo.
	#ifdef _DEBUG
		printf("Inimigos Posicao: %d\t%d\n", pos.i, pos.j);
	#endif
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

void inicializaMapa() {
	int i, j;
	for (i = 0;i < MAP_SIZE;i++) {
		for (j = 0;j < MAP_SIZE;j++) {
			mapa[i][j].wumpus = false;
			mapa[i][j].cheiro = false;
			mapa[i][j].brilho = false;
			mapa[i][j].brisa = false;
			mapa[i][j].poco = false;
			mapa[i][j].visitado = false;
			mapa[i][j].inimigo = NULL;
		}
	}
}


void desenhaMapa(){	
	int i, e;
	printf("/");
	for(i=0;i<MAP_SIZE-1;i++)
		printf("--------");
	printf("-------\\\n");
	for (i = 0;i < MAP_SIZE;i++) {
		printf("|");
		for (e = 0;e < MAP_SIZE;e++) {
			if(!mapa[i][e].visitado)
				printf("???????");
			else{
				if(i == agente.pos.i && e ==agente.pos.j)
					printf("*");
				else printf(" ");
				if(mapa[i][e].wumpus)
					printf("W");
				else printf(" ");
				if(mapa[i][e].cheiro)
					printf("C");
				else printf(" ");
				if(mapa[i][e].brilho)
					printf("G");
				else printf(" ");
				if(mapa[i][e].brisa)
					printf("B");
				else printf(" ");
				if(mapa[i][e].poco)
					printf("P");
				else printf(" ");
				if(mapa[i][e].inimigo && mapa[i][e].inimigo->ataque == 20)
					printf("A");
				if(mapa[i][e].inimigo && mapa[i][e].inimigo->ataque == 50)
					printf("B");
				else printf(" ");
			}
			printf("|");
		}
		printf("\n");
		//desenhando as linhas no meio
		if(i<MAP_SIZE-1){
			printf("|");
			for(e=0;e<MAP_SIZE-1;e++)
				printf("-------+");
			printf("-------|\n");
		}
		//desenhando a linha no final
		else{
			printf("\\");
			for(e=0;e<MAP_SIZE-1;e++)
				printf("--------");
			printf("-------/\n");
		}
	}
}
