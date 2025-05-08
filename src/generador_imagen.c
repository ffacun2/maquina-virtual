#include "generador_imagen.h"

void generarImagen(t_MV *mv)
{
    int reg;
    int base;
    int tam;
    FILE *arch = fopen(mv->nombreVMI, "wb");
    if (arch == NULL)
    {
        printf("Error al abrir el archivo de imagen\n");
        return;
    }
    else{
        //Header 
        // Identificador "VMI25"
        fwrite("VMI25", 1, 5, arch);
        int version = 1;
        fwrite(version, 1, 1, arch);
        int tam_kib = mv->memory_size/ 1024;
        fwrite(tam_kib, sizeof(int), 1, arch);
        // Registros 
        for (int i = 0; i < CANT_REGISTROS; i++) {
         reg = (int) mv->registros[i];
         fwrite(reg, sizeof(int), 1, arch);}

    for (int i = 0; i < CANT_SEGMENTOS; i++) {
        base = mv->tabla_segmentos[i].base;
        tam  = mv->tabla_segmentos[i].tamano;
        fwrite(base, sizeof(int), 1, arch);
        fwrite(tam,  sizeof(int), 1, arch);
    }

    // Memoria principal
    fwrite(mv->memoria, 1, mv->memory_size, arch);

    fclose(arch);}
   }
    
