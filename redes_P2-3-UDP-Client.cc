// Álvaro Cuerva Hernández
// Manuel Adeliño Consuegra

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <time.h>
#include <string.h>
#include <unistd.h>

//compilar
//   gcc -o redes_P2-3-UDP-Client redes_P2-3-UDP-Client.cc

 //ejecutar 
 //  ./redes_P2-3-UDP-Client 0.0.0.0 3000 t

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo* info;

    //Inicializar socket UDP
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &info);

    if ( rc != 0 )
    {
        printf("getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    //info contiene la representación como sockaddr de dirección + puerto

    int sd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    freeaddrinfo(info);

    //Construir comando 
    int bufferLen = 80;
    char buffer[bufferLen];
    memset(&buffer, '\0', bufferLen);

    struct sockaddr client_addr;
    socklen_t client_len = sizeof(struct sockaddr);

    ssize_t bytes;

    //Enviar comando al servidor,con la t, d o el parámetro[3] que sea
    sendto(sd, argv[3], strlen(argv[3]) + 1, 0, info->ai_addr, info->ai_addrlen);

    //Recibir respuesta del servidor
    bytes = recvfrom(sd, buffer, bufferLen * sizeof(char), 0, info->ai_addr, &info->ai_addrlen);

    //Escribo lo que me envía el servidor
    printf("%s\n", buffer);


    return 0;
}