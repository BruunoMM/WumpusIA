#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "agente.h"

void add_agente(){
	agente.vida = 100;
	agente.municao = 5;
	agente.pos.i = 0;
	agente.pos.j = 0;
	agente.orientacao = E;
	agente.pontos = 0;
	mapa[0][0].visitado = true;
}

void grito(){
	printf("grawwwrrr! O agente acertou alguem. Avisar prolog\n");
}

void fimDeJogo(int status){
	if(status == WIN)
		printf("parabens voce venceu!\n");
	else
		printf("Que pena voce morreu..");
	scanf(" %*s");
	exit(1);
}

//checa por paredes.
int ehParede(Pos *pos){
	return pos->i <0 || pos->j<0 || pos->i >= MAP_SIZE || pos->j >= MAP_SIZE;
	
}

//move o agente para a posição indicada por pos
void agenteMove(Pos *pos){
	Cell *cell;
	int ataque;
	copyPos(&agente.pos, pos);
	cell = &mapa[pos->i][pos->j];
	printf("Agente se move para pos i:%d, j:%d\n", pos->i, pos->j);
	mapa[pos->i][pos->j].visitado = true;
	if(cell->wumpus){
		printf("Agente se move para Wumpus\n");
		fimDeJogo(LOSE);
		return;
	}	
	if(cell->inimigo.vida>0){
		ataque = cell->inimigo.ataque;
		agente.vida -= ataque;
		agente.pontos -= ataque;
		printf("Agente se move para Inimigo, %d de dano\n", ataque);
		if(agente.vida<=0)
			fimDeJogo(LOSE);
	}
	if(cell->poco){
		printf("Agente cai no poco\n");
		agente.pontos -= 1000;
	}
	avisaProlog();
}

//soma um aos componentes certos de pos, seguindo a orientacao do agente.
//Essa função ALTERA o argumento passado
void posNaFrente(Pos *pos){
	switch(agente.orientacao){
	case N:
		pos->i-=1;
		break;
	case E:
		pos->j+=1;
		break;
	case S:
		pos->i+=1;
		break;
	case W:
		pos->j-=1;
		break;
	}
}

//Se um inimigo for abatido, o cheiro dele precisa desaparacer dos arredores.
//Essa função checa se ao redor de pos ainda tem algum inimigo, se não houver mais nenhum, ela cessa o cheiro nessa posição.
//essa posição deve ser chamada nas quatro posições adjacentes a um inimigo recem abatido
void checaCheiro(Pos *pos){
	Pos posCheca;
	if(ehParede(pos)) return;
	copyPos(&posCheca, pos);
	mapa[pos->i][pos->j].cheiro = false;
	posCheca.i+=1;
	if(!ehParede(&posCheca) && isEnemy(posCheca)){
		mapa[pos->i][pos->j].cheiro = true;
		return;
	}
	copyPos(&posCheca, pos);
	posCheca.j+=1;
	if(!ehParede(&posCheca) && isEnemy(posCheca)){
		mapa[pos->i][pos->j].cheiro = true;
		return;
	}
	copyPos(&posCheca, pos);
	posCheca.i-=1;
	if(!ehParede(&posCheca) && isEnemy(posCheca)){
		mapa[pos->i][pos->j].cheiro = true;
		return;
	}
	copyPos(&posCheca, pos);
	posCheca.j-=1;
	if(!ehParede(&posCheca) && isEnemy(posCheca))
		mapa[pos->i][pos->j].cheiro = true;
}

