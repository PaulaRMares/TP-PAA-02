CC = gcc
CFLAGS = -Wall -Wextra

# Caminhos de saída com barra invertida para o Windows
OUT_DIR = saida
EXTRA_OUT_DIR = Extra\output

# Alvo padrao
all: folders mapa extra4

# 1. Cria as pastas se não existirem (Sintaxe Windows)
folders:
	if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	if not exist $(EXTRA_OUT_DIR) mkdir $(EXTRA_OUT_DIR)

# 2. Compila o programa principal (Incluindo o novo Extra3.c)
mapa: src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c Extra/Extra3.c
	$(CC) $(CFLAGS) src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c Extra/Extra3.c -o $(OUT_DIR)\mapa.exe

# 3. Compila o Gerador Extra4 no local correto
extra4: Extra/Extra4.c
	$(CC) $(CFLAGS) Extra/Extra4.c -o $(EXTRA_OUT_DIR)\Extra4.exe

# Limpa os arquivos (Sintaxe Windows)
clean:
	if exist $(OUT_DIR)\mapa.exe del /Q $(OUT_DIR)\mapa.exe
	if exist $(EXTRA_OUT_DIR)\Extra4.exe del /Q $(EXTRA_OUT_DIR)\Extra4.exe

# Roda o programa
run:
	$(OUT_DIR)\mapa.exe