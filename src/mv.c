#include "operaciones.h"
#include "mv.h"
#include "disassembler.h"

/*
    Inicializa la máquina virtual, configurando los registros y la tabla de segmentos.
    Se establece el tamaño del segmento de datos y el segmento de código, así como la base.A
    Este metodo es utilizado para inicializar la máquina virtual version 1.
    @param mv: puntero a la máquina virtual
    @param tamano: tamaño del segmento de codigo
*/
void inicializar_maquina(t_MV* mv, short int tamano) {
    mv->registros[CS] = 0x00000000;
    mv->registros[DS] = 0x00010000;
    mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base = 0;
    mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano = tamano;
    mv->tabla_segmentos[(mv->registros[DS] >> 16) & 0x0FFFF].base = tamano;
    mv->tabla_segmentos[(mv->registros[DS] >> 16) & 0x0FFFF].tamano = TAMANO_MEMORIA - tamano;
    mv->registros[IP] = mv->registros[CS];
    mv->offsetEntryPoint = 0;
}

/*
    Inicializa la máquina virtual con los tamaños de segmentos, los registros y el punto de entrada.
    Este metodo es utilizado para inicializar la máquina virtual version 2.
    @param mv: puntero a la máquina virtual
    @param segmentos_size: array de tamaños de segmentos [PS, KS, CS, DS, ES, SS]
    @param empty_point: offset del CS hasta el main del codigo assembler
*/
void inicializar_maquina2(t_MV* mv, short segmentos_size[], int entry_point, int total_memoria, int ptro_vec_param, int cant_param) {
    int seg = 0;
    int base = 0;
    int reg[] = { KS, CS, DS, ES, SS };

    for (int i = 0; i < 6; i++) {
        if (segmentos_size[i] > 0) {
            mv->tabla_segmentos[seg].base = base;
            mv->tabla_segmentos[seg].tamano = segmentos_size[i];
            if (i > 0) {
                mv->registros[reg[i - 1]] = seg << 16;
            }
            base += segmentos_size[i];
            seg++;
        }
        else {
            if (i > 0) {
                mv->registros[reg[i - 1]] = null;
            }
        }
    }
    mv->registros[IP] = mv->registros[CS] + entry_point;
    mv->registros[SP] = mv->registros[SS] + segmentos_size[5];
    mv->offsetEntryPoint = entry_point;
    mv->memory_size = total_memoria;

    if (cant_param > 0) {
        // Guardar la dirección de los parámetros en la pila
        pushValor(mv, ptro_vec_param);
        // Guardar la cantidad de  parámetros en la pila
        pushValor(mv, cant_param);
        // representa el RET del main que seria posicion fuera del code segment
        pushValor(mv, -1);
    }
    else {
        // Guardar la dirección de los parámetros en la pila
        pushValor(mv, -1);
        // Guardar la cantidad de  parámetros en la pila
        pushValor(mv, 0);
        // representa el RET del main que seria posicion fuera del code segment
        pushValor(mv, -1);
    }
}

/*
    Inicializa los registros de la máquina virtual con los datos
    proporcionados por el archivo vmi.
    @param mv: puntero a la máquina virtual
    @param registros: array con todos los valores de los registros de la mv
*/
void inicializo_registros(t_MV* mv, int registros[]) {
    for (int i = 0; i < CANT_REGISTROS; i++) {
        mv->registros[i] = registros[i];
    }
}

/*
    Inicializa la tabla de segmentos de la máquina virtual con los datos
    proporcionados por el archivo vmi.
    @param mv: puntero a la máquina virtual
    @param segmentos: array con todos los valores de los segmentos de la mv
*/
void inicializo_segmentos(t_MV* mv, int segmentos[]) {
    for (int i = 0; i < CANT_SEGMENTOS; i++) {
        mv->tabla_segmentos[i].base = (segmentos[i] >> 16) & 0x0000FFFF;
        mv->tabla_segmentos[i].tamano = segmentos[i] & 0x0000FFFF;
    }
}

/*
    Inicializa la memoria de la máquina virtual con los datos proporcionados
    por el archivo vmi.
    @param mv: puntero a la máquina virtual
    @param memoria: array con todos los valores de la memoria de la mv
    @param size: tamaño de la memoria
*/
void inicializo_memoria(t_MV* mv, char memoria[], int size) {
    for (int i = 0; i < size; i++) {
        mv->memoria[i] = memoria[i];
    }
}

