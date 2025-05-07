#include "mv.h"

typedef struct
{
    char header[8];
    int registros[CANT_REGISTROS];
    t_tabla_segmento tabla_segmentos[CANT_SEGMENTOS];
    char memoria[TAMANO_MEMORIA];
} t_imagen;

t_imagen generarImagen(t_MV mv);
