all:
	gcc src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c -o ./saida/mapa

	gcc Extra/Extra4.c -o ./saida/extra4

clear:
	rm -f ./saida/mapa ./saida/extra4

run:
	./saida/mapa

run-extra4:
	./saida/extra4
