#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "agente.h"

void add_agente(){
	agente.vida = 100;
	agente.municao = 5;
	agente.i = 0;
	agente.j = 0;
	agente.Orientacao = E;
	agente.pontos = 0;
}

void grito(){
	printf("grawwwrrr! A agente acertou alguem. Avisar prolog\n");
}

void fimDeJogo(int status){
	if(status == WIN){
		printf("parabens voce venceu!\n");
	else
		printf("Que pena voce morreu..");
	scanf(" %*s");
	exit(1);
}

//checa por paredes.
int ehParede(Pos &pos){
	return pos->i <0 || pos->j<0 || pos->i > MAP_SIZE || pos->j> MAP_SIZE;
	
}

//move o agente para a posição indicada por pos
void agenteMove(Pos *pos){
	Cell *cell;
	int ataque;
	copyPos(&agente.pos, pos);
	cell = &mapa[pos->i][pos->j];
	#ifdef _DEBUG
		printf("Agente se move para pos i:%d, j:%d\n", pos->i, pos->j);
	#endif
	if(cell->wumpus){
		#ifdef _DEBUG
			printf("Agente se move para Wumpus\n");
		#endif
		fimDeJogo(LOSE)
		return -1;
	}	
	if(cell->inimigo){
		ataque = cell->inimigo->ataque;
		agente.vida -= ataque;
		agente.pontos -= ataque;
		#ifdef _DEBUG
			printf("Agente se move para Inimigo, %d de dano\n", ataque);
		#endif
	}
	if(poco){
		#ifdef _DEBUG
			printf("Agente cai no poco\n");
		#endif
		agente.pontos -= 1000;
	}
	avisaProlog();
}

//soma um aos componentes certos de pos, seguindo a orientacao do agente.
//Essa função ALTERA o argumento passado
void posNaFrente(Pos *pos){
	switch(orientacaoDoAgente()){
	case N:
		pos->i-=1;
		break;
	case E:
		pos->j-=1;
		break;
	case S:
		pos->i+=1;
		break;
	case W:
		pos->j+=1;
		break;
}

//atira flecha a partir da posição do agente, verifica cada cell em linha reta, seguindo a aorientação do agente.
//para ao encontrar ou um inimigo, ou uma parede
void atirarFlecha(){
	Pos posFlecha;
	Cell *cell;
	if(agente->municao<=0){
		printf("agente nao possui flechas\n");
		return;
	}
	agente->municao-=1;
	copyPos(&posFlecha, &agente->pos);
	while(!ehParede(&posFlecha)){
		cell = mapa[posFlecha.i][posFlecha.j]
		if(cell->inimigo){
			cell->inimigo->vida -= (rand()%31)+20;
			grito();
			break;	
		}
	}
}

//Copia os valores de pos source para pos destiny
void copyPos(Pos *dest, Pos *src){
	dest->i = src->i;
	dest->j = src->j;
}

//pede uma ação para o prolog, retorna como enum
Acao pedirAcao(){
	char s[][20] = {"MoverFrente","VirarDireita", "VirarEsquerda", "PegarObjeto", "AtirarFlecha", "Subir"};
	return (Acao) rand()%6;
}
void avisaProlog(){
	printf("avisando o prolog da situação atual. MENTIRA!! essa funcao nao faz nada\n");
}

//tenta executar a ação, retorna se foi possivel ou não.
//se não for possivel, o agente não executa a ação.
int executarAcao(Acao acao){
	Pos pos;
	agente.pontuacao -= 1;
	switch(acao){
		case MoverFrente:
			copyPos(pos, posDoAgente());
			posNaFrente(&pos);
			if(ehParede(pos)) return PAREDE;
			agenteMove(pos);
			break;
		case VirarDireita:
			agente.orientacao = (agente.orientacao+1)%4;
			break;
		case VirarEsquerda:
			agente.orientacao = (agente.orientacao-1)%4;
			break;
		case PegarObjeto:
			if(isGold(agente.pos)){
				agente.pontuacao += 1000;
			}
			else
				printf("tentativa de pegar o ouro em i:%d,j:%d. Nao ha ouro aqui\n", agente.pos.i, agente.pos.j);
			break;
		case AtirarFlecha:
			atirarFlecha();
			break;
		case Subir:
			if(agente.pos.i == 0 && agente.pos.j == 0){
				fimDeJogo(WIN);
			}
			else
				printf("tentativa de subir em i:%d,j:%d. Nao ha escada aqui\n", agente.pos.i, agente.pos.j);
			break;
		default:
			printf("Acao invalida");
			return -1;
			break;
	}
};