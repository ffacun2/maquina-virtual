#include "operaciones.h"
#include "mv.h"
#include "disassembler.h"
#include <stdio.h>
#include <string.h>
/*
    Inicializa la máquina virtual, configurando los registros y la tabla de segmentos.
    Se establece el tamaño del segmento de datos y el segmento de código, así como la base
*/
void inicializar_maquina(t_MV *maquina, short int tamano)
{
    maquina->registros[CS] = 0x00000000;
    maquina->registros[DS] = 0x00010000;
    maquina->tabla_segmentos[(maquina->registros[CS] >> 16) & 0x0FFFF].base = 0;
    maquina->tabla_segmentos[(maquina->registros[CS] >> 16) & 0x0FFFF].tamano = tamano;
    maquina->tabla_segmentos[(maquina->registros[DS] >> 16) & 0x0FFFF].base = tamano;
    maquina->tabla_segmentos[(maquina->registros[DS] >> 16) & 0x0FFFF].tamano = TAMANO_MEMORIA - tamano;
    maquina->registros[IP] = maquina->registros[CS];
}

/*
    Inicia el proceso de lectura y ejecución de instrucciones de la máquina virtual.
    Se lee un array de instrucciones donde cada elemento tiene operacion y los
    operando 1 y 2 con sus respectivos tipo y valor. Este array es generado previamente.
*/
void ejecutar_maquina(t_MV *maquina, t_instruccion *instrucciones, int instruccion_size)
{
    printf("Ejecutando la maquina virtual...\n");

    for (int i = 0; i < instruccion_size; i++)
    {
        // operaciones(instruccion[i].opcode)(mv, instrucciones[i].op1, instrucciones[i].op2);
        if (instrucciones[i].op1.tipo == NINGUNO && instrucciones[i].op2.tipo == NINGUNO)
        {
            // instruccion con 0 operandos
        }
        else
        {
            if (instrucciones[i].op1.tipo == NINGUNO)
            {
                // instrucicon con 1 operando
            }
            else
            {
                MOV(maquina, instrucciones[i].op1, instrucciones[i].op2); // Ejemplo de instruccion con 2 operandos
                // instruccion con 2 operandos
            }
        }
    }
}

/*
    Leo el valor del operando de acuerdo a su tipo en el code segment.
    Memoria -> 3 bytes
    Inmediato -> 2 bytes
    Registro -> 1 byte
    Todo esto es parte del archivo leido, es del code segment.
*/
void valor_operacion(t_operador *op, t_MV mv)
{
    short index = (mv.registros[IP] >> 16) & 0x000000FF;      // Extraer el índice del segmento
    short offset = mv.registros[IP] & 0x0000FFFF;             // Extraer el offset
    short dirFisic = mv.tabla_segmentos[index].base + offset; // Calcular la dirección física
    op->valor = 0;                                            // Inicializar el valor del operando a 0
    switch (op->tipo)
    {
    case MEMORIA:
        op->valor = mv.memoria[dirFisic] & 0x0FF;
        op->valor <<= 8;
        op->valor |= mv.memoria[dirFisic + 1] & 0x0FF;
        op->valor <<= 8;
        op->valor |= mv.memoria[dirFisic + 2] & 0x0FF;
        break;
    case INMEDIATO:
        op->valor = mv.memoria[dirFisic] & 0x0FF;
        op->valor <<= 8;
        op->valor |= mv.memoria[dirFisic + 1] & 0x0FF;
        break;
    case REGISTRO:
        op->valor = mv.memoria[dirFisic] & 0x0FF;
        break;
    case NINGUNO:
        break;
    default:
        printf("Tipo de operando invalido. [valor_operacion()] Reg/Inm/Mem\n");
        break;
    }
}

