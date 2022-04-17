// Álvaro Cuerva Hernández
// Manuel Adeliño Consuegra

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <time.h>
#include <string.h>
#include <iostream>


//compilar
//   g++ redes_P2-5-TCP-Client.cc -o redes_P2-5-TCP-Client

 //ejecutar 
 //  ./redes_P2-5-TCP-Client 0.0.0.0 3000 t

int main(int argc, char **argv)
{
    // Gestion de errores
    if (argc != 3)
    {
        printf("Error argumentos\n");
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *info;

    memset(&hints, 0, sizeof(struct addrinfo));

    // hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &info);

    if (rc != 0)
    {
        printf("getaddrinfo: %s\n",gai_strerror(rc));
        return -1;
    }

    
    //Inicializar el socket y conectar
    int sd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    connect(sd, info->ai_addr, info->ai_addrlen);
    

    int bufferLen = 120; //si se pone a 80 no devuelve bien los datos
    char buffer[bufferLen];
    memset(&buffer, '\0', sizeof(char) * bufferLen);

    ssize_t bytes = -1;
    //mientras el servidor no cierre conexión
    while (bytes != 0)
    {
        //Leer en el buffer y enviar
        memset(&buffer, '\0', sizeof(char) * bufferLen);
        std::cin >> buffer;

        //Si el mensaje enviado es simplemente "Q" o "q", se acaba la conexíon, no hay que recibir nada
        if ((buffer[0] == 'Q' || buffer[0] == 'q') && buffer[1] == '\0')    break;

        bytes = send(sd, buffer, strlen(buffer) + 1, 0);
        
        //Recibir en el recvbuffer y escribir
        char recvBuffer[bufferLen];
        memset(&recvBuffer, '\0', bufferLen);
        bytes = recv(sd, recvBuffer, (bufferLen - 1) * sizeof(char), 0);
        
        if (bytes != 0)
        {
            recvBuffer[bytes] = '\0';
            printf("%s\n", recvBuffer);
        }
        
    }

    freeaddrinfo(info);
    close(sd);


    return 0;
}