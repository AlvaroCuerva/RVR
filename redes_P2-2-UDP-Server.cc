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
//   gcc -o redes_P2-2-UDP-Server redes_P2-2-UDP-Server.cc
 
 //ejecutar 
 //  ./redes_P2-2-UDP-Server 0.0.0.0 3000

 //nc -u 127.0.0.1 3000




//Estructura de un socket:
// struct sockaddr_in {
//       sa_family_t    sin_family; // fijar a AF_INET
//       in_port_t      sin_port;   // puerto
//       struct in_addr sin_addr;   // dirección IP
//     };
//     struct in_addr {
//       uint32_t       s_addr;     // 32 bits dirección IP
//     };


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("No se ha introducido la dirección o el puerto");
        return 1;
    }

    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    // hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;//De base viene SOCK_STREAM, que no paran de llegar y sería TCP, con DGRAM es UDP

    int check = getaddrinfo(argv[1], argv[2], &hints, &info);

    if (check != 0)
    {
        fprintf(stderr, "Error using getaddrinfo: %s\n", gai_strerror(check));
        return -1;
    }

    int udpSocket = socket(info->ai_family, info->ai_socktype, 0);

    if (udpSocket == -1)
    {
        fprintf(stderr, "Error getting the socket %s\n", gai_strerror(udpSocket));
        return -1;
    }

    //
    bind(udpSocket, info->ai_addr, info->ai_addrlen);

    freeaddrinfo(info);

    const int bufferLen = 80; 

    while (true)
    {
        char buffer[buferLen] = {};

        struct sockaddr client;
        socklen_t clientLen = sizeof(sockaddr);

        //Recibir comando del cliente
        int bytes = recvfrom(udpSocket, buffer, bufferLen, 0, &client, &clientLen);

        char port[NI_MAXSERV];
        char hostName[NI_MAXHOST];
        //
        check = getnameinfo(&client, clientLen, hostName, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if (check != 0)
        {
            fprintf(stderr, "Error de getnameinfo: %s\n", gai_strerror(check));
            continue;
        }

        printf("%d bytes de %s:%s\n", bytes, hostName, port);
        time_t rawTime;
        struct tm *timeInfo;

        //Obtención de tiempo actual
        time(&rawTime);

        timeInfo = localtime(&rawTime);

        char *format;

        //Lectura de comando
        switch (buffer[0])
        {
        case 't':
            format = "%r";
            break;
        case 'd':
         format = "%F";
            break;
        case 'q':
            printf("Cerrando conexión\n");
            close(udpSocket);
            return 0;
        default:
            printf("Comando erróneo %s\n", buffer);
            sendto(udpSocket, "Comando erróneo\n", 21, 0, &client, clientLen);
            continue;
        }

        char send[80];
        int timeBytes = strftime(send, 80, format, timeInfo);
        send[timeBytes] = '\n';

        //Enviar respuesta al cliente
        sendto(udpSocket, send, timeBytes + 1, 0, &client, clientLen);
    }

    close(udpSocket);

    return 0;
}