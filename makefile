CC = gcc
CFLAGS = -Wall -Wextra

# Detecta se é Windows para adicionar .exe, senão deixa vazio (Linux)
ifdef OS
   EXT = .exe
else
   EXT =
endif

all: folders mapa extra4

# 1. Cria as pastas necessárias se não existirem (-p evita erro se já existirem)
folders:
	mkdir -p saida
	mkdir -p Extra/output

# 2. Compila o programa principal na pasta 'saida'
mapa: src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c Extra/Extra3.c
	$(CC) $(CFLAGS) src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c Extra/Extra3.c -o saida/mapa$(EXT)

# 3. Compila o Extra4 e coloca onde o main.c procura (Extra/output)
extra4: Extra/Extra4.c
	$(CC) $(CFLAGS) Extra/Extra4.c -o Extra/output/Extra4$(EXT)

# Limpa os executáveis nos dois locais
clean:
	rm -f saida/mapa$(EXT) Extra/output/Extra4$(EXT)

# Roda o programa principal
run:
	./saida/mapa$(EXT)