//atira flecha a partir da posição do agente, verifica cada cell em linha reta, seguindo a aorientação do agente.
//para ao encontrar ou um inimigo, ou uma parede
void atirarFlecha(){
	Pos posCheca;
	Pos posFlecha;
	int danoFlecha;
	Cell *cell;
	if(agente.municao<=0){
		printf("agente nao possui flechas\n");
		return;
	}
	agente.municao-=1;
	copyPos(&posFlecha, &agente.pos);
	while(!ehParede(&posFlecha)){
		printf("Flecha em i:%d,j:%d.\n", posFlecha.i, posFlecha.j);
		cell = &mapa[posFlecha.i][posFlecha.j];
		if(cell->inimigo.vida>0){
			danoFlecha = (rand()%31)+20;
			cell->inimigo.vida -= danoFlecha;
			printf("inimigo sofre dano de %d pontos. Agora lhe restam %d\n", danoFlecha, cell->inimigo.vida);
			if(cell->inimigo.vida<=0){
				
				copyPos(&posCheca, &posFlecha);
				posCheca.i+=1;
				checaCheiro(&posCheca);

				copyPos(&posCheca, &posFlecha);
				posCheca.j+=1;
				checaCheiro(&posCheca);

				copyPos(&posCheca, &posFlecha);
				posCheca.i-=1;
				checaCheiro(&posCheca);

				copyPos(&posCheca, &posFlecha);
				posCheca.j-=1;
				checaCheiro(&posCheca);
			}
	
			grito();
			break;	
		}
		posNaFrente(&posFlecha);
	}
}

//Copia os valores de pos source para pos destiny
void copyPos(Pos *dest, Pos *src){
	dest->i = src->i;
	dest->j = src->j;
}

//pede uma ação para o prolog, retorna como enum
Acao pedirAcao(){
	char le[81];
	char s[][20] = {"MoverFrente","VirarDireita", "VirarEsquerda", "PegarObjeto", "AtirarFlecha", "Subir"};
	Acao acao;
	int i;
	for(i=0;i<6;i++)
		printf("deseja: %s ? Digite %d\n",s[i],i);
	scanf("%s", le);
	acao = (Acao	) atoi(le);
	printf("acao escolhida: %s\n", s[acao]);
	return acao;
}
void avisaProlog(){
	printf("avisando o prolog da situação atual. MENTIRA!! essa funcao nao faz nada\n");
}

//tenta executar a ação, retorna se foi possivel ou não.
//se não for possivel, o agente não executa a ação.
void executarAcao(Acao acao){
	Pos pos;
	agente.pontos -= 1;
	switch(acao){
		case MoverFrente:
			copyPos(&pos, &agente.pos);
			posNaFrente(&pos);
			if(ehParede(&pos)) {
				printf("Agente tentando se mover para uma parede\n");
				//TODO avisa prolog
				return;
			}
			agenteMove(&pos);
			break;
		case VirarDireita:
			printf("movendo agente para a direita de %d\n", agente.orientacao);
			agente.orientacao = (agente.orientacao+1)%4;
			printf("movendo agente para a direita para %d\n", agente.orientacao);
			break;
		case VirarEsquerda:
			printf("movendo agente para a esquerda de %d\n", agente.orientacao);
			agente.orientacao = (agente.orientacao-1)%4;
			printf("movendo agente para a esquerda para %d\n", agente.orientacao);
			break;
		case PegarObjeto:
			printf("agente tentando pegar ouro pontos:%d\n", agente.pontos);
			if(isGold(agente.pos)){				
				agente.pontos += 1000;
				mapa[agente.pos.i][agente.pos.j].brilho = false;
				printf("agente pegou ouro pontos:%d\n", agente.pontos);
			}
			else
				printf("tentativa de pegar o ouro em i:%d,j:%d. Nao ha ouro aqui\n", agente.pos.i, agente.pos.j);
			break;
		case AtirarFlecha:			
			printf("agente atirou flecha em i:%d,j:%d\n", agente.pos.i, agente.pos.j);
			atirarFlecha();
			break;
		case Subir:
			printf("agente tentando subir em i:%d,j:%d\n", agente.pos.i, agente.pos.j);
			if(agente.pos.i == 0 && agente.pos.j == 0){
				printf("agente subiu\n");
				fimDeJogo(WIN);
			}
			else
				printf("tentativa de subir em i:%d,j:%d. Nao ha escada aqui\n", agente.pos.i, agente.pos.j);
			
			break;
		default:
			printf("Acao invalida. Perdeu um ponto a toa\n");
			break;
	}
}
