#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SWI-Prolog.h>
#include "map.h"
#include "agente.h"

int main(int argc, char *argv[]){
	char *args[2];
	Acao acao;
	int seed = time(NULL);
	srand(seed);
	printf("seed usado: %d\n", seed);
	
	args[0] = argv[0];
	args[1] = NULL;
	if(!PL_initialise(argc, args)){
		printf("falha ao inicializar prolog\n");
		PL_halt(1);
	}
	 
	gera_mapa();
	add_agente();
	while(1){
		printf("Pontos: %d\n", agente.pontos);
		desenhaMapa();
		scanf("%*s");
		acao = pedirAcao();
		executarAcao(acao);
	}
	printf("fim\n");
	return 0;
}
