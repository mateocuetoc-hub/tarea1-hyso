#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANT_NODOS 30
#define CANT_OBJETIVOS 5
#define MAX_RUTA 31
#define INFINITO 9999

int grafo[CANT_NODOS + 1][CANT_NODOS + 1];
int objetivos[CANT_OBJETIVOS];
int nodoInicial;

int rutaActual[MAX_RUTA];
int mejorRuta[MAX_RUTA];
int visitados[CANT_OBJETIVOS];

int largoRutaActual;
int mejorLargo;

int obtenerNumero(char linea[]);
void inicializarGrafo();
void leerGrafo();
void imprimirGrafo();
void inicializarBusqueda();
int nodoEstaEnRuta(int nodo);
int buscarObjetivo(int nodo);
int todosObjetivosVisitados();
void imprimirRuta(int ruta[], int largo);
void actualizarMejorRuta();
void recorrerGrafo(int nodoActual); 


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
void inicializarGrafo()
{
    int i;
    int j;

    for (i = 0; i <= CANT_NODOS; i++) {
        for (j = 0; j <= CANT_NODOS; j++) {
            grafo[i][j] = -1;
        }
    }
}

void leerGrafo()
{
    FILE *archivo;
    char linea[300];
    char *token;

    int origen;
    int destino;
    int contadorObjetivos;
    int posicionVecino;

    archivo = fopen("grafo.csv", "r");

    if (archivo == NULL) {
        printf("Error: no se pudo abrir grafo.csv\n");
        return;
    }

    /* Primera linea: nodo inicial */
    if (fgets(linea, sizeof(linea), archivo) != NULL) {
        nodoInicial = obtenerNumero(linea);
    }

    /* Segunda linea: nodos obligatorios */
    if (fgets(linea, sizeof(linea), archivo) != NULL) {
        contadorObjetivos = 0;
        token = strtok(linea, ";\n\r");

        while (token != NULL && contadorObjetivos < CANT_OBJETIVOS) {
            objetivos[contadorObjetivos] = atoi(token);
            contadorObjetivos++;
            token = strtok(NULL, ";\n\r");
        }
    }

    /* Desde la tercera linea: enlaces del grafo */
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        token = strtok(linea, ";\n\r");

        if (token != NULL) {
            origen = atoi(token);
            posicionVecino = 0;

            token = strtok(NULL, ";\n\r");

            while (token != NULL) {
                destino = atoi(token);

                if (origen >= 1 && origen <= CANT_NODOS &&
                    destino >= 1 && destino <= CANT_NODOS &&
                    posicionVecino <= CANT_NODOS) {

                    grafo[origen][posicionVecino] = destino;
                    posicionVecino++;
                }

                token = strtok(NULL, ";\n\r");
            }
        }
    }

    fclose(archivo);
}

void imprimirGrafo()
{
    int i;
    int j;

    printf("Nodo inicial: %d\n", nodoInicial);

    printf("Nodos obligatorios: ");
    for (i = 0; i < CANT_OBJETIVOS; i++) {
        printf("%d ", objetivos[i]);
    }

    printf("\n\n");

    printf("Grafo cargado como matriz de vecinos:\n");

    for (i = 1; i <= CANT_NODOS; i++) {
        printf("Vecinos del nodo %d: ", i);

        j = 0;
        while (j <= CANT_NODOS && grafo[i][j] != -1) {
            printf("%d ", grafo[i][j]);
            j++;
        }

        printf("\n");
    }
}

