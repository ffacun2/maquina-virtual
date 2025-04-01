#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANT_REGISTROS 16
#define TAMANO_MEMORIA 16384
#define CANT_SEGMENTOS 8
#define IDENTIFICADOR "VMX25\0"
#define VERSION_MV 1


enum {
    // Data Segment
    DS = 0,
    //Code Segment
    CS = 1,
    //Instruction Pointer
    IP = 5,
    // condition code
    CC = 8,
    //acumulador
    AC = 9,
    // registros de proposito general EAX ...
    A = 10,
    B = 11,
    C = 12,
    D = 13,
    E = 14,
    F = 15
};

typedef struct {
    short int base;
    short int tamano;
}t_tabla_segmento;

typedef struct MV{
    char memoria[TAMANO_MEMORIA];
    int registros[CANT_REGISTROS];
    t_tabla_segmento tabla_segmentos[CANT_SEGMENTOS];
}t_MV;



void inicializar_maquina(t_MV *MV);