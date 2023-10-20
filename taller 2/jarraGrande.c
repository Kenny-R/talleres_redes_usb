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
 de la jarra grande (5lts)(modelo entregado en la parte (a) de este taller).
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
#include <sys/socket.h>
#define SERVER_PORT 4321
#define BUFFER_LEN 1024
#define MY_ADDR "127.0.0.1"
#define MAX_CONTENT 5

/* Configuramos el estado inicial en contenido = 0lts ya que está vacía la jarra */
int contenido = 0;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "\nuso: %s [hostname de la jarra pequeña]\n", argv[0]);
        exit(1);
    }

    int sockfd;                    /* descriptor para el socket */
    struct sockaddr_in their_addr; /* direccion IP y numero de puerto del cliente */
    struct sockaddr_in my_addr;    /* direccion IP y numero de puerto local */
    /* addr_len contendra el taman~o de la estructura sockadd_in y numbytes el
     * numero de bytes recibidos */
    int addr_len, numbytes;
    char buf[BUFFER_LEN]; /* Buffer de recepción */
    char temp[BUFFER_LEN];
    struct hostent *he; /* para obtener nombre del host */

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    /* Leemos la IP de la jarra pequeña */
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

    /* Esperamos a que la jarra pequeña (3lts) reciba la primera peticion para saber si se pudo conectar */
    while (1)
    {
        /* Se verifica si la jarra está vacía, y si lo está, se llena */
        if (contenido == 0)
        {
            printf("Se ha llenado la jarra.\n");
            contenido = MAX_CONTENT;
        }

        /* Limpiamos el buf, ya que suele tener basura */
        memset(buf, 0, sizeof(buf));

        /* Se verifica que haya conexión entre las jarras */
        /* Limpiamos el buf */
        memset(temp, 0, sizeof(temp));
        sprintf(temp, "MOVER:%d", contenido);

        /* Se envía el mensaje a la jarra pequeña y se verifica si algo falla */
        if ((numbytes = sendto(sockfd, temp, strlen(temp), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
        {
            perror("sendto");
            exit(2);
        }

        /* Limpiamos el buf */
        memset(buf, 0, sizeof(buf));

        /* Se recibe la respuesta de la jarra pequeña y se verifica si algo falla */
        if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1)
        {
            perror("recvfrom");
            exit(3);
        }
        /* Se resta del contenido, el agua que fue movida a la jarra pequeña */
        printf("Se ha movido %d Litros de agua de la jarra, a una jarra más pequeña.\n", atoi(buf + 7));
        contenido = contenido - atoi(buf + 7);

        /* Se comprueba el estado de la jarra, y si está en el estado igual a contenido = 4 lts, se desactiva la bomba */
        if (contenido == 4)
        {
            printf("¡Felicidades! Se ha desativado la bomba.\n");

            /* Se avisa a la jarra pequeña que ya se ha logrado el objetivo */
            if ((numbytes = sendto(sockfd, "FIN", strlen("FIN"), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
            {
                perror("sendto");
                exit(2);
            }

            break;
        }
    }
    /* Se cierra el socket */
    close(sockfd);
    exit(0);
}