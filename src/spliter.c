#include "spliter.h"

void setEntrada(t_spliter *spliter, int entrada)
{
    spliter->entrada = entrada;
}

void getSalidas(t_spliter spliter, int salidas[CANT_BITS])
{
    int aux = spliter.entrada;
    for (int i = 0; i < CANT_BITS; i++)
    {
        salidas[i] = aux & 0x1;
        aux = aux >> 1;
    }
}