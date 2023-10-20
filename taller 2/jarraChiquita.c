/**
 Dada una red a la cual está conectada la computadora que está ejeccutando este programa, 
 y que también exista otra computadora conectada a la misma y se provea su dirección, 
 entonces, el siguiente programa se encarga de comunicar ambas computadoras. 
 Más específicamente, permite que la computadora actual tenga un rol de cliente y servidor 
 al enviar y recibir mensajes.  

 El problema planteado, es el de una escena de la película "Duro de matar", en el cual se tiene
 una bomba que se desea desactivar colocando exactamente 4lts de agua en una jarra de capacidad 5lts.
 Este problema se ha modelado a través de autómatas de estados, los cuales representan los estados
 de las jarras proveídas para resolverlo (una de 3lts y la otra de 5lts).

 En este sentido, el programa actual también implementa el funcionamiento del autómata de estados
 de la jarra pequeña (3lts)(modelo entregado en la parte (a) de este taller).
 Este debe recibir mensajes así como enviar, y de este modo conocer cuánta 
agua se ha movido, añadido, etc., con el fin de desactivar la bomba.

 Autores:
    Kenny Rojas, 18-10595
    Gabriela Panqueva, 18-10761
    Simón Puyosa, 18-10717

 Créditos:
    Prof. Miguel Torrealba.
    Leandro Lucarella - Copyleft 2004.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define SERVER_PORT 4321
#define BUFFER_LEN 1024
#define MY_ADDR "127.0.0.1"
#define MAX_CONTENT 3

/* Configuramos el estado inicial en contenido = 0lts ya que está vacía la jarra */
int contenido = 0;

int main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr,"\nuso: %s [hostname de la jarra grande]\n", argv[0]);
        exit(1);
    }

    int sockfd;                    /* descriptor para el socket */
    struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
    struct sockaddr_in my_addr;    /* direccion IP y numero de puerto local */
    /* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
     * numero de bytes recibidos */
    int addr_len, numbytes;
    char buf[BUFFER_LEN]; /* Buffer de recepción */
    struct hostent *he;   /* para obtener nombre del host */
    char temp[BUFFER_LEN];

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* Leemos la ip de la jarra Grande */
    if ((he = gethostbyname(argv[1])) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }

    /* Se establece la estructura my_addr para luego llamar a bind() */
    my_addr.sin_family = AF_INET;          /* usa host byte order */
    my_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY;  /* escuchamos en todas las IPs */
    bzero(&(my_addr.sin_zero), 8);         /* rellena con ceros el resto de la estructura */

    /* asignarle un nombre al socket */
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(2);
    }

    their_addr.sin_family = AF_INET;          /* usa host byte order */
    their_addr.sin_port = htons(SERVER_PORT); /* usa network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    bzero(&(their_addr.sin_zero), 8); /* pone en cero el resto */
    addr_len = sizeof(struct sockaddr);
    printf("Esperando órdenes ....\n");

    /* Recibimos la info */
    addr_len = sizeof(struct sockaddr);
    while (1)
    {
        /* Limpiamos el buf, ya que suele tener basura */
        memset(buf, 0, sizeof(buf));

        /* Se recibe la petición de acción de la jarra grande y se verifica si algo falla */
        if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1)
        {
            perror("recvfrom");
            exit(3);
        }

        /* Se verifica el mensaje de la jarra grande, y que sea la acción de mover agua */
        if (strlen(buf) > 5 && strncmp(buf, "MOVER", 5) == 0)
        {
            int disponible = atoi(buf + 6);

            /* Se calcula cuánta agua se puede recibir de la jarra grande */
            int recibido = disponible <= MAX_CONTENT - contenido ? disponible : MAX_CONTENT - contenido;
            sprintf(temp, "SEPASO:%d", recibido);
            printf("Se recibió %d litros de agua de una jarra más grande.\n", recibido);
            contenido = recibido;

            /* Se envía el mensaje a la jarra pequeña y se verifica si algo falla */
            if ((numbytes = sendto(sockfd, temp, strlen(temp), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
            {
                perror("sendto");
                exit(2);
            }

            /* Limpiamos el buf */
            memset(temp, 0, sizeof(temp));
        }
        /* Se verifica si la jarra grande avisó el fin del proceso (llegó al estado contenido = 4lts) */
        else if (strcmp(buf, "FIN") == 0)
        {
            printf("¡Felicidades! Se ha desactivado la bomba.\n");
            close(sockfd);
            exit(0);
        }
        /* En caso de no reconocer el mensaje de la jarra grande */
        else
        {
            perror("Error: Acción no conocida.");
            exit(13);
        }

        /* Se verifica si la jarra está en estado contenido = 3lts, si es así, se vacía */
        if (contenido == MAX_CONTENT)
        {
            printf("Se vació la jarra.\n");
            contenido = 0;
        }
    }
}