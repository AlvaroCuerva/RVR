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
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = sizeof(char) * MAX_NAME + sizeof(int16_t) * 2;
        alloc_data(data_size);

        char* data = _data;

        memcpy(data, name, sizeof(char) * MAX_NAME);
        data += MAX_NAME * sizeof(char);

        memcpy(data, &pos_x, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(data, &pos_y, sizeof(int16_t));
    }

    int from_bin(char* _data)
    {
        char* data = _data;

        memcpy(name, data, MAX_NAME * sizeof(char));
        data += MAX_NAME * sizeof(char);

        memcpy(&pos_x, data, sizeof(int16_t));
        data += sizeof(int16_t);

        memcpy(&pos_y, data, sizeof(int16_t));
        data += sizeof(int16_t);
        return 0;
    }


public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    // 2. Escribir la serialización en un fichero
    // 3. Leer el fichero
    // 4. "Deserializar" en one_r
    // 5. Mostrar el contenido de one_r

    return 0;
}