/*
    Obtiene el valor de la maquina virutal segun el tipo de operando de la operacion.
    Si el tipo de operando/ sector de registro es invalido se muestra mensaje de error
    y devuelve valor -1.
*/
int getValor(t_operador op, t_MV maquina)
{
    short valor = 0;
    switch (op.tipo)
    {
    case REGISTRO:
    {
        // Extraer el sector del registro( EAX=00, AL=01, AH=10, AX=11)
        short sectorReg = (op.valor & 0x000C) >> 2;
        short codigoReg = (op.valor & 0x00F0) >> 4; // Extraer el registro
        switch (sectorReg)
        {
        case 0: // EAX XXXX
            return maquina.registros[codigoReg];
        case 1: // AL 000X
            return maquina.registros[codigoReg] & 0x000F;
        case 2: // AH 00X0
            return maquina.registros[codigoReg] & 0x00F0;
        case 3: // AX 00XX
            return maquina.registros[codigoReg] & 0x00FF;
        default:
            printf("Tipo de sector de registro invalido. [getValor()] EAX/AX/AL/AH\n");
            break;
        }
    }
    break;
    case INMEDIATO:
        return op.valor;
    case MEMORIA:
    {
        short codigoReg = (op.valor >> 4) & 0x000F;
        short offsetReg = maquina.registros[codigoReg] & 0x0FFFF;
        short offset = (op.valor >> 8) & 0x0FFFF;
        int dirFisic = maquina.tabla_segmentos[(maquina.registros[codigoReg] >> 16) & 0x0000FFFF].base + offsetReg + offset;

        if (dirFisic < maquina.tabla_segmentos[(maquina.registros[CS] >> 16) & 0x0FFFF].base || (dirFisic + 4) > maquina.tabla_segmentos[(maquina.registros[CS] >> 16) & 0x0FFFF].tamano)
            error(&maquina, 3); // Error: Overflow de memoria
        else
        {
            for (int i = 0; i < TAM_CELDA; i++)
            {
                valor = valor << 8;
                valor += maquina.memoria[dirFisic + 1] & 0x000000FF;
            }
        }
        return valor;
    }
    case NINGUNO:
        return 0;
    default:
        printf("Tipo de operando invalido.[getValor()] Reg/Inm/Mem\n");
        break;
    }

    return -1;
}

/*
    Establece el valor de un operando en la máquina virtual según su tipo.
    Dependiendo del tipo de operando (MEMORIA, REGISTRO), se actualiza el valor
    en la memoria o en los registros correspondientes.
*/
void setValor(t_operador op, int valor, t_MV *maquina)
{
    switch (op.tipo)
    {
    case MEMORIA:
    {
        short codReg = (op.valor >> 4) & 0x000F;
        short offsetReg = maquina->memoria[codReg] & 0x0FFFF;
        short offset = (op.valor >> 8) & 0x0FFFF;
        int dirFisic = maquina->tabla_segmentos[(maquina->registros[codReg] >> 16) & 0x0000FFFF].base + offsetReg + offset;

        if (dirFisic < maquina->tabla_segmentos[(maquina->registros[CS] >> 16) & 0x0FFFF].base || (dirFisic + 4) > maquina->tabla_segmentos[(maquina->registros[CS] >> 16) & 0x0FFFF].tamano)
            error(maquina, 3); // Error: Overflow de memoria
        else
            for (int i = 0; i < TAM_CELDA; i++)
            {
                maquina->memoria[dirFisic + i] = (valor >> ((3 - i) * 8)) & 0x000000FF;
            }
    }
    break;
    case REGISTRO:
    {
        short sectorReg = (op.valor & 0x000C) >> 2;
        short codigoReg = (op.valor & 0x00F0) >> 4;
        switch (sectorReg)
        {
        case 0: // EAX XXXX
            maquina->registros[codigoReg] = valor;
            break;
        case 1: // AL 000X
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x00F0) | (valor & 0x000F);
            break;
        case 2: // AH 00X0
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x000F) | (valor & 0x00F0);
            break;
        case 3: // AX 00XX
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0x00FF) | (valor & 0x00FF);
            break;
        default:
            printf("Tipo de sector de registro invalido. [setValor()] EAX/AX/AL/AH\n");
            break;
        }
    }
    break;
    default:
        break;
    }
}

