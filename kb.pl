:- dynamic cheiro/2.
:- dynamic visitado/2.
:- dynamic percepcao/7.
:- dynamic agentePos/2.

start :-
	format("testando start", []).

% true se houver chance de um inimigo estar em i,j. Caso retorne false, pergunte se o lugar foi visitado, e qual a percepçao de la
assumeEnemy(I,J) :-
	\+ visitado(I,J), % se não ter visitado i,j. Pois se ja tiver visitado ja sei o que tem em i,j
	cheiro(I2, J2),
	adjacente(I, J, I2, J2).
	
assumePoco(I,J) :-
	\+ visitado(I,J), % se não ter visitado i,j. Pois se ja tiver visitado ja sei o que tem em i,j
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
	retract(agentePos(_,_)),
	assert(agentePos(I,J)),
	assert(visitado(I,J)),
	assert(percepcao(I, J, Poco, Brisa, Brilho, Cheiro, Wumpus)).


%diz se os numeros são sucessores um do outro, ajuda na função adjacente logo abaixo
sucAnt(A, B) :-
	A is B+1;
	A is B-1.

%diz se a coordenada I,J é adjacente a coordenada I2,J2
adjacente(I,J,I2,J2) :-
	I = I2, sucAnt(J, J2);
	J = J2, sucAnt(I, I2).

visitado(3,4).
visitado(3,5).
visitado(3,6).
percepcao(3,5,0,0,1,0,0).
percepcao(3,5,0,1,0,0,0).
percepcao(3,6,0,1,1,0,0).

%transformar isso em escolher proxima acao
teste(I, J, X) :-
	brilho(I,J) -> (format("melhor acao pegar", []), X=3);
	brisa(I,J) -> format("melhor acao voltar", [], X=2).
