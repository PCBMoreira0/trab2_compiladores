#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int merge_files(const char *arq1_path, const char *arq2_path, const char *destino_path) {
    FILE *arq1 = fopen(arq1_path, "r");
    FILE *arq2 = fopen(arq2_path, "r");
    FILE *destino = fopen(destino_path, "w");

    if (arq1 == NULL || arq2 == NULL || destino == NULL) {
        printf("Erro ao abrir um dos arquivos.\n");
        if (arq1) fclose(arq1);
        if (arq2) fclose(arq2);
        if (destino) fclose(destino);
        return 0;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_lidos;

    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), arq1)) > 0) {
        fwrite(buffer, 1, bytes_lidos, destino);
    }

    fputc('\n', destino);

    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), arq2)) > 0) {
        fwrite(buffer, 1, bytes_lidos, destino);
    }

    fclose(arq1);
    fclose(arq2);
    fclose(destino);

    return 1;
}