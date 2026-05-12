#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANT_NODOS 30
#define CANT_OBJETIVOS 5

int obtenerNumero(char linea[])
{
    int i = 0;

    while (linea[i] != '\0') {
        if (linea[i] >= '0' && linea[i] <= '9') {
            return atoi(&linea[i]);
        }
        i++;
    }

    return 0;
}

int main()
{
    FILE *archivo;
    char linea[300];
    char *token;

    int grafo[CANT_NODOS + 1][CANT_NODOS + 1];
    int nodoInicial;
    int objetivos[CANT_OBJETIVOS];

    int i;
    int j;
    int origen;
    int destino;
    int contadorObjetivos;

    nodoInicial = 0;

    /* Inicializar matriz en 0 */
    for (i = 0; i <= CANT_NODOS; i++) {
        for (j = 0; j <= CANT_NODOS; j++) {
            grafo[i][j] = 0;
        }
    }

    archivo = fopen("grafo.csv", "r");

    if (archivo == NULL) {
        printf("Error: no se pudo abrir grafo.csv\n");
        return 1;
    }

    /* Leer primera linea: nodo inicial */
    if (fgets(linea, sizeof(linea), archivo) != NULL) {
        nodoInicial = obtenerNumero(linea);
    }

    /* Leer segunda linea: nodos obligatorios */
    if (fgets(linea, sizeof(linea), archivo) != NULL) {
        contadorObjetivos = 0;
        token = strtok(linea, ";\n\r");

        while (token != NULL && contadorObjetivos < CANT_OBJETIVOS) {
            objetivos[contadorObjetivos] = atoi(token);
            contadorObjetivos++;
            token = strtok(NULL, ";\n\r");
        }
    }

    /* Leer el grafo desde la tercera linea */
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        token = strtok(linea, ";\n\r");

        if (token != NULL) {
            origen = atoi(token);

            token = strtok(NULL, ";\n\r");

            while (token != NULL) {
                destino = atoi(token);

                if (origen >= 1 && origen <= CANT_NODOS &&
                    destino >= 1 && destino <= CANT_NODOS) {
                    grafo[origen][destino] = 1;
                }

                token = strtok(NULL, ";\n\r");
            }
        }
    }

    fclose(archivo);

    printf("Nodo inicial: %d\n", nodoInicial);

    printf("Nodos obligatorios: ");
    for (i = 0; i < CANT_OBJETIVOS; i++) {
        printf("%d ", objetivos[i]);
    }

    printf("\n\n");
    printf("Grafo cargado en matriz:\n");

    for (i = 1; i <= CANT_NODOS; i++) {
        printf("Desde nodo %d puedo ir a: ", i);

        for (j = 1; j <= CANT_NODOS; j++) {
            if (grafo[i][j] == 1) {
                printf("%d ", j);
            }
        }

        printf("\n");
    }

    return 0;
}