void inicializarBusqueda()
{
    int i;

    largoRutaActual = 0;
    mejorLargo = INFINITO;

    for (i = 0; i < MAX_RUTA; i++) {
        rutaActual[i] = -1;
        mejorRuta[i] = -1;
    }

    for (i = 0; i < CANT_OBJETIVOS; i++) {
        visitados[i] = 0;
    }
}
int nodoEstaEnRuta(int nodo)
{
    int i;

    /* 
       Recorremos la ruta actual desde la posicion 0
       hasta largoRutaActual - 1.
       
       largoRutaActual indica cuántos nodos tiene guardados
       la ruta en este momento.
    */
    for (i = 0; i < largoRutaActual; i++) {

        /*
           Si encontramos el nodo dentro de rutaActual,
           significa que ya pasamos por ese nodo en esta ruta.
           
           Retornamos 1 para indicar "sí está en la ruta".
        */
        if (rutaActual[i] == nodo) {
            return 1;
        }
    }

    /*
       Si termina el ciclo y nunca encontramos el nodo,
       significa que todavía no ha sido visitado en esta ruta.
       
       Retornamos 0 para indicar "no está en la ruta".
    */
    return 0;
}
int buscarObjetivo(int nodo)
{
    int i;

    /*
       Recorremos el arreglo de objetivos.
       
       Este arreglo contiene los 5 nodos obligatorios
       que fueron leídos desde la segunda línea del archivo grafo.csv.
       
       Por ejemplo:
       objetivos[0] = 2
       objetivos[1] = 8
       objetivos[2] = 17
       objetivos[3] = 20
       objetivos[4] = 28
    */
    for (i = 0; i < CANT_OBJETIVOS; i++) {

        /*
           Si el nodo recibido coincide con alguno de los objetivos,
           retornamos la posición donde fue encontrado.
           
           Por ejemplo:
           si nodo = 17 y objetivos[2] = 17,
           entonces retorna 2.
        */
        if (objetivos[i] == nodo) {
            return i;
        }
    }

    /*
       Si termina el ciclo y no se encontró el nodo,
       significa que el nodo no es uno de los 5 obligatorios.
       
       Retornamos -1 para indicar "no es objetivo".
    */
    return -1;
}

