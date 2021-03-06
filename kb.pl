:- dynamic cheiro/2.
:- dynamic visitado/2.
:- dynamic percepcao/7.
:- dynamic ehParede/2.

% true se houver chance de um inimigo estar em i,j. Caso retorne false, pergunte se o lugar foi visitado, e qual a percepçao de la
assumeEnemy(I,J) :-
	\+ visitado(I,J), % se não tiver visitado i,j. Pois se ja tiver visitado ja sei o que tem em i,j
	cheiro(I2, J2),
	adjacente(I, J, I2, J2).
	
assumePoco(I,J) :-
	\+ visitado(I,J), % se não tiver visitado i,j. Pois se ja tiver visitado ja sei o que tem em i,j
	brisa(I2, J2),
	adjacente(I, J, I2, J2).

poco(I,J) :-
	visitado(I,J),
	percepcao(I,J,Poco,_,_,_,_),
	Poco = 1.

brisa(I,J) :-
	visitado(I,J),
	percepcao(I,J,_,Brisa,_,_,_),
	Brisa = 1.

brilho(I,J) :-
	visitado(I,J),
	percepcao(I,J,_,_,Brilho,_,_),
	Brilho = 1.

cheiro(I,J) :-
	visitado(I,J),
	percepcao(I,J,_,_,_,Cheiro,_),
	Cheiro = 1.

enemy(I,J) :-
	visitado(I,J),
	percepcao(I,J,_,_,_,_,Enemy),
	Enemy = 1.

% poco, brisa, brilho, cheiro, wumpus - note que estou chamando de 'wumpus', mas na verdade é qualquer inimigo
move(I, J, Poco, Brisa, Brilho, Cheiro, Wumpus) :-
	format("move li do c i:~p, j:~p, [~p,~p,~p,~p,~p]~n", [I,J, Poco, Brisa, Brilho, Cheiro, Wumpus]),
	\+ visitado(I,J) -> (
		assert(visitado(I,J)),
		assert(percepcao(I, J, Poco, Brisa, Brilho, Cheiro, Wumpus))
	).

%chamada pelo c para adicionar uma posicao como parede
addParede(I,J) :-
	format("parede li do c i:~p, j:~p~n", [I,J]),
	\+ ehParede(I,J) ->
		assert(ehParede(I,J)).

%chamada pelo c para avisar que em uma determinada posição não existe mais ouro.
removeOuro(I,J):-
	format("removeOuro li do c i:~p, j:~p~n", [I,J]),
	percepcao(I, J, Poco, Brisa, _, Cheiro, Wumpus),
	retractall(percepcao(I, J, _,_,_,_,_)),
	assert(percepcao(I, J, Poco, Brisa, 0, Cheiro, Wumpus)).
	


%diz se os numeros são sucessores um do outro, ajuda na função adjacente logo abaixo
sucAnt(A, B) :-
	A is B+1;
	A is B-1.

%diz se a coordenada I,J é adjacente a coordenada I2,J2
adjacente(I,J,I2,J2) :-
	I = I2, sucAnt(J, J2);
	J = J2, sucAnt(I, I2).

%visitado(3,4).
%visitado(3,5).
%visitado(3,6).
%percepcao(3,4,0,0,1,0,0).
%percepcao(3,5,0,1,0,0,0).
%percepcao(3,6,0,1,1,0,0).

/*
alguns algoritmos para determinar melhor acao
se brilho -> pegar
se nenhum perigo avancar
se encontrou parede virar para a direita
se em perigo voltar para local conhecido. Com "em perigo", quero dizer do lado de uma brisa ou inimigo.
caso nao esteja em perigo ir para lugar desconhecido. Desbravando!

Se certeza de inimigo, flecha nele.

Contar distancia desde a origem (distancia em I e J mesmo), se tiver duas ou tres barras de ouro retornar para ela. Isso é, tentar diminuir a distancia.
Se tiver pouca vida retornar para ela.
A principio, pensei em fazer variaveis que contariam a distancia ate a origem, como se o agente soubesse de que direção ele veio, e que distancia percorreu. Mas percebi que essas variaveis seriam redundantes, ja que o agente começa no 0,0 e a distancia dele, é a propria coordenada.
Mesmo não sabendo onde começou no mapa, o agente assume que começou em 0,0. Para ele essa é a origem, e a posição dele em I,J é a mesma distancia da origem. Isso ajuda a simplificar as coisas. E não quebra as regras. É so uma mudança de referencial

*/

