
#ifndef MV_H_INCLUDED
#define MV_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANT_REGISTROS 16
#define TAMANO_MEMORIA 16384
#define CANT_SEGMENTOS 8
#define IDENTIFICADOR "VMX25\0"
#define VERSION_MV1 1
#define VERSION_MV2 2
#define TAM_CELDA 4

enum {
    // Segmentos

    // Code Segment
    CS = 0,
    // Data Segment
    DS = 1,
    // Extra Segment
    ES = 2,
    // Stack Segment
    SS = 3,
    // Const Segmet
    KS = 4,

    // Instruction Pointer
    IP = 5,

    // Pila de ejecución

    // Stack Pointer
    SP = 6,
    // Base Pointer
    BP = 7,

    // condition code
    CC = 8,

    // acumulador
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
} t_tabla_segmento;

typedef struct MV {
    char memoria[TAMANO_MEMORIA];
    int registros[CANT_REGISTROS];
    t_tabla_segmento tabla_segmentos[CANT_SEGMENTOS];
    int flag_ejecucion;
    const char* nombreVMI;
    const char* nombreVMX;
    int flag_d;
    int memory_size;
    int offsetEntryPoint;
} t_MV;

typedef enum {
    NINGUNO = 0,
    REGISTRO = 1,
    INMEDIATO = 2,
    MEMORIA = 3
} t_operando;

typedef struct {
    int valor;
    t_operando tipo;
} t_operador;

typedef struct {
    char opcode;
    t_operador op1;
    t_operador op2;
} t_instruccion;

void inicializar_maquina(t_MV* MV, short int tamano);
void inicializar_maquina2(t_MV* mv, short int tamanoCS, short int tamanoDS, short int tamanoES, short int tamanoSS, short int tamanoKS, short int offsetEntryPoint);
void inicializo_registros(t_MV* mv, int registros[]);
void inicializo_segmentos(t_MV* mv, int segmentos[]);
void inicializo_memoria(t_MV* mv, char memoria[], int size);

void ejecutar_maquina(t_MV* maquina, t_instruccion*, int instruccion_size);
void valor_operacion(t_operador* op, t_MV mv);
int getValor(t_operador op, t_MV mv);
void setValor(t_operador op, int valor, t_MV* maquina);
void codOperacionValido(int cod_op, t_MV mv);
void error(t_MV* mv, int errorCode);
void genero_array_instrucciones(t_MV* mv, t_instruccion** instrucciones, int* instruccion_size);

#endif // MV_H_INCLUDED