#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

enum acciones
{
    FIN = -1,
    ESPERA = 0,
    MOVER = 1,
    LLENAR = 2,
    VACIAR = 3,
    AGREGAR = 4,
};

// con estas variables controlaremos las acciones que hara cada jarra
// en cada turno
enum acciones accion = ESPERA;
int parametro = 0;
int semaforoPropio = 0;

// funciones del profe
void llenar(int *variable, int max)
{
    *variable = max;
};

void vaciar(int *variable)
{
    *variable = 0;
};

void agregar(int cantidad)
{
    accion = AGREGAR;
    parametro = cantidad;
};

void mover(int *variable, int cantidad)
{
    *variable = *variable - cantidad;
    accion = MOVER;
    parametro = cantidad;
};

// Busy waiting
void esperar(int valor)
{
    while (semaforoPropio != valor)
    {
    };
};

// Funciones para controlar los hilos que representan cada jarra de agua
void jarraGrande()
{
    int contenido = 0;
    int max = 5;

    // Primer paso
    llenar(&contenido, max);
    printf("\tLlenaste la jarra grande\n");

    // Segundo paso
    mover(&contenido, 3);
    semaforoPropio = 1;
    printf("\tMoviste 3 lt de la jarra grande a la pequeña\n");
    esperar(0);

    // Tercer paso
    accion = VACIAR;
    semaforoPropio = 1;
    printf("\tVaciaste la jarra pequeña\n");
    esperar(0);

    // Cuarto paso
    mover(&contenido, 2);
    semaforoPropio = 1;
    printf("\tMoviste 2 lt de la jarra grande a la pequeña\n");
    esperar(0);

    // Quinto paso
    llenar(&contenido, 5);
    printf("\tLlenaste la jarra grande\n");

    // Sexto paso
    mover(&contenido, 1);
    semaforoPropio = 1;
    printf("\tMoviste 1 lt de la jarra grande a la pequeña\n");
    esperar(0);

    if (contenido == 4)
    {
        printf("Felicidades desactivaste la bomba!\n");
    }
    else
    {
        printf("Muy mal, acabas de morir\n");
    }
    accion = FIN;
    semaforoPropio = 1;
}

void jarraChica()
{
    int contenido = 0;
    int max = 3;

    while (1 == 1)
    {
        esperar(1);
        if (accion == MOVER)
        {
            contenido += parametro;
            accion = ESPERA;
            parametro = 0;
            semaforoPropio = 0;
        }
        else if (accion == VACIAR)
        {
            vaciar(&contenido);
            semaforoPropio = 0;
        }
        else if (accion == FIN)
        {
            break;
        }
    }
}

int main(int argc, char const *argv[])
{

    // creamos e iniciamos los hilos
    pthread_t h1, h2;
    printf("Inicie los hilos\n");
    pthread_create(&h1, NULL, (void *)jarraGrande, NULL);
    pthread_create(&h2, NULL, (void *)jarraChica, NULL);

    // Esperamos a que los hilos terminen
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    return 0;
}
