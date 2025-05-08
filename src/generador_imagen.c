#include "generador_imagen.h"
#include <stdint.h>
void generarImagen(t_MV *mv)
{
    int  descriptor;
    FILE *arch = fopen(mv->nombreVMI, "wb");
    if (arch == NULL)
    {
        printf("Error al abrir el archivo de imagen\n");
    }
    else{
        //Header 
        // Identificador "VMI25"
        fwrite("VMI25", 1, 5, arch);
        uint8_t version = 1;
        fwrite(&version, 1, 1, arch);
        uint16_t tam_kib =(uint16_t) mv->memory_size/ 1024;
        fwrite(&tam_kib, sizeof(int), 1, arch);
        // Registros 
        for (int i = 0; i < CANT_REGISTROS; i++) {
         fwrite(&mv->registros[i], sizeof(int), 1, arch);}

    for (int i = 0; i < CANT_SEGMENTOS; i++) {
        descriptor = (mv->tabla_segmentos[i].base << 16) | (mv->tabla_segmentos[i].tamano & 0xFFFF);
        fwrite(&descriptor, sizeof(int), 1, arch);
    }

    // Memoria principal
    fwrite(mv->memoria, 1, mv->memory_size, arch);

    fclose(arch);}
   }
    
