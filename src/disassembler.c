#include "disassembler.h"

char *identificarMnemonico(int codigo){
    switch (codigo)
    {
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

    case 0xF:
        return "STOP";

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

char *indentificarRegistro(int codigo) {
    switch (codigo) {
        case 0:
            return "CS";
        case 1:
            return "DS";
        case 5:
            return "IP";
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

void imprime_byte (int valor, int cant_byte){
    for (int i = 0; i < cant_byte; i++) {
        printf("%02X ", (valor >> (8 * (cant_byte - i - 1))) & 0xFF);
    }
}

void imprimir_operador(t_operador operador) {
    switch (operador.tipo) {
        case REGISTRO: {
            switch ((operador.valor >> 2) & 0x03) {
                case 0:
                    printf("%s", indentificarRegistro((operador.valor >> 4)&0x0F));
                break;
                case 1:
                    printf("%cL", indentificarRegistro((operador.valor >> 4)&0x0F)[1]);
                break;
                case 2:
                    printf("%cH", indentificarRegistro((operador.valor >> 4)&0x0F)[1]);
                break;
                case 3:
                    printf("%cX", indentificarRegistro((operador.valor >> 4)&0x0F)[1]);
                break;
                default:
                break;
            }
        }
            break;
        case INMEDIATO:
            printf(" %d ", (signed) operador.valor );
            break;
        case MEMORIA:
            printf(" [%s + %d] ", indentificarRegistro((operador.valor >> 4)&0x0F),(operador.valor >> 8)&0x0FFFF);
            break;
        default:
            break;
    }
}

void escribirDisassembler(t_instruccion *instrucciones, int tamano) {
    char op_ant = 0;
    char *op1, *op2;

    for (int i = 0; i < tamano; i++) {
        printf("[%04X] %02X ", op_ant, instrucciones[i].opcode&0x0FF); 
        imprime_byte(instrucciones[i].op2.valor, instrucciones[i].op2.tipo);
        imprime_byte(instrucciones[i].op1.valor, instrucciones[i].op1.tipo);
        for (int j = 0; j < (7-instrucciones[i].op1.tipo-instrucciones[i].op2.tipo); j++) {
            printf("  ");
        }
        printf("\t| %s ",identificarMnemonico(instrucciones[i].opcode&0x01F));
        imprimir_operador(instrucciones[i].op1);
        if (instrucciones[i].op1.tipo != NINGUNO)
            printf(",");
        imprimir_operador(instrucciones[i].op2);
        printf("\n");
        op_ant += instrucciones[i].op1.tipo + instrucciones[i].op2.tipo + 1;
    }
    

}