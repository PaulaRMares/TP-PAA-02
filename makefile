all:
	gcc src/main.c src/mapa.c -o ./saida/mapa
clear:
	rm -f ./saida/mapa
run:
	./saida/mapa