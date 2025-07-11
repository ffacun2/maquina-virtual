#include "disassembler.h"

char* identificarMnemonico(int codigo) {
    switch (codigo) {
        // Mnemónicos de un solo operando
    case 0:
        return "SYS";
    case 1:
        return "JMP";
    case 2:
        return "JZ";
    case 3:
        return "JP";
    case 4:
        return "JN";
    case 5:
        return "JNZ";
    case 6:
        return "JNP";
    case 7:
        return "JNN";
    case 8:
        return "NOT";
    case 0xB:
        return "PUSH";
    case 0xC:
        return "POP";
    case 0xD:
        return "CALL";

        // Mnemónicos sin operando
    case 0xE:
        return "RET";
    case 0xF:
        return "STOP";

        // Mnemónicos con 2 operandos
    case 0x10:
        return "MOV";
    case 0x11:
        return "ADD";
    case 0x12:
        return "SUB";
    case 0x13:
        return "SWAP";
    case 0x14:
        return "MUL";
    case 0x15:
        return "DIV";
    case 0x16:
        return "CMP";
    case 0x17:
        return "SHL";
    case 0x18:
        return "SHR";
    case 0x19:
        return "AND";
    case 0x1A:
        return "OR";
    case 0x1B:
        return "XOR";
    case 0x1C:
        return "LDL";
    case 0x1D:
        return "LDH";
    case 0x1E:
        return "RND";

    default:
        return "Mnemonico no identificado";
    }
}

char* identificarRegistro(int codigo) {
    switch (codigo) {
    case 0:
        return "CS";
    case 1:
        return "DS";
    case 2:
        return "ES";
    case 3:
        return "SS";
    case 4:
        return "KS";
    case 5:
        return "IP";
    case 6:
        return "SP";
    case 7:
        return "BP";
    case 8:
        return "CC";
    case 9:
        return "AC";
    case 10:
        return "EAX";
    case 11:
        return "EBX";
    case 12:
        return "ECX";
    case 13:
        return "EDX";
    case 14:
        return "EEX";
    case 15:
        return "EFX";
    default:
        return "Registro no identificado";
    }
}

void imprime_byte(int valor, int cant_byte) {
    for (int i = 0; i < cant_byte; i++) {
        printf("%02X ", (valor >> (8 * (cant_byte - i - 1))) & 0xFF);
    }
}

void imprimir_operador(t_operador operador) {
    switch (operador.tipo) {
    case REGISTRO:
    {
        switch ((operador.valor >> 2) & 0x03) {
        case 0:
            printf("%s", identificarRegistro((operador.valor >> 4) & 0x0F));
            break;
        case 1:
            printf("%cL", identificarRegistro((operador.valor >> 4) & 0x0F)[1]);
            break;
        case 2:
            printf("%cH", identificarRegistro((operador.valor >> 4) & 0x0F)[1]);
            break;
        case 3:
            printf("%cX", identificarRegistro((operador.valor >> 4) & 0x0F)[1]);
            break;
        default:
            break;
        }
    }
    break;
    case INMEDIATO:
        printf("%d", (short)operador.valor);
        break;
    case MEMORIA:
    {
        short valor = (operador.valor >> 8) & 0x0FFFF;
        short tamano = operador.valor & 0x3;
        switch (tamano) {
        case 0:
            printf("l");
            break;
        case 2:
            printf("w");
        case 3:
            printf("b");

        default:
            break;
        }
        if (valor > 0)
            printf("[%s + %d]", identificarRegistro((operador.valor >> 4) & 0x0F), valor);
        else if (valor == 0)
            printf("[%s]", identificarRegistro((operador.valor >> 4) & 0x0F));
        else
            printf("[%s%d]", identificarRegistro((operador.valor >> 4) & 0x0F), valor);
    }
    break;
    default:
        break;
    }
}

void escribirDisassembler(t_MV mv, t_instruccion *t_instrucciones, int tamano) {
    if (mv.registros[KS] != -1)
        escribirConstantes(mv);

    escribirInstrucciones(mv,t_instrucciones,tamano);
}

void escribirInstrucciones(t_MV mv,t_instruccion* instrucciones, int tamano) {
    int base = mv.tabla_segmentos[(mv.registros[CS] >> 16) & 0x0FFFF].base;
    int posicion = 0;
    while (posicion < tamano) {
        
        if(mv.version == 2 )
            if ( mv.offsetEntryPoint == posicion)
                printf(">");
            else
                printf(" ");

        printf("[%04X] %02X ", base+posicion, instrucciones[posicion].opcode & 0x0FF);
        imprime_byte(instrucciones[posicion].op2.valor, instrucciones[posicion].op2.tipo);
        imprime_byte(instrucciones[posicion].op1.valor, instrucciones[posicion].op1.tipo);
        for (int j = 0; j < (7 - instrucciones[posicion].op1.tipo - instrucciones[posicion].op2.tipo); j++) {
            printf("  ");
        }
        printf("\t| %s ", identificarMnemonico(instrucciones[posicion].opcode & 0x01F));
        imprimir_operador(instrucciones[posicion].op1);
        if (instrucciones[posicion].op1.tipo != NINGUNO)
            printf(", ");
        imprimir_operador(instrucciones[posicion].op2);
        printf("\n");
        posicion += instrucciones[posicion].op1.tipo + instrucciones[posicion].op2.tipo + 1;
    }
}

void escribirConstantes (t_MV mv) {
    int index = (mv.registros[KS] >> 16) & 0x0FFFF;
    int base = mv.tabla_segmentos[index].base;
    int dirFisica = mv.tabla_segmentos[index].base;
    int posicion;

    while (dirFisica < (base + mv.tabla_segmentos[index].tamano) ){
        posicion = dirFisica;
        printf(" [%04X]",posicion);
        while (dirFisica < (base + mv.tabla_segmentos[index].tamano) && mv.memoria[dirFisica] != '\0') {
            printf("%02X ",mv.memoria[dirFisica]);
            dirFisica++;
        }
        dirFisica++;
        printf("00\t | \"%s\"\n",&mv.memoria[posicion]);
    }
}