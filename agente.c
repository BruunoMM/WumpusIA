#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SWI-Prolog.h>
#include "map.h"
#include "agente.h"

//coloca agente no mapa, chamada no inicio do jogo
void add_agente(){
	agente.vida = 100;
	agente.municao = 5;
	agente.pos.i = 0;
	agente.pos.j = 0;
	agente.orientacao = E;
	agente.pontos = 0;
	mapa[0][0].visitado = true;
	avisaProlog(agente.pos);
}

//avisa ao prolog que um inimigo foi atingido por uma flecha
void grito(){
	printf("grawwwrrr! O agente acertou alguem.\n");
}

void fimDeJogo(int status){
	if(status == WIN)
		printf("\n\nParabens voce venceu!\n");
	else
		printf("\n\nQue pena voce morreu..");
	printf("Pontuacao alcançada: %d\n", agente.pontos);
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
		fimDeJogo(LOSE);
	}
	avisaProlog(*pos);
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

//Se um inimigo for abatido, o cheiro dele precisa desaparecer dos arredores.
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

//atira flecha a partir da posição do agente, verifica cada cell em linha reta, seguindo a orientação do agente.
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

/*
%escolher proxima acao
%I,J posicao na matriz
%O orientaçao
%X acao retornada
melhorAcao(I, J, O, X) :-
*/

//pede uma ação para o prolog, retorna como enum
Acao pedirAcao(FILE *log){
	char s[][20] = {"MoverFrente","VirarDireita", "VirarEsquerda", "PegarObjeto", "AtirarFlecha", "Subir"};
	char orientacaoS[] = "NESW";
	char *returnString = NULL;
	char arg[81];
	Acao acao;
	predicate_t p;
	term_t t;
	term_t termoAcao;
	int rval;
	int qid;
	size_t tam;
	int i;
	int returnInteger=0;

	Cell *c = &mapa[agente.pos.i][agente.pos.j];

	// versão AI	

	p = PL_predicate("melhorAcao", 4, "user");
	t = PL_new_term_refs(4);
	rval = PL_put_integer(t, agente.pos.i);
	rval = PL_put_integer(t+1, agente.pos.j);
	rval = PL_put_integer(t+2, agente.orientacao);

	qid = PL_open_query(NULL, PL_Q_NORMAL, p, t);
	rval = PL_next_solution(qid);
	termoAcao = t+3;
	rval = PL_get_integer(termoAcao, &returnInteger);
	Sprintf("valorInt:%d\n", returnInteger);
	acao = (Acao) returnInteger;
	Sprintf("A melhor solucao lida do prolog e: %d\n", returnInteger);
	PL_cut_query(qid);

	//gravando em log
	fprintf(log, "i:%d j:%d orientacao:%c, %s poco, %s brisa, %s brilho, %s cheiro, %s wumpus -> %s\n",
		agente.pos.i,
		agente.pos.j,
		orientacaoS[agente.orientacao],
		c->poco?"":"sem",
		c->brisa?"":"sem",
		c->brilho?"":"sem",
		c->cheiro?"":"sem",
		c->wumpus?"":"sem",
		s[acao]
	);
	fflush(log);
	
	/* versão jogavel------------------------
	char le[81];
	int i;
	for(i=0;i<6;i++)
		printf("deseja: %s ? Digite %d\n",s[i],i);
	scanf("%s", le);
	acao = (Acao	) atoi(le);
	*/
	
	printf("acao escolhida: %s\n", s[acao]);
	return acao;
}

//envia para o prolog a percepção na posição indicada
void avisaProlog(Pos pos){
	predicate_t p;
	term_t t0;
	term_t t;
	int qid;
	int rval;
	Cell *cell;

	printf("avisando o prolog da situação atual.\n");
	cell = &mapa[pos.i][pos.j];

	t0 = PL_new_term_refs(7);
	p = PL_predicate("move",7,"user");
	t = t0;
	rval = PL_put_integer(t++, pos.i);
	Sprintf("%d - ", t0);

	rval = PL_put_integer(t++, pos.j);
	Sprintf("%d - ", t0+1);

	rval = PL_put_integer(t++, cell->poco?1:0);
	Sprintf("%d - ", t0+2);

	rval = PL_put_integer(t++, cell->brisa?1:0);
	Sprintf("%d - ", t0+3);

	rval = PL_put_integer(t++, cell->brilho?1:0);
	Sprintf("%d - ", t0+4);

	rval = PL_put_integer(t++, cell->cheiro?1:0);
	Sprintf("%d - ", t0+5);

	rval = PL_put_integer(t, cell->wumpus?1:0);
	Sprintf("%d\n", t0+6);

	qid = PL_open_query(NULL, PL_Q_NORMAL, p, t0);
	rval = PL_next_solution(qid);
	PL_cut_query(qid);
}

void chamaProlog(char func[], Pos pos){
	predicate_t p;
	term_t t;
	int qid;
	int rval;
	t = PL_new_term_refs(2);
	p = PL_predicate(func,2,"user");
	rval = PL_put_integer(t, pos.i);
	rval = PL_put_integer(t+1, pos.j);
	qid = PL_open_query(NULL, PL_Q_NORMAL, p, t);
	rval = PL_next_solution(qid);
	PL_cut_query(qid);
}

void addParede(Pos pos){
	chamaProlog("addParede", pos);
}

void removeOuro(Pos pos){
	chamaProlog("removeOuro", pos);
}

//tenta executar a ação.
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
				addParede(pos);
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
				removeOuro(agente.pos);
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
