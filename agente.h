#include <stdbool.h>

#define WIN 1
#define LOSE 0

typedef enum orientacao{
	N,
	E,
	S,
	W
} Orientacao;

typedef struct agente{
	int vida;
	int municao;
	Pos pos;
	Orientacao orientacao;
	int pontos;
} Agente;
	
typedef enum acao{
	MoverFrente,
	VirarDireita,
	VirarEsquerda,
	PegarObjeto,
	AtirarFlecha,
	Subir
} Acao;

Agente agente;

//deve ser chamada no inicio do jogo
void add_agente();

void grito();

void fimDeJogo(int status);

//checa por paredes
int ehParede(Pos *pos);
//move o agente para a posição indicada por pos
void agenteMove(Pos *pos);

//soma um aos componentes certos de pos, seguindo a orientacao do agente.
//Essa função ALTERA o argumento passado
void posNaFrente(Pos *pos);

//atira flecha a partir da posição do agente, verifica cada cell em linha reta, seguindo a aorientação do agente.
//para ao encontrar ou um inimigo, ou uma parede
void atirarFlecha();

//Copia os valores de pos source para pos destiny
void copyPos(Pos *dest, Pos *src);
//TODO pede uma ação para o prolog, retorna como enum
Acao pedirAcao();
//TODO conta para o prolog qual a situação do quadrado atual. chamada quando o agente se move para um novo quadrado
void avisaProlog();

//tenta executar a ação, retorna se foi possivel ou não.
//se não for possivel, o agente não executa a ação.
void executarAcao(Acao acao);