/*
    Maneja los errores que pueden ocurrir durante la ejecución de la máquina virtual.
    Dependiendo del código de error, se muestra un mensaje específico y se ajusta el registro IP
    al final del codigo del code segment para finalizarlo.
*/
void error(t_MV *mv, int errorCode)
{
    switch (errorCode)
    {
    case 1:
        printf("Error: Operacion no valida.\n");
        break;
    case 2:
        printf("Error: Division por cero.\n");
        break;
    case 3:
        printf("Error: Overflow.\n");
        break;
    }
    mv->registros[IP] = (mv->registros[IP] & 0xF0000) + mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano;
}

/*
    Genero el array de instrucciones a partir del code segment de la memoria
    de la maquina virtual.
    Este array contiene por cada elemento, el opcode y los operandos (tipo y valor)
    para luego recorrerlo y ejecutar cada instruccion.
*/
void genero_array_instrucciones(t_MV *mv, t_instruccion **instrucciones, int *instruccion_size)
{
    // obtengo el tamaño del segmento de código
    short size_code = mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano - mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base;
    // Asigna memoria para las instrucciones
    *instrucciones = malloc(sizeof(t_operador) * size_code);

    if (*instrucciones == NULL)
    {
        printf("Error al asignar memoria para el disassembler\n");
        return;
    }
    else
    {
        short index = (mv->registros[IP] >> 16) & 0x0FFFF;         // Extraer el índice del segmento
        short offset = mv->registros[IP] & 0x0FFFF;                // Extraer el offset
        short dirFisic = mv->tabla_segmentos[index].base + offset; // Calcular la dirección física

        while (dirFisic < mv->tabla_segmentos[CS].tamano)
        {
            // Leer la instrucción de la memoria
            char instruccion = mv->memoria[dirFisic];
            mv->registros[IP]++;

            (*instrucciones)[*instruccion_size].opcode = instruccion & 0x0FF; // Guardar el opcode en la instrucción actual
            // Ejecutar la instrucción
            (*instrucciones)[*instruccion_size].op2.tipo = (instruccion >> 6) & 0x03; // Extraer el tipo de operando 2
            (*instrucciones)[*instruccion_size].op1.tipo = (instruccion >> 4) & 0x03; // Extraer el tipo de operando 1

            valor_operacion(&(*instrucciones)[*instruccion_size].op2, *mv); // Obtener el valor del operando 2 del code segment
            mv->registros[IP] += (*instrucciones)[*instruccion_size].op2.tipo;

            valor_operacion(&(*instrucciones)[*instruccion_size].op1, *mv); // Obtener el valor del operando 1 del code segment
            mv->registros[IP] += (*instrucciones)[*instruccion_size].op1.tipo;

            offset = mv->registros[IP] & 0x0FFFF;
            dirFisic = mv->tabla_segmentos[index].base + offset;
            *instruccion_size += 1; // Incrementar el tamaño de las instrucciones
        }
    }
}

void cargaAlta(t_operador op, short int valor, t_MV *maquina)
{
    switch (op.tipo)
    {
    // 1 registro = 4 bytes
    case REGISTRO:
        setValor(op, getValor(op, *maquina) | valor << 16, maquina);
        break;
    // 1 celda de memoria = 1 byte
    case MEMORIA:
        setValor(op, getValor(op, *maquina) | valor << 4, maquina);
        break;

    default:
        printf("Error: tipo de operando invalido");
        break;
    }
}

void cargaBaja(t_operador op, short int valor, t_MV *maquina)
{
    switch (op.tipo)
    {
    // 1 registro = 4 bytes
    case REGISTRO:
        setValor(op, getValor(op, *maquina) | valor, maquina);
        break;
    // 1 celda de memoria = 1 byte
    case MEMORIA:
        setValor(op, getValor(op, *maquina) | valor & 0xF, maquina);
        break;

    default:
        printf("Error: tipo de operando invalido");
        break;
    }
}