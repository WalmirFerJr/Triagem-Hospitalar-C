#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char cor[20];
    char categoria[50];
    int tempo;
} Paciente;

void trim(char *str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) return;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
}

int get_prioridade_num(char *cor) {
    if (strcmp(cor, "Vermelho") == 0) return 1;
    if (strcmp(cor, "Laranja") == 0) return 2;
    if (strcmp(cor, "Amarelo") == 0) return 3;
    if (strcmp(cor, "Verde") == 0) return 4;
    if (strcmp(cor, "Azul") == 0) return 5;
    return 6; 
}

int eh_preferencial(char *categoria) {
    if (strcmp(categoria, "N/A") == 0) return 0;
    return 1;
}

void escalonar_prioridade(Paciente *p) {
    int cor_atual = get_prioridade_num(p->cor);

    if (cor_atual == 2 && p->tempo > 10) { 
        strcpy(p->cor, "Vermelho");
    }
    else if (cor_atual == 3 && p->tempo > 60) { 
        strcpy(p->cor, "Laranja");
    }
    else if (cor_atual == 4 && p->tempo > 120) {
        strcpy(p->cor, "Amarelo");
    }
    else if (cor_atual == 5 && p->tempo > 240) { 
        strcpy(p->cor, "Verde");
    }
}

int comparar_pacientes(const void *a, const void *b) {
    Paciente *p1 = (Paciente *)a;
    Paciente *p2 = (Paciente *)b;

    int prio1 = get_prioridade_num(p1->cor);
    int prio2 = get_prioridade_num(p2->cor);

    if (prio1 != prio2) {
        return prio1 - prio2;
    }

    if (prio1 == 1 || prio1 == 2) {
        return p2->tempo - p1->tempo;
    }

    int pref1 = eh_preferencial(p1->categoria);
    int pref2 = eh_preferencial(p2->categoria);

    if (pref1 != pref2) {
        return pref2 - pref1;
    }

    return p2->tempo - p1->tempo;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_pacientes.csv>\n", argv[0]);
        return 1;
    }

    FILE *arq_entrada = fopen(argv[1], "r");
    if (!arq_entrada) {
        printf("Erro ao abrir o arquivo de entrada: %s\n", argv[1]);
        return 1;
    }

    int capacidade = 100;
    int total_pacientes = 0;
    Paciente *lista = (Paciente *)malloc(capacidade * sizeof(Paciente));
    
    char linha[256];
    
    while (fgets(linha, sizeof(linha), arq_entrada)) {
        if (total_pacientes >= capacidade) {
            capacidade *= 2;
            lista = (Paciente *)realloc(lista, capacidade * sizeof(Paciente));
        }

        char *token_cor = strtok(linha, ",");
        char *token_cat = strtok(NULL, ",");
        char *token_tempo = strtok(NULL, ",");

        if (token_cor && token_cat && token_tempo) {
            char cor_limpa[20];
            char cat_limpa[50];
            
            strcpy(cor_limpa, token_cor);
            strcpy(cat_limpa, token_cat);
            
            trim(cor_limpa);
            trim(cat_limpa);
            
            strcpy(lista[total_pacientes].cor, cor_limpa);
            strcpy(lista[total_pacientes].categoria, cat_limpa);
            lista[total_pacientes].tempo = atoi(token_tempo);

            escalonar_prioridade(&lista[total_pacientes]);

            total_pacientes++;
        }
    }
    fclose(arq_entrada);

    qsort(lista, total_pacientes, sizeof(Paciente), comparar_pacientes);

    FILE *arq_saida = fopen("OrdemDeAtendimentos.csv", "w");
    if (!arq_saida) {
        printf("Erro ao criar arquivo de saida.\n");
        free(lista);
        return 1;
    }

    for (int i = 0; i < total_pacientes; i++) {
        fprintf(arq_saida, "%s, %s, %d\n", 
                lista[i].cor, 
                lista[i].categoria, 
                lista[i].tempo);
    }

    fclose(arq_saida);
    free(lista);

    return 0;
}