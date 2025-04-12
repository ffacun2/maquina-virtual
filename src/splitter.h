#define CANT_BITS 32

typedef struct
{
    int entrada;
} t_splitter;

void setEntrada(t_splitter *splitter, int entrada);
void getSalidas(t_splitter splitter, int salidas[CANT_BITS]);