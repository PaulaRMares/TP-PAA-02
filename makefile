all:
	gcc src/main.c src/mapa.c src/solucionador.c -o ./saida/mapa

clear:
	rm -f ./saida/mapa

run:
	./saida/mapa