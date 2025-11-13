# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra

# --- DETECÇÃO DE SISTEMA OPERACIONAL ---
ifeq ($(OS),Windows_NT)
    # Configurações para WINDOWS
    EXT = .exe
    RM = del /Q
    
    # Função para criar pasta no Windows (verifica se existe antes)
    # O 'subst' troca / por \ para o CMD entender o caminho
    MKDIR_P = if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1))
    
    # Fix para caminhos nos comandos de limpeza e execução
    FIX_PATH = $(subst /,\,$(1))
else
    # Configurações para LINUX / UNIX
    EXT =
    RM = rm -f
    
    # Comando simples do Linux
    MKDIR_P = mkdir -p $(1)
    
    # No Linux o caminho já usa barras normais
    FIX_PATH = $(1)
endif

# --- ARQUIVOS E DIRETÓRIOS ---
SRC_MAIN = src/main.c src/mapa.c src/solucionador.c Extra/Extra1.c Extra/Extra2.c Extra/Extra3.c
SRC_EXTRA4 = Extra/Extra4.c

OUT_DIR = saida
EXTRA_OUT_DIR = Extra/output

TARGET_MAIN = $(OUT_DIR)/mapa$(EXT)
TARGET_EXTRA4 = $(EXTRA_OUT_DIR)/Extra4$(EXT)

# --- REGRAS DE COMPILAÇÃO ---

all: folders main extra4

# Regra para criar as pastas (Usa a função definida acima dependendo do OS)
folders:
	@$(call MKDIR_P,$(OUT_DIR))
	@$(call MKDIR_P,$(EXTRA_OUT_DIR))

# Compila o programa principal
main: $(SRC_MAIN)
	$(CC) $(CFLAGS) $(SRC_MAIN) -o $(call FIX_PATH,$(TARGET_MAIN))

# Compila o gerador Extra4
extra4: $(SRC_EXTRA4)
	$(CC) $(CFLAGS) $(SRC_EXTRA4) -o $(call FIX_PATH,$(TARGET_EXTRA4))

# Limpa os arquivos gerados
clean:
	$(RM) $(call FIX_PATH,$(TARGET_MAIN))
	$(RM) $(call FIX_PATH,$(TARGET_EXTRA4))

# Roda o programa principal
run:
	$(call FIX_PATH,$(TARGET_MAIN))