#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t dataSize = sizeof(char) * MAX_NAME + sizeof(int16_t) * 2;
        alloc_data(dataSize);

        char* data = _data;
        memcpy(data, name, sizeof(char) * MAX_NAME);
        data += sizeof(char) * MAX_NAME;
        memcpy(data, &x, sizeof(int16_t));
        data += sizeof(int16_t);
        memcpy(data, &y, sizeof(int16_t));
    }

    int from_bin(char* _data)
    {
        char* data = _data;

        memcpy(name, data, sizeof(char) * MAX_NAME);
        data += sizeof(char) * MAX_NAME;

        memcpy(&x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&y, data, sizeof(int16_t));
        data += sizeof(int16_t);
        return 0;
    }


public:
    static const size_t MAX_NAME = 80;
    char name[MAX_NAME];
    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_read("", 0, 0);
    Jugador one_write("Player_ONE", 12, 345);

    // 1. Serializar el objeto one_w 

    one_write.to_bin();
    printf("1. Jugador one_write serializado.\n");

    // 2. Escribir la serialización en un fichero

    char* fileName = "./jugadorSerializado";
    int fd = open(fileName, O_CREAT | O_TRUNC | O_RDWR, 0666);
    write(fd, one_write.data(), one_write.size());

    printf("2. Fichero serializado escrito.\n");
    close(fd);

    // 3. Leer el fichero

    
    fd = open(fileName, O_RDONLY, 0666);
    char data[one_write.size()];
    read(fd, data, one_write.size());
    printf("3. Fichero serializado leído.\n");

    close(fd);

    // 4. "Deserializar" en one_r

    one_read.from_bin(data);
    printf("4. One_read deserializado.\n");

    // 5. Mostrar el contenido de one_r

    printf("Player name: %s\nPos x: %d\nPos y: %d\n", one_read.name, one_read.x, one_read.y);

    return 0;
}

