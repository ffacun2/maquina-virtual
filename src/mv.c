#include "operaciones.h"
#include "mv.h"
#include <stdio.h>
/*
    Inicializa la máquina virtual, configurando los registros y la tabla de segmentos.
    Se establece el tamaño del segmento de datos y el segmento de código, así como la base
*/
void inicializar_maquina (t_MV *maquina,short int tamano) {
    maquina->tabla_segmentos[CS].base = 0;
    maquina->tabla_segmentos[CS].tamano = tamano;
    maquina->tabla_segmentos[DS].base = tamano;
    maquina->tabla_segmentos[DS].tamano = TAMANO_MEMORIA - tamano;
    maquina->registros[IP] = maquina->tabla_segmentos[CS].base;
}

/*
    Inicia el proceso de lectura y ejecución de instrucciones de la máquina virtual.
    Se encarga de leer la instrucción actual de la memoria y ejecutarla.
*/
void ejecutar_maquina (t_MV *maquina) {
    while (maquina->registros[IP] < maquina->tabla_segmentos[CS].base + maquina->tabla_segmentos[CS].tamano) {
        // Leer la instrucción de la memoria
        int instruccion = maquina->memoria[maquina->registros[IP]];
        maquina->registros[IP]++;
        // Ejecutar la instrucción
        ejecutar_instruccion(maquina, instruccion);
        
    }
}

/*
    Lee la instrucción actual de la memoria y ejecuta la operación correspondiente.
    Dependiendo del opcode, se determina el tipo de operación a realizar.
*/
void ejecutar_instruccion (t_MV *maquina,char instruccion){
    t_operador op1, op2;

    op2.tipo = (instruccion >> 6) & 0x03; // Extraer el tipo de operando 2
    op1.tipo = (instruccion >> 4) & 0x03; // Extraer el tipo de operando 1

    valor_operacion(&op2,*maquina); // Obtener el valor del operando 2
    maquina->registros[IP] += op2.tipo;

    valor_operacion(&op1,*maquina); // Obtener el valor del operando 1
    maquina->registros[IP] += op1.tipo;

    printf("t_op1: %d, t_op2: %d, valor op1:%06X, valor op2:%06X\n", op1.tipo, op2.tipo,op1.valor,op2.valor); // Debugging
}

/*
    Leo el valor del operando de acuerdo a su tipo.
    Memoria -> 3 bytes
    Inmediato -> 2 bytes
    Registro -> 1 byte
*/
void valor_operacion (t_operador *op,t_MV mv) {
    switch (op->tipo) {
        case MEMORIA:
            op->valor = mv.memoria[mv.registros[IP]] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[mv.registros[IP] + 1] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[mv.registros[IP] + 2] & 0x0FF;
            break;
        case INMEDIATO:
            op->valor = mv.memoria[mv.registros[IP]] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[mv.registros[IP] + 1] & 0x0FF;
            break;
        case REGISTRO:
            op->valor = mv.memoria[mv.registros[IP]] & 0x0FF;
            break;
        default:
            break;
    }
}