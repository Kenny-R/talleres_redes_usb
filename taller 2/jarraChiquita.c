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

int contenido = 0;

int main(int argc, char *argv[])
{

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
    printf("Esperando ordenes ....\n");
    if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1)
    {
        perror("recvfrom");
        exit(3);
    }
    /* Recibimos la info */
    addr_len = sizeof(struct sockaddr);
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1)
        {
            perror("recvfrom");
            exit(3);
        }

        if (strlen(buf) > 5 && strncmp(buf, "MOVER", 5) == 0)
        {

            int disponible = atoi(buf + 6);
            sprintf(temp, "SEPASO:%d", disponible <= 3 - contenido ? disponible : 3 - contenido);
            printf("Se recibio %d litros de agua de una jarra grande\n", disponible <= 3 - contenido ? disponible : 3 - contenido);
            contenido = disponible <= 3 - contenido ? disponible : 3 - contenido;
            if ((numbytes = sendto(sockfd, temp, strlen(temp), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
            {
                perror("sendto");
                exit(2);
            }
            memset(temp, 0, sizeof(temp));
        }
        else if (strcmp(buf, "FIN") == 0)
        {
            printf("Felicidades, se desactivo la bomba\n");
            close(sockfd);
            exit(0);
        }
        else
        {
            perror("Error: Accion no conocida");
            exit(13);
        }

        if (contenido == MAX_CONTENT)
        {
            printf("Se vacia la jarra\n");
            contenido = 0;
        }
    }
}