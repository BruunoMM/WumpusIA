


#include <stdbool.h>

#define ROW


struct inimigo{
	int vida;
	int ataque;
}

struct cell{
	Inimigo *inimigo;
	bool wumpus;
	bool buraco;
	bool cheiro;
	bool brilho;
	bool vento;
}

typedef struct pos{
	int x;
	int y;
} Pos;

enum orientacao{
	N,
	E,
	S,
	W
} Orientacao;
	
enum acao{
	MoverFrente; 
	VirarDireita,
	VirarEsquerda,
	PegarObjeto,
	AtirarFlecha,
	Subir
} Acao;

//checa por paredes.
int ehParede(Pos &pos){
	return pos->x <0 || pos->y<0 || pos->x > ROW || pos->y> ROW;
	
}

//soma um aos componentes certos de pos, seguindo a orientacao do agente.
//Essa função ALTERA o argumento passado
void posNaFrente(Pos *pos){
	switch(orientacaoDoAgente()){
	case N:
		pos->y-=1;
		break;
	case E:
		pos->x+=1;
		break;
	case S:
		pos->y+=1;
		break;
	case W:
		pos->x-=1;
		break;
}

void copyPos(Pos *dest, Pos *src){
	dest->x = src->x;
	dest-> = src->y;
}

//pede uma ação para o prolog, retorna como enum
Acao pedirAcao(){};

//tenta executar a ação, retorna se foi possivel ou não.
//se não for possivel, o agente não executa a ação.
int executarAcao(Acao acao){
	Pos pos;
	switch(acao){
		case MoverFrente:
			copyPos(pos, posDoAgente());
			posNaFrente(&pos);
			if(ehParede(pos)) return PAREDE;
			agenteMove(pos);
			break;
		case VirarDireita:
			break;
		case VirarEsquerda:
			break;
		case PegarObjeto:
			break;
		case AtirarFlecha:
			break;
		case Subir:
			break;
		default:
			printf("Acao invalida");
			return -1;
			break;
	}
};
	 


int main(){
	cell m[ROW][ROW];
	int i, e;
	int cell;
	Inimigo emy1;
	Inimigo emy2;

	for(i=0;i<INIMIGOS;i++){
		x = rand();
		y = rand();
		cell[y][x].inimigo = rand()?&emy1:&emy2;
	}


	return 0;
}
