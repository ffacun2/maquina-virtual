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
    maquina->registros[CS] = 0x00000000;
    maquina->registros[DS] = 0x00010000;
    maquina->registros[IP] = maquina->memoria[CS];
}

/*
    Inicia el proceso de lectura y ejecución de instrucciones de la máquina virtual.
    Se encarga de leer la instrucción actual de la memoria y ejecutarla.
*/
void ejecutar_maquina (t_MV *maquina) {
    while (maquina->registros[IP] < maquina->tabla_segmentos[CS].base + maquina->tabla_segmentos[CS].tamano) {
        // Leer la instrucción de la memoria
        int instruccion = maquina->memoria[maquina->tabla_segmentos[(maquina->registros[IP] >> 16) & 0x000000FF].base + (maquina->registros[IP] & 0x0000FFFF)];
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

    //Aca iria el disassembler creo
    //disassembler(op1,op2,instruccion); mando op1 y op2 para los valores y tipo, y intruccion para la operacion
    printf("t_op1: %d, t_op2: %d, valor op1:%06X, valor op2:%06X\n", op1.tipo, op2.tipo,op1.valor,op2.valor); // Debugging
}

/*
    Leo el valor del operando de acuerdo a su tipo en el code segment.
    Memoria -> 3 bytes
    Inmediato -> 2 bytes
    Registro -> 1 byte
*/
void valor_operacion (t_operador *op,t_MV mv) {
    short index = (mv.registros[IP] >> 16) & 0x000000FF; // Extraer el índice del segmento
    short offset = mv.registros[IP] & 0x0000FFFF; // Extraer el offset
    short dirFisic = mv.tabla_segmentos[index].base + offset; // Calcular la dirección física
    switch (op->tipo) {
        case MEMORIA:
            op->valor = mv.memoria[dirFisic] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[dirFisic + 1] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[dirFisic + 2] & 0x0FF;
            break;
        case INMEDIATO:
            op->valor = mv.memoria[dirFisic + offset] & 0x0FF;
            op->valor <<= 8;
            op->valor |= mv.memoria[dirFisic + 1] & 0x0FF;
            break;
        case REGISTRO:
            op->valor = mv.memoria[dirFisic + offset] & 0x0FF;
            break;
        default:
            printf("Tipo de operando invalido. [valor_operacion()] Reg/Inm/Mem");
            break;
    }

}

/*
    Obtiene el valor de la maquina virutal segun el tipo de operando de la operacion.
    Si el tipo de operando/ sector de registro es invalido se muestra mensaje de error 
    y devuelve valor -1.
*/
int getValor(t_operador op,t_MV maquina) {
    int valor = 0;
    switch (op.tipo) {
        case REGISTRO:
            // Extraer el sector del registro( EAX=00, AL=01, AH=10, AX=11)
            char sectorReg = (op.valor & 0x000C) >> 2; 
            char codigoReg = (op.valor & 0x00F0) >> 4; // Extraer el registro
            switch (sectorReg) {
                case 0: //EAX XXXX
                    return maquina.registros[codigoReg]; 
                case 1://AL 000X
                    return maquina.registros[codigoReg] & 0x000F;
                case 2://AH 00X0
                    return maquina.registros[codigoReg] & 0x00F0;
                case 3://AX 00XX
                    return maquina.registros[codigoReg] & 0x00FF;
                default:
                    printf("Tipo de sector de registro invalido. [getValor()] EAX/AX/AL/AH");
                    break;
            }
            break;
        case INMEDIATO:
            return op.valor;
        case MEMORIA:
            char codigoReg = (op.valor >> 4) & 0x000F;
            short int offset = (op.valor >> 8) & 0x00FF;
            int dirFisic = maquina.tabla_segmentos[(maquina.registros[codigoReg]>>16) & 0x000000FF].base + offset;
            for (int i = 0; i < 4; i++) {
                valor = valor << 8;
                valor += maquina.memoria[dirFisic + 1] & 0x000000FF;
            }
            return valor;
        default:
            printf("Tipo de operando invalido.[getValor()] Reg/Inm/Mem");
            break;
    }

    return -1;
}


void setValor(t_operador op,int valor,t_MV *maquina) {
    switch (op.tipo) {
    case MEMORIA:
        char codReg = (op.valor >> 4) & 0x000F;
        short int offset = (op.valor >> 8) & 0x00FF;
        int dirFisic = maquina->tabla_segmentos[(maquina->registros[codReg]>>16) & 0x000000FF].base + offset;
        
        //if ( /*Verifico que no se produzca Overflow*/ )

        for (int i = 0; i < 4; i++) {
            maquina->memoria[dirFisic + i] = (valor >> ( (3 - i)*8)) & 0x000000FF;
        }
        
        break;
    case REGISTRO:
        char sectorReg = (op.valor & 0x000C) >> 2;
        char codigoReg = (op.valor & 0x00F0) >> 4;
        switch (sectorReg) {
            case 0: //EAX XXXX
                maquina->registros[codigoReg] = valor; 
                break;
            case 1://AL 000X
                maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x00F0) | (valor & 0x000F);
                break;
            case 2://AH 00X0
                maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x000F) | (valor & 0x00F0);
                break;
            case 3://AX 00XX
                maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x00FF) | (valor & 0x00FF);
                break;
            default:
                printf("Tipo de sector de registro invalido. [setValor()] EAX/AX/AL/AH");
                break;
        }
        break;
    default:
        break;
    }
}

