#include "operaciones.h"
#include "mv.h"
#include "disassembler.h"
#include <stdio.h>
#include <string.h>
/*
    Inicializa la máquina virtual, configurando los registros y la tabla de segmentos.
    Se establece el tamaño del segmento de datos y el segmento de código, así como la base
*/
void inicializar_maquina(t_MV *mv, short int tamano)
{
    mv->registros[CS] = 0x00000000;
    mv->registros[DS] = 0x00010000;
    mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base = 0;
    mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano = tamano;
    mv->tabla_segmentos[(mv->registros[DS] >> 16) & 0x0FFFF].base = tamano;
    mv->tabla_segmentos[(mv->registros[DS] >> 16) & 0x0FFFF].tamano = TAMANO_MEMORIA - tamano;
    mv->registros[IP] = mv->registros[CS];
}

/*
    Inicia el proceso de lectura y ejecución de instrucciones de la máquina virtual.
    Se lee un array de instrucciones donde cada elemento tiene operacion y los
    operando 1 y 2 con sus respectivos tipo y valor. Este array es generado previamente.
*/
void ejecutar_maquina(t_MV *mv, t_instruccion *instrucciones, int instruccion_size)
{
    t_func0 t_func0[1];
    t_func1 t_func1[10];
    t_func2 t_func2[15];
    int posicion = 0;

    inicializo_vector_op(t_func0, t_func1, t_func2); // Inicializa los vectores de funciones
    // printf("Ejecutando la maquina virtual...\n");

    mv->registros[IP] = mv->registros[CS]; // Inicializa el registro IP con la dirección del segmento de código
    while ((mv->registros[IP] & 0x0FFFF) < instruccion_size)
    {
        posicion = mv->registros[IP] & 0x0FFFF;                                                       // Actualiza la posición en el array de instrucciones
        mv->registros[IP] += instrucciones[posicion].op1.tipo + instrucciones[posicion].op2.tipo + 1; // Actualiza la posición en el array de instrucciones

        if (instrucciones[posicion].op1.tipo == NINGUNO && instrucciones[posicion].op2.tipo == NINGUNO)
            t_func0[(instrucciones[posicion].opcode & 0x01F) - 0x0F](mv);
        else if (instrucciones[posicion].op1.tipo == NINGUNO)
            t_func1[(instrucciones[posicion].opcode & 0x01F)](mv, instrucciones[posicion].op2);
        else
            t_func2[(instrucciones[posicion].opcode & 0x01F) - 16](mv, instrucciones[posicion].op1, instrucciones[posicion].op2);
    }
    for (int i = 0; i < 12; i++)
    {
        printf("[%d]: %02X\n", i, mv->memoria[mv->tabla_segmentos[1].base + i]);
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
        op->valor = (short)op->valor;
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
            return (maquina.registros[codigoReg] & 0x0FF);
        case 2: // AH 00X0
            return (maquina.registros[codigoReg] & 0x0FF00);
        case 3: // AX 00XX
            return (maquina.registros[codigoReg] & 0x00FF);
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
        short codigoReg = (op.valor >> 4) & 0x0F;
        short offsetReg = maquina.registros[codigoReg] & 0x0FFFF;
        short offset = (op.valor >> 8) & 0x0FFFF;
        int dirFisic = maquina.tabla_segmentos[(maquina.registros[codigoReg] >> 16) & 0x0000FFFF].base + offsetReg + offset;

        if ((dirFisic < maquina.tabla_segmentos[(maquina.registros[DS] >> 16) & 0x0FFFF].base) || ((dirFisic + 4) > maquina.tabla_segmentos[(maquina.registros[DS] >> 16) & 0x0FFFF].tamano))
            error(&maquina, 3); // Error: Overflow de memoria
        else
        {
            for (int i = 0; i < TAM_CELDA; i++)
            {
                valor <<= 8;
                valor |= (maquina.memoria[dirFisic + i] & 0x000000FF);
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
        short offsetReg = maquina->registros[codReg] & 0x0FFFF;
        short offset = (op.valor >> 8) & 0x0FFFF;
        int dirFisic = maquina->tabla_segmentos[(maquina->registros[codReg] >> 16) & 0x0FFFF].base + offsetReg + offset;

        if ((dirFisic < maquina->tabla_segmentos[(maquina->registros[DS] >> 16) & 0x0FFFF].base) || ((dirFisic + 4) > maquina->tabla_segmentos[(maquina->registros[DS] >> 16) & 0x0FFFF].tamano))
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
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0xFFFFFF00) | (valor & 0x000000FF);
            break;
        case 2: // AH 00X0
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0xFFFFFF00FF) | ((valor << 8) & 0x0000FF00);
            break;
        case 3: // AX 00XX
            maquina->registros[codigoReg] = (maquina->registros[codigoReg] & 0xFFFF0000) | (valor & 0x0000FFFF);
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
    exit(1);
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
    *instrucciones = malloc(sizeof(t_instruccion) * size_code);

    if (*instrucciones == NULL)
    {
        printf("Error al asignar memoria para el disassembler\n");
        return;
    }
    else
    {
        short index = (mv->registros[IP] >> 16) & 0x0FFFF;                                                                           // Extraer el índice del segmento
        short offset = mv->registros[IP] & 0x0FFFF;                                                                                  // Extraer el offset
        short dirFisic = mv->tabla_segmentos[index].base + offset;                                                                   // Calcular la dirección física
        *instruccion_size = mv->tabla_segmentos[mv->registros[CS] >> 16].tamano - mv->tabla_segmentos[mv->registros[CS] >> 16].base; // Tamaño del segmento de código

        while (dirFisic < *instruccion_size)
        {
            // Leer la instrucción de la memoria
            char instruccion = mv->memoria[dirFisic];

            int posicion = mv->registros[IP] & 0x0FFFF; // Obtener la posición de la instrucción en el array

            mv->registros[IP]++;

            (*instrucciones)[posicion].opcode = instruccion & 0x0FF; // Guardar el opcode en la instrucción actual
            // Ejecutar la instrucción
            (*instrucciones)[posicion].op2.tipo = (instruccion >> 6) & 0x03; // Extraer el tipo de operando 2
            (*instrucciones)[posicion].op1.tipo = (instruccion >> 4) & 0x03; // Extraer el tipo de operando 1

            valor_operacion(&(*instrucciones)[posicion].op2, *mv); // Obtener el valor del operando 2 del code segment
            mv->registros[IP] += (*instrucciones)[posicion].op2.tipo;

            valor_operacion(&(*instrucciones)[posicion].op1, *mv); // Obtener el valor del operando 1 del code segment
            mv->registros[IP] += (*instrucciones)[posicion].op1.tipo;

            offset = mv->registros[IP] & 0x0FFFF;
            dirFisic = mv->tabla_segmentos[index].base + offset;
        }
    }
}