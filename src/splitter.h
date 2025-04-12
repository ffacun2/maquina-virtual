#define CANT_BITS 32

// Un splitter es un dipositivo que recibe un bus y separa el dato ingresados
// Entrada: es el bus que recibe
// Salidas: vector que agrupa los 1 y 0 separados
// Tamanio: tamaño de las celdas de Salidas

typedef struct
{
    int entrada, tamanio;
} t_splitter;

t_splitter constructorSplitter(int entrada, int tamanio);
void setEntrada(t_splitter *splitter, int entrada);
void setTamanio(t_splitter *splitter, int tamanio);
void getSalidas(t_splitter splitter, int salidas[CANT_BITS]);