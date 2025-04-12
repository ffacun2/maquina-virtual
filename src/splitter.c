#include "splitter.h"

void setEntrada(t_splitter *splitter, int entrada)
{
    splitter->entrada = entrada;
}

void getSalidas(t_splitter splitter, int salidas[CANT_BITS])
{
    int aux = splitter.entrada;
    for (int i = 0; i < CANT_BITS; i++)
    {
        salidas[i] = aux & 0x1;
        aux = aux >> 1;
    }
}