/*
    Carga el segmento de parámetros en la memoria de la máquina virtual.
    Se utiliza para inicializar el segmento de parámetros con los datos
    proporcionados en la consola de comando.
    @param mv: puntero a la máquina virtual
    @param param: array de strings que representan los parámetros
    @param size: tamaño del array de parámetros
*/
void cargoParamSegment(t_MV* mv, char** param, int cant_param, int* ptro_vec_param, int* size_param_segment) {
    int posicionamiento = 0;
    int vec_param[cant_param];

    // Vector de punteros a los parametros, guardo el offset al primer elemento 0x0000 0xxx
    *ptro_vec_param = 0x00000000;

    // Guardo en memoria los parametros
    for (int i = 0; i < cant_param; i++) {
        vec_param[i] = posicionamiento;
        for (int j = 0; j < strlen(param[i]); j++) {
            mv->memoria[posicionamiento] = param[i][j];
            posicionamiento++;
        }
        mv->memoria[posicionamiento] = '\0'; // Agregar el terminador de cadena
        posicionamiento++;
    }
    *ptro_vec_param += posicionamiento;
    // Guardo en memoria la direccion de cada parametro
    for (int i = 0; i < cant_param; i++) {
        mv->memoria[posicionamiento] = 0x00;
        mv->memoria[posicionamiento + 1] = 0x00;
        mv->memoria[posicionamiento + 2] = (vec_param[i] >> 16) & 0x0FF;
        mv->memoria[posicionamiento + 3] = (vec_param[i] & 0x0FF);
        posicionamiento += 4;
    }

    // retorno el tamaño del segmento de parametros
    *size_param_segment = posicionamiento;
}

/*
    Carga el segmento de constantes en la memoria de la máquina virtual.
    Se utiliza para inicializar en memoria en la posicion del segmento de constantes
    con los datos proporcionados en el archivo vmx.
    @param mv: puntero a la máquina virtual
    @param constant: array de constantes a cargar
    @param size: tamaño del array de constantes
*/
void cargoConstSegment(t_MV* mv, char constant[], int size) {
    if (size == 0) {
        return; // Si el tamaño es cero, no hay nada que cargar
    }
    int base = mv->tabla_segmentos[(mv->registros[KS] >> 16) & 0x0FFFF].base;
    for (int i = base; i < (base + size); i++) {
        mv->memoria[i] = constant[i - base];
    }
}

/*
    Carga el segmento de código en la memoria de la máquina virtual.
    Se utiliza para inicializar en memoria en la posicion del  segmento
    de código con los datos proporcionados en el archivo vmx.
    @param mv: puntero a la máquina virtual
    @param code: array de código a cargar
    @param size: tamaño del array de código
*/
void cargoCodeSegment(t_MV* mv, char code[], int size) {
    if (size == 0) {
        return; // Si el tamaño es cero, no hay nada que cargar
    }
    int base = mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base;
    for (int i = base; i < (base + size); i++) {
        mv->memoria[i] = code[i - base];
    }
}

