#include <stdbool.h>
#include <stdio.h>

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

bool agentAdj = false; // se alguma adjacencia do agente possui poço, wumpus ou inimigo.

int gera_mapa(Cell mapa[][12]) {
	int i=0;
	add_wumpus(mapa); // adiciono o wumpus num local aleatório
	while (i<MAX_POCOS){
		add_pit(mapa); // adiciono um poço.
	}
}

int add_wumpus(Cell mapa[][12]) {
	int x,y;
	do {
	x= rand()%12;
	y= rand()%12;
	} while(x==0 && y==0); // escolho novamente, caso o wumpus caia no espaço inicial do agente.

	mapa[x][y].wumpus = true;
	mapa[x-1][y].cheiro = true;
	mapa[x+1][y].cheiro = true;
	mapa[x][y-1].cheiro = true;
	mapa[x][y+1].cheiro = true;
}

int add_pit(Cell mapa[][12]) {
	int x, y;
	do {
		x = rand() % 12;
		y = rand() % 12;
	} while (((x == 0 && y == 0) && mapa[x][y].wumpus == true) && mapa[x][y].poco == true); // escolho novamente, caso o poço caia no espaço inicial do agente ou onde ja tenha poço.

	mapa[x][y].poco = true;
	mapa[x - 1][y].brisa = true;
	mapa[x + 1][y].brisa = true;
	mapa[x][y - 1].brisa = true;
	mapa[x][y + 1].brisa = true;

	if ((x==1 && y==2) || (x==2 && y==1))
		agentAdj = true;	// se o poço for adicionado num espaço adjacente ao espaço inicial do agente, ja que o agente nao pode ficar preso entre 2 poços.
	return 0;
}
