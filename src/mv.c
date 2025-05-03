#include "operaciones.h"
#include "mv.h"
#include "disassembler.h"
#include <stdio.h>
#include <string.h>
#define null -1
/*
    Inicializa la máquina virtual, configurando los registros y la tabla de segmentos.
    Se establece el tamaño del segmento de datos y el segmento de código, así como la base
*/

// Version 1
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

// Version 2
void inicializar_maquina2(t_MV *mv, short int tamanoCS, short int tamanoDS, short int tamanoES, short int tamanoSS, short int tamanoKS, short int offsetEntryPoint, int memoria)
{
    int i, acumulador = 0;
    int b = 0;
    short V[] = {0, tamanoKS, tamanoCS, tamanoDS, tamanoES, tamanoSS};
    int seg[] = {KS, CS, DS, ES, SS};

    for (i = 0; i < CANT_SEGMENTOS; i++)
    {
        mv->tabla_segmentos[i].base = acumulador;
    }

    // Tamaño del Param Segment
    short int tamanoPS = memoria;
    for (i = 1; i < 6; i++)
    {
        if (V[i] > 0)
        {
            tamanoPS -= V[i];
        }
    }
    V[0] = tamanoPS;

    // Tabla de segmentos
    for (i = 0; i < 6; i--)
    {
        if (V[i] <= 0)
        {
            mv->tabla_segmentos[i].base = mv->tabla_segmentos[i].tamano = null;
            if (i > 0)
            {
                mv->registros[seg[i - 1]] = null;
            }
        }
        else
        {
            mv->tabla_segmentos[i].base = b;
            if (i > 0)
            {
                mv->registros[seg[i - 1]] = b << 16;
            }
            b++;
            mv->tabla_segmentos[i].tamano = V[i];
        }
    }

    mv->registros[IP] = mv->registros[CS] + offsetEntryPoint;
}

void inicializo_registros(t_MV *mv, int registros[])
{
    for (int i = 0; i < CANT_REGISTROS; i++)
    {
        mv->registros[i] = registros[i];
    }
}

void inicializo_segmentos(t_MV *mv, int segmentos[])
{
    for (int i = 0; i < CANT_SEGMENTOS; i++)
    {
        mv->tabla_segmentos[i].base = (segmentos[i] >> 16) & 0x0000FFFF;
        mv->tabla_segmentos[i].tamano = segmentos[i] & 0x0000FFFF;
    }
}

void inicializo_memoria(t_MV *mv, char memoria[], int size)
{
    for (int i = 0; i < size; i++)
    {
        mv->memoria[i] = memoria[i];
    }
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
    mv->registros[IP] = mv->registros[CS];           // Inicializa el registro IP con la dirección del segmento de código
    mv->flag_ejecucion = 1;

    while (mv->flag_ejecucion && mv->registros[IP] < instruccion_size)
    {
        posicion = mv->registros[IP] & 0x0FFFF;
        mv->registros[IP] += instrucciones[posicion].op1.tipo + instrucciones[posicion].op2.tipo + 1; // Actualiza la posición en el array de instrucciones

        codOperacionValido((instrucciones[posicion].opcode & 0x01F), *mv);
        if (instrucciones[posicion].op1.tipo == NINGUNO && instrucciones[posicion].op2.tipo == NINGUNO)
            t_func0[(instrucciones[posicion].opcode & 0x01F) - 0x0F](mv);
        else if (instrucciones[posicion].op1.tipo == NINGUNO)
            t_func1[(instrucciones[posicion].opcode & 0x01F)](mv, instrucciones[posicion].op2);
        else
            t_func2[(instrucciones[posicion].opcode & 0x01F) - 16](mv, instrucciones[posicion].op1, instrucciones[posicion].op2);
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
    int valor = 0;
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
            valor = (maquina.registros[codigoReg] & 0x0FF) << 24;
            return valor >> 24;
        case 2: // AH 00X0
            valor = (maquina.registros[codigoReg] & 0x0FF00) << 16;
            return valor >> 24;
        case 3: // AX 00XX
            valor = (maquina.registros[codigoReg] & 0x00FFFF) << 16;
            return valor >> 16;
        default:
            printf("op:%d valor:%06X\n", op.tipo, op.valor);
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
        short data_size = 4 - op.valor & 0x03; // Extraer el tamaño de los datos (0, 1, 2 o 3 bytes)
        int dirFisic = maquina.tabla_segmentos[(maquina.registros[codigoReg] >> 16) & 0x0000FFFF].base + offsetReg + offset;

        if ((dirFisic < maquina.tabla_segmentos[(maquina.registros[DS] >> 16) & 0x0FFFF].base) || ((dirFisic + 4) > maquina.tabla_segmentos[(maquina.registros[DS] >> 16) & 0x0FFFF].tamano))
            error(&maquina, 3); // Error: Overflow de memoria
        else
        {
            for (int i = 0; i < data_size; i++)
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
    }
}

/*
    Verifica si el codigo de operacion se encuentra dentro de los disponibles
    En caso de que no se encuentre se lanza -> Error: Operacion no valida y se corta la ejecucion
*/
void codOperacionValido(int cod_op, t_MV mv)
{
    if (!((cod_op >= 0x10 && cod_op <= 0x1E) || (cod_op >= 0x00 && cod_op <= 0x08) || cod_op == 0x0F))
        error(&mv, 1);
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
        printf("Error: Falla de segmento.\n");
        break;
    case 4:
        printf("Error: Memoria insuficiente\n");
        break;
    case 5:
        printf("Error: Stack overflow\n");
        break;
    case 6:
        printf("Error: Stack underflow\n");
    }
    mv->flag_ejecucion = 0;
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