/*
    Inicia el proceso de lectura y ejecución de instrucciones de la máquina virtual.
    Se lee un array de instrucciones donde cada elemento tiene operacion y los
    operando 1 y 2 con sus respectivos tipo y valor. Este array es generado previamente.
    @param mv: puntero a la máquina virtual
    @param instrucciones: array de instrucciones a ejecutar
    @param instruccion_size: tamaño del array de instrucciones
*/
void ejecutar_maquina(t_MV* mv, t_instruccion* instrucciones, int instruccion_size) {
    t_func0 t_func0[2];
    t_func1 t_func1[14];
    t_func2 t_func2[15];
    int posicion = 0;

    // Para llamar a breakpoint
    t_operador op;
    op.tipo = INMEDIATO;
    op.valor = 0x0F;

    // Inicializa los vectores de funciones
    inicializo_vector_op(t_func0, t_func1, t_func2);
    // Inicializa el registro IP con la dirección del segmento de código
    mv->registros[IP] = mv->registros[CS] + mv->offsetEntryPoint;

    while (mv->flag_ejecucion && ((mv->registros[IP] & 0x0FFFF) < instruccion_size)) {
        posicion = mv->registros[IP] & 0x0FFFF;
        mv->registros[IP] += instrucciones[posicion].op1.tipo + instrucciones[posicion].op2.tipo + 1; // Actualiza la posición en el array de instrucciones

        if (codOperacionValido((instrucciones[posicion].opcode & 0x01F), *mv))
            if (instrucciones[posicion].op1.tipo == NINGUNO && instrucciones[posicion].op2.tipo == NINGUNO)
                t_func0[0x0F - (instrucciones[posicion].opcode & 0x01F)](mv);
            else if (instrucciones[posicion].op1.tipo == NINGUNO)
                t_func1[(instrucciones[posicion].opcode & 0x01F)](mv, instrucciones[posicion].op2);
            else
                t_func2[(instrucciones[posicion].opcode & 0x01F) - 16](mv, instrucciones[posicion].op1, instrucciones[posicion].op2);
        // Breakpoint
        if (mv->flag_breakpoint && instrucciones[posicion].op1.valor != 0x0F && (instrucciones[posicion].opcode & 0x1F)!= 0x0F) {
            SYS(mv, op);
        }

    }
}