int todosObjetivosVisitados()
{
    int i;

    /*
       Recorremos el arreglo visitados.
       
       Cada posicion representa si uno de los 5 nodos obligatorios
       ya fue visitado en la ruta actual.
       
       Ejemplo:
       objetivos:  2  8  17 20 28
       visitados:  1  0  1  0  0
       
       Eso significa:
       - ya pasamos por 2
       - no pasamos por 8
       - ya pasamos por 17
       - no pasamos por 20
       - no pasamos por 28
    */
    for (i = 0; i < CANT_OBJETIVOS; i++) {

        /*
           Si encontramos un 0, significa que todavía falta
           visitar al menos un nodo obligatorio.
           
           Por eso retornamos 0.
        */
        if (visitados[i] == 0) {
            return 0;
        }
    }

    /*
       Si el ciclo termina sin encontrar ceros,
       significa que los 5 objetivos ya fueron visitados.
       
       Retornamos 1 para indicar que la ruta ya cumple.
    */
    return 1;
}
void imprimirRuta(int ruta[], int largo)
{
    int i;

    /*
       Esta funcion imprime una ruta almacenada en un arreglo.

       El arreglo ruta contiene los nodos recorridos.
       La variable largo indica cuantas posiciones validas tiene la ruta.

       Ejemplo:
       ruta[0] = 10
       ruta[1] = 4
       ruta[2] = 1
       largo = 3

       Se imprime:
       10 -> 4 -> 1
    */

    for (i = 0; i < largo; i++) {

        /*
           Imprimimos el nodo que esta en la posicion i de la ruta.
        */
        printf("%d", ruta[i]);

        /*
           Si no estamos en el ultimo nodo, imprimimos una flecha.
           Esto evita que la ruta termine con una flecha extra.
        */
        if (i < largo - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
}

void actualizarMejorRuta()
{
    int i;

    /*
       Esta funcion compara la ruta actual con la mejor ruta encontrada.

       largoRutaActual indica cuantos nodos tiene la ruta actual.
       mejorLargo guarda el largo de la mejor ruta encontrada hasta ahora.

       Si la ruta actual es mas corta que la mejor ruta guardada,
       entonces se actualiza mejorRuta.
    */

    if (largoRutaActual < mejorLargo) {

        mejorLargo = largoRutaActual;

        /*
           Copiamos la ruta actual en mejorRuta.
        */
        for (i = 0; i < largoRutaActual; i++) {
            mejorRuta[i] = rutaActual[i];
        }

        /*
           Limpiamos el resto del arreglo mejorRuta con -1
           para evitar que queden datos antiguos.
        */
        for (i = largoRutaActual; i < MAX_RUTA; i++) {
            mejorRuta[i] = -1;
        }

        printf("\nNueva mejor ruta encontrada:\n");
        imprimirRuta(mejorRuta, mejorLargo);
        printf("Cantidad de nodos visitados: %d\n", mejorLargo);
        printf("Cantidad de saltos: %d\n", mejorLargo - 1);
    }
}

void recorrerGrafo(int nodoActual)
{
    int i;
    int vecino;
    int posicionObjetivo;
    int estabaVisitado;

    /*
       Si la ruta ya alcanzó el máximo permitido,
       no seguimos recorriendo para evitar salirnos del arreglo.
    */
    if (largoRutaActual >= MAX_RUTA) {
        return;
    }

    /*
       Agregamos el nodo actual a la ruta.
    */
    rutaActual[largoRutaActual] = nodoActual;
    largoRutaActual++;

    /*
       Revisamos si el nodo actual es uno de los nodos obligatorios.
       Si lo es, guardamos su posicion dentro del arreglo objetivos.
       Si no lo es, buscarObjetivo retorna -1.
    */
    posicionObjetivo = buscarObjetivo(nodoActual);
    estabaVisitado = 0;

    /*
       Si el nodo actual era objetivo, lo marcamos como visitado.
       Antes guardamos si ya estaba visitado, porque al volver
       de la recursion debemos restaurar el estado anterior.
    */
    if (posicionObjetivo != -1) {
        estabaVisitado = visitados[posicionObjetivo];
        visitados[posicionObjetivo] = 1;
    }

    /*
       Si ya pasamos por los 5 nodos obligatorios,
       la ruta actual es una solucion valida.
       Entonces revisamos si es mejor que la mejor ruta guardada.
    */
    if (todosObjetivosVisitados() == 1) {
        actualizarMejorRuta();
    } else {

        /*
           Si todavia faltan objetivos, recorremos los vecinos
           del nodo actual.

           Como el grafo fue guardado como matriz de vecinos,
           cada fila tiene vecinos hasta encontrar un -1.
        */
        i = 0;

        while (i <= CANT_NODOS && grafo[nodoActual][i] != -1) {
            vecino = grafo[nodoActual][i];

            /*
               Solo avanzamos si el vecino no está ya en la ruta.
               Esto evita ciclos como:
               10 -> 4 -> 1 -> 10 -> 4 -> ...
            */
            if (nodoEstaEnRuta(vecino) == 0) {

                /*
                   Poda simple:
                   si la ruta actual ya es igual o más larga que
                   la mejor ruta encontrada, no conviene seguir.
                */
                if (largoRutaActual < mejorLargo) {
                    recorrerGrafo(vecino);
                }
            }

            i++;
        }
    }

    /*
       Backtracking:
       Antes de salir de la funcion, debemos deshacer los cambios
       hechos en este nivel de recursion.

       Si marcamos un objetivo como visitado, lo dejamos como estaba antes.
    */
    if (posicionObjetivo != -1) {
        visitados[posicionObjetivo] = estabaVisitado;
    }

    /*
       Sacamos el nodo actual de la ruta.
    */
    largoRutaActual--;
    rutaActual[largoRutaActual] = -1;
}

int main()
{
    inicializarGrafo();
    leerGrafo();
    imprimirGrafo();

    inicializarBusqueda();

    printf("\nBuscando rutas...\n");

    recorrerGrafo(nodoInicial);

    printf("\nMejor ruta final encontrada:\n");

    if (mejorLargo == INFINITO) 
    {
        printf("No se encontro una ruta que pase por todos los objetivos.\n");
    } 
    else
    {
        imprimirRuta(mejorRuta, mejorLargo);
        printf("Cantidad de nodos visitados: %d\n", mejorLargo);
        printf("Cantidad de saltos: %d\n", mejorLargo - 1);
    }

    return 0;
}