%indica se a posição I, J é perigosa
emPerigo(I,J) :-
	brisa(I,J);
	cheiro(I,J).

%retorna em I2, J2 a localização exatamente a frente de I,J seguind a orientacao O
emFrente(I,J,O,I2,J2) :- 
	O = 0 -> format("O = 0~n",[]), J2 = J, I2 is I-1;
	O = 1 -> format("O = 1~n",[]), I2 = I, J2 is J+1;
	O = 2 -> format("O = 2~n",[]), J2 = J, I2 is I+1;
	O = 3 -> format("O = 3~n",[]), I2 = I, J2 is J-1.

/*
MoverFrente,
VirarDireita,
VirarEsquerda,
PegarObjeto,
AtirarFlecha,
Subir
*/

%escolher proxima acao
%I,J posicao na matriz
%O orientaçao
%X acao retornada
/*
melhorAcao(I,J,O,X):-
	format("melhorAcaoC li do c:~p,~p,~p~n", [I,J,O]),
	J=0 -> format("decisaoA~n"), X = 2, true;
	(J=1, O=1 )-> format("decisaoB~n"), X = 1;
	(I=0, J=1, O=2 )-> format("decisaoC~n"), X = 3;
	(I=1, O=2) -> format("decisaoD~n"), X = 2;
	I=1, O=1 -> format("decisaoE~n"), X = 2;
	X = 3.
*/	
% retorna se em volta se todas ja foram visitadas
olhaEmVolta(I,J) :- adjacente(I, J, A, B), visitado(A, B).
% retorna se em volta nenhuma casa foi visitada
olhaEmVoltaNao(I,J) :- adjacente(I, J, A, B), \+ visitado(A, B).


melhorAcao(I, J, O, X) :-
	format("melhorAcaoA li do c i:~p j:~p O:~p~n", [I, J, O]),
	emFrente(I,J,O,A,B), format("melhorAcaoB I:~p J:~p~n", [A,B]),
	brilho(I,J) -> format("melhor acao pegar", []), X=3;
	emFrente(I,J,O,A,B), ehParede(A,B) -> % caso logo a frente esteja uma parede, virar para a direita
		format("melhor acao virar direita", []), X=1;
	%if
	emPerigo(I,J) -> (  % caso em perigo, ir para lugar conhecido, a nao ser que todos tenham sido visitados
		format("em perigo", []);
		%if
		emFrente(I,J,O,A,B), visitado(A,B) ->
			format("melhor acao avancar", []), X=1;
		%else if
		olhaEmVolta(I,J) -> format("melhor virar direita", []), X=1;
		%else if
		olhaEmVoltaNao(I,J) ->
			format("melhor acao ir frente, pois nao tem para onde ir", []),
			X=1;
		);
	%else
	\+ emPerigo(I,J) -> (  % caso não esteja em perigo, ir para lugar desconhecido, a nao ser que todos sejam conhecidos
		format("nao esta em perigo", []);
		%if
		emFrente(I,J,O,A,B), \+ visitado(A,B) ->
			format("melhor acao avancar", []), X=1;
		%else if
		olhaEmVoltaNao(I,J) -> format("melhor virar direita", []), X=1;
		%else if
		olhaEmVolta(I,J) ->
			format("melhor acao ir frente, pois nao tem para onde ir", []),
			X=1;
		);
	/*
	emFrente(I,J,O,A,B), \+ visitado(A,B), emPerigo(I,J)  -> %se na frente não é conhecido, virar a direita.
		format("melhor acao virar direita", []), X=1;
	emFrente(I,J,O,A,B), \+ emPerigo(I,J), visitado(A,B) -> % nao esta em perigo, melhor acao avancar para lugar desconhecido.
		format("melhor acao virar direita", []), X=1; % vira a direita se a frente ja for conhecido.
	emFrente(I,J,O,A,B), \+ emPerigo(I,J), \+ visitado(A,B) -> % nao esta em perigo, prioridade é desbravar o desconhecido
		format("melhor acao avancar "), X=0;
	*/
	format("do nothing", []), X=0.
