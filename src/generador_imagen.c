#include "generador_imagen.h"
#include <stdint.h>
void escribir_uint16_be(FILE *f, uint16_t valor) {
    uint8_t bytes[2];
    bytes[0] = (valor >> 8) & 0xFF;
    bytes[1] = valor & 0xFF;
    fwrite(bytes, 1, 2, f);
}
void escribir_uint32_be(FILE *f, uint32_t valor) {
    uint8_t bytes[4];
    bytes[0] = (valor >> 24) & 0xFF;
    bytes[1] = (valor >> 16) & 0xFF;
    bytes[2] = (valor >> 8) & 0xFF;
    bytes[3] = valor & 0xFF;
    fwrite(bytes, 1, 4, f);
}
void generarImagen(t_MV *mv)
{
    uint32_t  descriptor;
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
        escribir_uint16_be(arch, tam_kib); // Tamaño de memoria en Kbytes
        // Registros 
        for (int i = 0; i < CANT_REGISTROS; i++) {
         escribir_uint32_be(arch, (uint32_t)mv->registros[i]);

    for (int i = 0; i < CANT_SEGMENTOS; i++) {
        descriptor = (mv->tabla_segmentos[i].base << 16) | (mv->tabla_segmentos[i].tamano & 0xFFFF);
        escribir_uint32_be(arch, descriptor);
    }

    // Memoria principal
    fwrite(mv->memoria, 1, mv->memory_size, arch);

    fclose(arch);}
   }

    
/*
    Leo el archivo vmi, primero leo el header para verificar el identificador y la version
    y luego los datos que iran a la memoria de la maquina virtual. El archivos debe contener
    todos los datos necesarios para inicializar la maquina virtual. VMI es una imagen de la
    maquina virtual en un estado determinado.
    @param mv: puntero a la maquina virtual
    @return: 1 si la lectura fue exitosa, 0 si hubo un error.
*/
int lectura_vmi(t_MV* mv) {
    short high, low;
    short tamano;
    char modelo[6];
    char version;
    FILE* archivo = fopen(mv->nombreVMI, "rb");
    int registros[16];
    int segmentos[8];

    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return 0;
    }

    fread(modelo, sizeof(char), 5, archivo); // Leo el header del archivo
    modelo[5] = '\0';                        // Aseguro que el header sea una cadena de caracteres

    fread(&version, sizeof(char), 1, archivo); // Leo la version del archivo

    if (strcmp(modelo, "VMI25") != 0 || version != 1) {
        fclose(archivo);
        return 0;
    }
    else {
        fread(&high, sizeof(char), 1, archivo); // Leo el byte alto del tamaño de datos
        fread(&low, sizeof(char), 1, archivo);  // Leo el byte bajo del tamaño de datos
        tamano = ((high << 8) | low);           // armo el tamaño de datos
        tamano *= 1024;                         // Multiplico por 1024 para pasarlo a bytes

        fread(registros, sizeof(int), 16, archivo); // Leo los registros del archivo
        // inicializar_registros(mv, registros); // Inicializo los registros de la máquina virtual
        fread(segmentos, sizeof(int), 8, archivo); // Leo los segmentos del archivo
        // inicializar_segmentos(mv, segmentos); // Inicializo los segmentos de la máquina virtual
        char memoria[tamano];
        fread(memoria, sizeof(char), tamano, archivo); // Leo los datos del archivo
        // inicializar_memoria(mv, memoria, tamano); // Inicializo la memoria de la máquina virtual
    }
    fclose(archivo);
    return 1;
}
