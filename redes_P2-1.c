// Álvaro Cuerva Hernández
// Manuel Adeliño Consuegra

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

//compilar
//   gcc -o redes_P2-1 redes_P2-1.c
 
 //ejecutar 
 //  ./redes_P2-1 www.ucm.es

// getaddrinfo: Se le da como parámetro un variable de tipo addrinfo y devuelve ahí toda la info de una dirección. Descripción manual:
//  Given  node  and service, which identify an Internet host and a service, getaddrinfo() returns one or more addrinfo structures, each of which contains
//  an Internet address that can be specified in a call to bind(2) or connect(2).  The getaddrinfo() function combines the functionality provided  by  the
//  gethostbyname(3)  and  getservbyname(3) functions into a single interface, but unlike the latter functions, getaddrinfo() is reentrant and allows pro‐
//  grams to eliminate IPv4-versus-IPv6 dependencies.
// DeVUELVE 0 si es correcto, y si no un código de error, que con el método gai_strerror se consigue en string


//Esta es la estructura de addrinfo
// struct addrinfo {
//                int              ai_flags;
//                int              ai_family;
//                int              ai_socktype;
//                int              ai_protocol;
//                socklen_t        ai_addrlen;
//                struct sockaddr *ai_addr;
//                char            *ai_canonname;
//                struct addrinfo *ai_next;
//            };

//getnameinfo: es el inverso a getaddr info, devuelve en hostname la dirección, p.e. 199.59.243.200    
int main (int argc, char* argv[]){
    
    //Si no se le pasa la dirección como parámetro
    if(argc<2)
    {
        printf("No se ha introducido ninguna dirección \n");
        return 1;
    }

    struct addrinfo* info;
    int flagError = getaddrinfo(argv[1],argv[2],NULL,&info);

    if(flagError != 0){
        printf("Error getaddrinfo: %s\n" ,gai_strerror(flagError));
        return -1;
    }

    struct addrinfo* it;

    for(it = info; it != NULL; it = it->ai_next){

        char hostname[NI_MAXHOST];
        getnameinfo(it->ai_addr, it->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
       
        if(hostname != "\0"){
            printf("Host: %s    Familia: %d    TipodeSocket: %d\n", hostname, it->ai_family, it->ai_socktype);
        }


    }

    freeaddrinfo(info);

    return 0;
}