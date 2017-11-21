#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SWI-Prolog.h>
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

/*
%escolher proxima acao
%I,J posicao na matriz
%O orientaçao
%X acao retornada
melhorAcao(I, J, O, X) :-
*/

//pede uma ação para o prolog, retorna como enum
Acao pedirAcao(){
	char s[][20] = {"MoverFrente","VirarDireita", "VirarEsquerda", "PegarObjeto", "AtirarFlecha", "Subir"};
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

	

	p = PL_predicate("melhorAcao", 4, "user");
	t = PL_new_term_refs(4);
	sprintf(arg, "%d", agente.pos.i);
	PL_put_atom_chars(t, arg);
	sprintf(arg, "%d", agente.pos.j);
	PL_put_atom_chars(t+1, arg);
	sprintf(arg, "%d", agente.orientacao);
	PL_put_atom_chars(t+2, arg);

	qid = PL_open_query(NULL, PL_Q_NORMAL, p, t);
	rval = PL_next_solution(qid);
	termoAcao = t+3;
	rval = PL_get_integer(termoAcao, &returnInteger);
	Sprintf("valorInt:%d\n", returnInteger);
	acao = (Acao) returnInteger;
	/*
	do{
		for(i=0;i<4;i++){
			printf("i:%d\n", i);
			printf("rvalA:%d\n", rval);
			termoAcao = t+i;
			if(PL_is_atom(termoAcao)){
				puts("atom---\n");
				rval = PL_get_atom_chars(termoAcao, &returnString);		
			}
			else if(PL_is_string(termoAcao)){
				puts("string---\n");
				rval = PL_get_string_chars(termoAcao, &returnString, &tam);				
			}
			else if(PL_is_variable(termoAcao)){
				puts("variavel---\n");
				rval = PL_get_chars(termoAcao, &returnString, CVT_ALL);
			}
			else if(PL_is_integer(termoAcao)){
				puts("inteiro---\n");
				rval = PL_get_integer(termoAcao, &returnInteger);
				Sprintf("valorInt:%d\n", returnInteger);
			}
			else{
				Sprintf("tipo: %d\n", PL_term_type(termoAcao));
			}
			printf("rvalA:%d\n", rval);
			Sprintf("valor:%s\n", returnString);
		}
	}while(PL_next_solution(qid));
/*
	if(PL_is_variable(t+3)){
		printf("variavel\n");
		rval = PL_get_chars(termoAcao, &returnString, CVT_ALL);
		
		//rval = PL_get_atom_chars(termoAcao, &returnString);
		Sprintf("string:%s\n", returnString);
		Sprintf("rval:%d", rval);
	}else Sprintf("tipo: %d\n", PL_term_type(t+3));

*/
	Sprintf("A melhor solucao lida do prolog e: %d\n", returnInteger);
	PL_cut_query(qid);
	
	//acao = (Acao) 0;

	/* versão jogavel------------------------
	char le[81];
	int i;
	for(i=0;i<6;i++)
		printf("deseja: %s ? Digite %d\n",s[i],i);
	scanf("%s", le);
	acao = (Acao	) atoi(le);
	*/
	// versão AI
	
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
