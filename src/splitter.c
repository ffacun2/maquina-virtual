#include "splitter.h"
#include <math.h>

t_splitter constructorSplitter(int entrada, int tamanio)
{
    t_splitter aux;
    aux.entrada = entrada;
    aux.tamanio = tamanio;
    return aux;
}

void setEntrada(t_splitter *splitter, int entrada)
{
    splitter->entrada = entrada;
}

void setTamanio(t_splitter *splitter, int tamanio)
{
    splitter->tamanio = tamanio;
}

void getSalidas(t_splitter splitter, int salidas[CANT_BITS])
{
    int aux = splitter.entrada;
    for (int i = 0; i < CANT_BITS; i++)
    {
        salidas[i] = aux & (int)(pow(2, splitter.tamanio) - 1);
        aux = aux >> splitter.tamanio;
    }
}