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
        return "RND"

    default:
        return "Mnemonico no identificado";
    }
}

void escribirDisassembler(t_MV maquina, short tamano)
{
    char *mnemonicos = {};
    int tamOpA, tamOpB, pc = 0, codMnemonic, instruccion;
    while (pc < tamano)
    {
        instruccion = maquina.memoria[pc];
        codMnemonic = instruccion & 0x1F;
        tamOpA = (instruccion >> 4) & 3;
        tamOpB = (instruccion >> 6) & 3;
    }
}