#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "agente.h"

int main(){
	int i;
	Acao acao;
	int seed = time(NULL);
	srand(seed);
	printf("seed usado: %d\n", seed);
	gera_mapa();
	add_agente();
	for(i=0;i<20;i++){
		acao = pedirAcao();
		executarAcao(acao);
	}
	printf("fim\n");
	return 0;
}
