#include "../headers/mapa.h"

static bool analisa_celula_token(const char tok[4], Celula *c, char *err, size_t errsz){
    if(strcmp(tok, "000") == 0){
        c->type = CELULA_VAZIA; 
        c->value = 0; 
        return true;
    }
    if(strcmp(tok, "***") == 0){
        c->type = CELULA_BLOQUEADA;
        c->value = 0; 
        return true;
    }
    if(strcmp(tok, "AAA") == 0){
        c->type = CELULA_ANCORA; 
        c->value = 0; 
        return true;
    }
    if(isdigit((unsigned char)tok[0]) &&
    isdigit((unsigned char)tok[1]) &&
    isdigit((unsigned char)tok[2]) &&
    tok[3] == '\0'){
        int v = (tok[0]-'0')*100 + (tok[1]-'0')*10 + (tok[2]-'0');
        if(v >= 1 && v <= 999){
            c->type = CELULA_MONSTRO; 
            c->value = v; 
            return true;
        }
    }
    snprintf(err, errsz, "Token de celula invalido: '%s'", tok);
    return false;
}

static bool ler_token(FILE *fp, char buf[4]){
    int c;
    do{
        c = fgetc(fp);
        if (c == EOF) return false;
    }while (isspace(c));

    buf[0] = (char)c;
    for(int i = 1; i < 3; i++){
        c = fgetc(fp);
        if(c == EOF){
            return false;
        }
        buf[i] = (char)c;
    }
    buf[3] = '\0';
    return true;
}

bool mapa_carregar(const char *path, Mapa *out, char *errbuf, size_t errbuf_sz){
    if(!out){
        snprintf(errbuf, errbuf_sz, "Parametro 'out' nulo");
        return false;
    }

    memset(out, 0, sizeof(*out));
    FILE *fp = fopen(path, "r");
    if(!fp){
        snprintf(errbuf, errbuf_sz, "Nao foi possível abrir '%s'", path);
        return false;
    }

    // primeira linha: h w F D N
    if(fscanf(fp, "%d %d %d %d %d", &out->h, &out->w, &out->F, &out->D, &out->N) != 5){
        snprintf(errbuf, errbuf_sz, "Primeira linha invalida. Esperado: h w F D N");
        fclose(fp);
        return false;
    }
    if(out->h <= 0 || out->w <= 0){
        snprintf(errbuf, errbuf_sz, "Dimensões invalidas: h=%d w=%d", out->h, out->w);
        fclose(fp);
        return false;
    }

    size_t total = (size_t)out->h * (size_t)out->w;
    out->presente = (Celula*)calloc(total, sizeof(Celula));
    out->passado    = (Celula*)calloc(total, sizeof(Celula));
    if(!out->presente || !out->passado){
        snprintf(errbuf, errbuf_sz, "Falha de alocacao (h*w=%lu)", (unsigned long)total);
        fclose(fp);
        return false;
    }

    // ler h*w tokens do mapa do presente
    char tok[4], perr[128];
    for(int i = 0; i < out->h; i++){
        for(int j = 0; j < out->w; j++){
            if(!ler_token(fp, tok)){
                snprintf(errbuf, errbuf_sz, "EOF ao ler mapa do presente em (%d,%d)", i, j);
                fclose(fp);
                return false;
            }
            if(!analisa_celula_token(tok, cell_at(out->presente, out->h, out->w, i, j), perr, sizeof(perr))){
                snprintf(errbuf, errbuf_sz, "%s (presente em %d,%d)", perr, i, j);
                fclose(fp);
                return false;
            }
        }
    }

    // próximo token deve ser "///"
    if(!ler_token(fp, tok)){
        snprintf(errbuf, errbuf_sz, "EOF ao esperar separador '///'");
        fclose(fp);
        return false;
    }
    if(strcmp(tok, "///") != 0){
        snprintf(errbuf, errbuf_sz, "Separador '///' nao encontrado, lido '%s'", tok);
        fclose(fp);
        return false;
    }

    // ler h*w tokens do mapa do passado
    for(int i = 0; i < out->h; i++){
        for(int j = 0; j < out->w; j++){
            if(!ler_token(fp, tok)){
                snprintf(errbuf, errbuf_sz, "EOF ao ler mapa do passado em (%d,%d)", i, j);
                fclose(fp);
                return false;
            }
            if(!analisa_celula_token(tok, cell_at(out->passado, out->h, out->w, i, j), perr, sizeof(perr))){
                snprintf(errbuf, errbuf_sz, "%s (passado em %d,%d)", perr, i, j);
                fclose(fp);
                return false;
            }
        }
    }

    fclose(fp);
    return true;
}

void mapa_liberar(Mapa *m){
    if(!m){
        return;
    }
    free(m->presente); m->presente = NULL;
    free(m->passado);    m->passado    = NULL;
    m->h = m->w = m->F = m->D = m->N = 0;
}

static void print_celula(const Celula *c, FILE *out){
    switch (c->type){
        case CELULA_VAZIA:   fputs("000", out); break;
        case CELULA_BLOQUEADA: fputs("***", out); break;
        case CELULA_ANCORA:  fputs("AAA", out); break;
        case CELULA_MONSTRO: fprintf(out, "%03d", c->value); break;
        default:           fputs("???", out); break;
    }
}

void mapa_print_info(const Mapa *m, FILE *out){
    fprintf(out, "Dimensoes: %dx%d | F=%d D=%d N=%d\n", m->h, m->w, m->F, m->D, m->N);
}

void mapa_print_matriz(const Mapa *m, const Celula *matriz, FILE *out){
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            if(j){
                fputc(' ', out);
            }
            print_celula(cell_at((Celula*)matriz, m->h, m->w, i, j), out);
        }
        fputc('\n', out);
    }
}