/*
    Formo el valor del operando de acuerdo a su tipo en el code segment.
    Memoria -> 3 bytes
    Inmediato -> 2 bytes
    Registro -> 1 byte
    Todo esto es parte del archivo leido, es del code segment.
*/
void valor_operacion(t_operador* op, t_MV mv) {
    short index = (mv.registros[IP] >> 16) & 0x000000FF;      // Extraer el índice del segmento
    short offset = mv.registros[IP] & 0x0000FFFF;             // Extraer el offset
    short dirFisic = mv.tabla_segmentos[index].base + offset; // Calcular la dirección física
    op->valor = 0;                                            // Inicializar el valor del operando a 0
    switch (op->tipo) {
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
    @param op: operando a obtener
    @param maquina: puntero a la máquina virtual
    @return: valor del operando
*/
int getValor(t_operador op, t_MV maquina) {
    int valor = 0;
    switch (op.tipo) {
    case REGISTRO:
    {
        // Extraer el sector del registro( EAX=00, AL=01, AH=10, AX=11)
        short sectorReg = (op.valor & 0x000C) >> 2;
        short codigoReg = (op.valor & 0x00F0) >> 4; // Extraer el registro

        switch (sectorReg) {
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
        short data_size = 4 - (maquina.version == 2) * (op.valor & 0x03); // Extraer el tamaño de los datos (0, 1, 2 o 3 bytes)
        int base = maquina.tabla_segmentos[(maquina.registros[codigoReg] >> 16) & 0x0FFFF].base;
        int tam = maquina.tabla_segmentos[(maquina.registros[codigoReg] >> 16) & 0x0FFFF].tamano;
        int dirFisic = base + offsetReg + offset;

        if ((dirFisic < base) || ((dirFisic + data_size - 1) > (base + tam))) {
            error(&maquina, 3); // Error: Overflow de memoria
        }
        else {
            for (int i = 0; i < data_size; i++) {
                valor <<= 8;
                valor |= (maquina.memoria[dirFisic + i] & 0x0FF);
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
    @param op: operando a establecer
    @param valor: valor a establecer
    @param maquina: puntero a la máquina virtual
*/
void setValor(t_operador op, int valor, t_MV* maquina) {
    switch (op.tipo) {
    case MEMORIA:
    {
        short codReg = (op.valor >> 4) & 0x000F;
        short offsetReg = maquina->registros[codReg] & 0x0FFFF;
        short offset = (op.valor >> 8) & 0x0FFFF;
        int base = maquina->tabla_segmentos[(maquina->registros[codReg] >> 16) & 0x0FFFF].base;
        int tam = maquina->tabla_segmentos[(maquina->registros[codReg] >> 16) & 0x0FFFF].tamano;
        int dirFisic = base + offsetReg + offset;
        int tamano = TAM_CELDA - (maquina->version == 2) * (op.valor & 3);
        if ((dirFisic < base) || ((dirFisic + tamano - 1) > base + tam))
            error(maquina, 3); // Error: Overflow de memoria
        else
            for (int i = 0; i < tamano; i++) {
                maquina->memoria[dirFisic + i] = (valor >> ((tamano - 1 - i) * 8)) & 0x000000FF;
            }
    }
    break;
    case REGISTRO:
    {
        short sectorReg = (op.valor & 0x000C) >> 2;
        short codigoReg = (op.valor & 0x00F0) >> 4;
        switch (sectorReg) {
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
    Se tiene en cuenta la version de la maquina virtual para determinar los codigos de operacion validos.
    @param cod_op: codigo de operacion a verificar
    @param mv: puntero a la maquina virtual
    @return: 1 si el codigo de operacion es valido, 0 si no lo es
*/
int codOperacionValido(int cod_op, t_MV mv) {
    if (!((cod_op >= 0x10 && cod_op <= 0x1E) || (cod_op >= 0x00 && cod_op <= 0x08) || cod_op == 0x0F) && mv.version == 1) {
        error(&mv, 1);
        return 0;
    }
    if (!((cod_op >= 0x10 && cod_op <= 0x1E) || (cod_op >= 0x00 && cod_op <= 0x08) || (cod_op >= 0x0B && cod_op <= 0x0D) || (cod_op >= 0x0E && cod_op <= 0x0F)) && mv.version == 2) {
        error(&mv, 1);
        return 0;
    }
    return 1;
}

/*
    Maneja los errores que pueden ocurrir durante la ejecución de la máquina virtual.
    Dependiendo del código de error, se muestra un mensaje específico y se detiene la ejecución.
    1: Operacion no valida
    2: Division por cero
    3: Falla de segmento
    4: Memoria insuficiente
    5: Stack overflow
    6: Stack underflow
    @param mv: puntero a la máquina virtual
    @param errorCode: código de error que indica el tipo de error
*/
void error(t_MV* mv, int errorCode) {
    switch (errorCode) {
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
    El tamaño del array de instrucciones es el tamaño del segmento de código. Esto
    nos permite movernos en el a traves de la IP.
    @param mv: puntero a la maquina virtual
    @param instrucciones: puntero a un array de instrucciones
    @param instruccion_size: puntero a un entero que contiene el tamaño del array de instrucciones
*/
void genero_array_instrucciones(t_MV* mv, t_instruccion** instrucciones, int* instruccion_size) {
    // obtengo el tamaño del segmento de código
    unsigned short size_code = mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano;
    // Asigna memoria para las instrucciones
    *instrucciones = malloc(sizeof(t_instruccion) * size_code);

    if (*instrucciones == NULL) {
        printf("Error al asignar memoria para el disassembler\n");
        return;
    }
    else {
        short index = (mv->registros[IP] >> 16) & 0x0FFFF; // Extraer el índice del segmento
        short offset;
        short dirFisic = mv->tabla_segmentos[index].base; // Calcular la dirección física
        *instruccion_size = size_code;                    // Tamaño del segmento de código

        mv->registros[IP] = mv->registros[IP] & 0xFFFF0000;
        while (dirFisic < (mv->tabla_segmentos[index].base + mv->tabla_segmentos[index].tamano)) {
            // Leer la instrucción de la memoria
            char instruccion = mv->memoria[dirFisic];

            int posicion = mv->registros[IP] & 0x0FFFF; // Obtener la posición de la instrucción en el array

            mv->registros[IP]++;

            // Guardar el opcode en la instrucción actual
            (*instrucciones)[posicion].opcode = instruccion & 0x0FF;
            // Ejecutar la instrucción
            (*instrucciones)[posicion].op2.tipo = (instruccion >> 6) & 0x03; // Extraer el tipo de operando 2
            (*instrucciones)[posicion].op1.tipo = (instruccion >> 4) & 0x03; // Extraer el tipo de operando 1

            // Obtener el valor del operando 2 del code segment
            valor_operacion(&(*instrucciones)[posicion].op2, *mv);
            mv->registros[IP] += (*instrucciones)[posicion].op2.tipo;

            // Obtener el valor del operando 1 del code segment
            valor_operacion(&(*instrucciones)[posicion].op1, *mv);
            mv->registros[IP] += (*instrucciones)[posicion].op1.tipo;

            offset = mv->registros[IP] & 0x0FFFF;
            dirFisic = mv->tabla_segmentos[index].base + offset;
        }
    }
}