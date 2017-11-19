#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "agente.h"

int main(){
	Acao acao;
	int seed = time(NULL);
	srand(seed);
	printf("seed usado: %d\n", seed);
	gera_mapa();
	add_agente();
	while(1){
		printf("Pontos: %d\n", agente.pontos);
		desenhaMapa();
		acao = pedirAcao();
		executarAcao(acao);
		scanf("%*s");
	}
	printf("fim\n");
	return 0;
}
