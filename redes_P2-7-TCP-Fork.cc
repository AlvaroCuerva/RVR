#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <thread>

//g++ redes_P2-7-TCP-Fork.cc -o redes_P2-7-TCP-Fork -pthread
//./redes_P2-7-TCP-Fork 0.0.0.0 2222
//nc 127.0.0.1 2222

class ClientThread{
public:
    ClientThread(int sock): _sock(sock){}

    void do_message(){
        int bufferLen = 120;
        char buffer[bufferLen];
        memset(&buffer, '\0', sizeof(char) * bufferLen);

        while(true){
            int c = recv(_sock, (void*)buffer, (bufferLen - 1) * sizeof(char), 0);
 
            if(c == 0){
                printf("Conexión terminada\n");
            }

            buffer[c] = '\0'; 

            send(_sock, buffer, c + 1, 0);
            
        }

    }

    virtual ~ClientThread(){
        close(_sock);
    }
private:
    int _sock;
};

int main(int argc, char** argv){
    // Gestion de errores
    if (argc != 3)
    {
        printf("Formato incorrecto: \n ./redes_P2-4-TCP-Server host port\n");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *info;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int rc = getaddrinfo(argv[1], argv[2], &hints, &info);

    if (rc != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        fprintf(stderr, "Error getting the socket %s\n", gai_strerror(tcp_socket));
        return -1;
    }

    bind(tcp_socket, info->ai_addr, info->ai_addrlen);

    freeaddrinfo(info);

    int maxConexiones = 0;  //Número máximo de conexiones pendientes
    listen(tcp_socket, maxConexiones);

    struct sockaddr cliente_addr;
    socklen_t cliente_len = sizeof(struct sockaddr);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    while(true){
        
        int cliente_sd = accept(tcp_socket, &cliente_addr, &cliente_len);

        getnameinfo(&cliente_addr, cliente_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        
        printf("Conexión desde Host:%s Puerto:%s\n",host, serv);

        ClientThread* mthread = new ClientThread(cliente_sd);

        std::thread([mthread](){
            mthread->do_message();
            delete mthread;
        }).detach();
    }

    close(tcp_socket);
}