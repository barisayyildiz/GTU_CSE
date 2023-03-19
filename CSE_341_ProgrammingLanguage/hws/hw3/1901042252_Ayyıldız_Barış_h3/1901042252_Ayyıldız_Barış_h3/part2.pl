flight(canakkale, erzincan, 6).
flight(erzincan, antalya, 3).
flight(antalya, izmir, 2).
flight(antalya, diyarbakir, 4).
flight(izmir, istanbul, 2).
flight(izmir, ankara, 6).
flight(istanbul, ankara, 1).
flight(istanbul, rize, 4).
flight(ankara, rize, 5).
flight(ankara, van, 4).
flight(ankara, diyarbakir, 8).
flight(van, gaziantep, 3).

% returns true if there is a direct flight from X to Y
direct_flight(X,Y,C) :-
	flight(X,Y,C).

% returns true if there is a direct flight from Y to X
direct_flight(X,Y,C) :-
	flight(Y,X,C).

route(X,Y,_) :-
	search_route(X,Y,0,[]).

search_route(X,Y,Cin,List) :-
	direct_flight(X,Z,C),
	not(member(Z, List)),
	format('X=~w, C=~d;~n', [Z,Cin+C]),
	search_route(Z,Y,Cin+C,[X, Z |List]).

