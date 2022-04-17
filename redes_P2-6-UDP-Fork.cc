//Álvaro Cuerva Hernández 
//Manuel Adeliño Consuegra

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>

#define MAX_THREADS 5

//g++ redes_P2-6-UDP-Fork.cc -o redes_P2-6-UDP-Fork -pthread
//nc -u 127.0.0.1 3000 en distintas consolas y llamar a "d" y "t"

class MessageThread {
public:
    MessageThread(int socket) : udp_socket(socket) {
    }

    void do_message(){
    const int bufferLen = 80; 

    while (true)
    {
        char buffer[bufferLen] = {};

        struct sockaddr client;
        socklen_t clientLen = sizeof(sockaddr);
    
        int bytes = recvfrom(udp_socket, buffer, bufferLen, 0, &client, &clientLen);

        char puerto[NI_MAXSERV];
        char nombreHost[NI_MAXHOST];
        //
        int check = getnameinfo(&client, clientLen, nombreHost, NI_MAXHOST, puerto, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        sleep(3);
        if (check != 0)
        {
            fprintf(stderr, "Error using getnameinfo: %s\n", gai_strerror(check));
            continue;
        }

        printf("%d bytes de %s:%s\n", bytes, nombreHost, puerto);
        time_t rawTime;
        struct tm *timeInfo;

        time(&rawTime);

        timeInfo = localtime(&rawTime);

        char *format;

        switch (buffer[0])
        {
        case 't':
            format = "%r";
            break;
        case 'd':
         format = "%F";
            break;
        default:
            printf("Comando no soportado %s\n", buffer);
            continue;
        }

        char send[80];
        int timeBytes = strftime(send, 80, format, timeInfo);
        send[timeBytes] = '\n';

        sendto(udp_socket, send, timeBytes + 1, 0, &client, clientLen);
    }
    }

private:
    int udp_socket;
};

int main(int argc, char** argv){
    if(argc < 3 || argc > 3){
        std::cout << "Uso: ./ejercicio6 <ipv4> <port>" << std::endl;
        return -1;
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

    int udp_socket = socket(info->ai_family, info->ai_socktype, 0);
    if(udp_socket == -1){
        fprintf(stderr, "Error getting the socket %s\n", gai_strerror(udp_socket));
        return -1;
    }

    bind(udp_socket, info->ai_addr, info->ai_addrlen);
    
    freeaddrinfo(info);


    for(int i = 0; i < MAX_THREADS; ++i){
        MessageThread* mthread = new MessageThread(udp_socket);

        std::thread([mthread](){
            mthread->do_message();
            delete mthread;
        }).detach();
    }

    char salir;

    //Mientras qeu nos e pulsa la q no se sale
    while(std::cin >> salir){
        if(salir == 'q'){
            break;
        }   
    }
    printf("Saliendo...\n");
    check = close(udp_socket);
    if(check < 0){
        printf("Error close: %s\n",strerror(errno)); 
        return -1;
    }

    return 0;
}