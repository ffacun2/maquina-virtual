#ifndef OPERACIONES_H_INCLUDED
#define OPERACIONES_H_INCLUDED

#include "mv.h"

// Tipo array de funciones con ningun operando
typedef void (*t_func0)(t_MV *);
// tipo array de funciones con 1 operando
typedef void (*t_func1)(t_MV *, t_operador op1);
// tipo array de funciones con 2 operandos
typedef void (*t_func2)(t_MV *, t_operador op1, t_operador op2);

// Funciones auxiliares
void FuncionCC(t_MV *maquina, int resultado);
int deBinarioStringAInt(char bin[]);
void deIntABinarioString(int nro, char bin[]);

// funciones con 2 operandos
void MOV(t_MV *maquina, t_operador op1, t_operador op2);
void ADD(t_MV *maquina, t_operador op1, t_operador op2);
void SUB(t_MV *maquina, t_operador op1, t_operador op2);
void SWAP(t_MV *maquina, t_operador op1, t_operador op2);
void MUL(t_MV *maquina, t_operador op1, t_operador op2);
void DIV(t_MV *maquina, t_operador op1, t_operador op2);
void CMP(t_MV *maquina, t_operador op1, t_operador op2);
void SHL(t_MV *maquina, t_operador op1, t_operador op2);
void SHR(t_MV *maquina, t_operador op1, t_operador op2);
void AND(t_MV *maquina, t_operador op1, t_operador op2);
void OR(t_MV *maquina, t_operador op1, t_operador op2);
void XOR(t_MV *maquina, t_operador op1, t_operador op2);
void LDL(t_MV *maquina, t_operador op1, t_operador op2);
void LDH(t_MV *maquina, t_operador op1, t_operador op2);
void RND(t_MV *maquina, t_operador op1, t_operador op2);

// funciones con 1 operando
void SYS(t_MV *maquina, t_operador op1);
void JMP(t_MV *maquina, t_operador op1);
void JZ(t_MV *maquina, t_operador op1);
void JP(t_MV *maquina, t_operador op1);
void JN(t_MV *maquina, t_operador op1);
void JNZ(t_MV *maquina, t_operador op1);
void JNP(t_MV *maquina, t_operador op1);
void JNN(t_MV *maquina, t_operador op1);
void NOT(t_MV *maquina, t_operador op1);

// funciones sin operandos
void STOP(t_MV *maquina);

#endif // OPERACIONES_H