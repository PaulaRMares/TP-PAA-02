#include "../headers/mapa.h"

static bool parse_cell_token(const char tok[4], Cell *c, char *err, size_t errsz){
    if(strcmp(tok, "000") == 0){
        c->type = CELL_EMPTY; 
        c->value = 0; 
        return true;
    }
    if(strcmp(tok, "***") == 0){
        c->type = CELL_BLOCKED;
        c->value = 0; 
        return true;
    }
    if(strcmp(tok, "AAA") == 0){
        c->type = CELL_ANCHOR; 
        c->value = 0; 
        return true;
    }
    if(isdigit((unsigned char)tok[0]) &&
    isdigit((unsigned char)tok[1]) &&
    isdigit((unsigned char)tok[2]) &&
    tok[3] == '\0'){
        int v = (tok[0]-'0')*100 + (tok[1]-'0')*10 + (tok[2]-'0');
        if(v >= 1 && v <= 999){
            c->type = CELL_MONSTER; 
            c->value = v; 
            return true;
        }
    }
    snprintf(err, errsz, "Token de célula inválido: '%s'", tok);
    return false;
}

static bool read_token(FILE *fp, char buf[4]){
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

bool mapa_load_from_file(const char *path, Mapa *out, char *errbuf, size_t errbuf_sz){
    if(!out){
        snprintf(errbuf, errbuf_sz, "Parâmetro 'out' nulo");
        return false;
    }

    memset(out, 0, sizeof(*out));
    FILE *fp = fopen(path, "r");
    if(!fp){
        snprintf(errbuf, errbuf_sz, "Não foi possível abrir '%s'", path);
        return false;
    }

    // primeira linha: h w F D N
    if(fscanf(fp, "%d %d %d %d %d", &out->h, &out->w, &out->F, &out->D, &out->N) != 5){
        snprintf(errbuf, errbuf_sz, "Primeira linha inválida. Esperado: h w F D N");
        fclose(fp);
        return false;
    }
    if(out->h <= 0 || out->w <= 0){
        snprintf(errbuf, errbuf_sz, "Dimensões inválidas: h=%d w=%d", out->h, out->w);
        fclose(fp);
        return false;
    }

    size_t total = (size_t)out->h * (size_t)out->w;
    out->present = (Cell*)calloc(total, sizeof(Cell));
    out->past    = (Cell*)calloc(total, sizeof(Cell));
    if(!out->present || !out->past){
        snprintf(errbuf, errbuf_sz, "Falha de alocação (h*w=%zu)", total);
        fclose(fp);
        return false;
    }

    // ler h*w tokens do mapa do presente
    char tok[4], perr[128];
    for(int i = 0; i < out->h; i++){
        for(int j = 0; j < out->w; j++){
            if(!read_token(fp, tok)){
                snprintf(errbuf, errbuf_sz, "EOF ao ler mapa do presente em (%d,%d)", i, j);
                fclose(fp);
                return false;
            }
            if(!parse_cell_token(tok, cell_at(out->present, out->h, out->w, i, j), perr, sizeof(perr))){
                snprintf(errbuf, errbuf_sz, "%s (presente em %d,%d)", perr, i, j);
                fclose(fp);
                return false;
            }
        }
    }

    // próximo token deve ser "///"
    if(!read_token(fp, tok)){
        snprintf(errbuf, errbuf_sz, "EOF ao esperar separador '///'");
        fclose(fp);
        return false;
    }
    if(strcmp(tok, "///") != 0){
        snprintf(errbuf, errbuf_sz, "Separador '///' não encontrado, lido '%s'", tok);
        fclose(fp);
        return false;
    }

    // ler h*w tokens do mapa do passado
    for(int i = 0; i < out->h; i++){
        for(int j = 0; j < out->w; j++){
            if(!read_token(fp, tok)){
                snprintf(errbuf, errbuf_sz, "EOF ao ler mapa do passado em (%d,%d)", i, j);
                fclose(fp);
                return false;
            }
            if(!parse_cell_token(tok, cell_at(out->past, out->h, out->w, i, j), perr, sizeof(perr))){
                snprintf(errbuf, errbuf_sz, "%s (passado em %d,%d)", perr, i, j);
                fclose(fp);
                return false;
            }
        }
    }

    fclose(fp);
    return true;
}

void mapa_free(Mapa *m){
    if(!m){
        return;
    }
    free(m->present); m->present = NULL;
    free(m->past);    m->past    = NULL;
    m->h = m->w = m->F = m->D = m->N = 0;
}

static void print_cell(const Cell *c, FILE *out){
    switch (c->type){
        case CELL_EMPTY:   fputs("000", out); break;
        case CELL_BLOCKED: fputs("***", out); break;
        case CELL_ANCHOR:  fputs("AAA", out); break;
        case CELL_MONSTER: fprintf(out, "%03d", c->value); break;
        default:           fputs("???", out); break;
    }
}

void mapa_print_header(const Mapa *m, FILE *out){
    fprintf(out, "Dimensões: %dx%d | F=%d D=%d N=%d\n", m->h, m->w, m->F, m->D, m->N);
}

void mapa_print_grid(const Mapa *m, const Cell *grid, FILE *out){
    for(int i = 0; i < m->h; i++){
        for(int j = 0; j < m->w; j++){
            if(j){
                fputc(' ', out);
            }
            print_cell(cell_at((Cell*)grid, m->h, m->w, i, j), out);
        }
        fputc('\n', out);
    }
}
