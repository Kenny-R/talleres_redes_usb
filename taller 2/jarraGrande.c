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
    char temp[BUFFER_LEN];
    struct hostent *he; /* para obtener nombre del host */

    /* Creamos el socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    struct timeval read_timeout;
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

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

    /* Esperamos a que la jarra chiquita reciba la primera peticion para saber si se pudo conectar */
    while (1)
    {
        if (contenido == 0)
        {
            printf("Se llena la jarra grande\n");
            contenido = MAX_CONTENT;
        }

        memset(buf, 0, sizeof(buf));
        while (buf[0] == 0)
        {
            memset(temp, 0, sizeof(temp));
            sprintf(temp, "MOVER:%d", contenido);
            if ((numbytes = sendto(sockfd, temp, strlen(temp), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
            {
                perror("sendto");
                exit(2);
            }

            memset(buf, 0, sizeof(buf));
            if ((numbytes = recvfrom(sockfd, buf, BUFFER_LEN, 0, (struct sockaddr *)&their_addr, (socklen_t *)&addr_len)) == -1)
            {
                printf("No se ha recibido resupuesta de la jarra chiquita, restableciendo conexion\n");
                continue;
            }
        }

        printf("Se movio %d Litros de agua de la jarra grande a la jarra chiquita\n", atoi(buf + 7));

        contenido = contenido - atoi(buf + 7);

        if (contenido == 4)
        {
            printf("Felicidades has desactivado la bomba\n");

            if ((numbytes = sendto(sockfd, "FIN", strlen("FIN"), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1)
            {
                perror("sendto");
                exit(2);
            }

            break;
        }
    }
    /* cierro socket */
    close(sockfd);
    exit(0);
}