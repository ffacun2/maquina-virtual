#define CANT_BITS 32

typedef struct
{
    int entrada;
} t_spliter;

void setEntrada(t_spliter *spliter, int entrada);
void getSalidas(t_spliter spliter, int salidas[CANT_BITS]);