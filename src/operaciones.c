#include "operaciones.h"
#include "mv.h"

void MOV(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando MOV...\n");
    int b = getValor(op2, *maquina);
    setValor(op1, b, maquina);
}
void ADD(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando ADD...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b + a, maquina);
    printf("Resultado: %d\n", b + a);
    CMP(maquina, op1, op2);
}
void SUB(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SUB...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b - a, maquina);
    printf("Resultado: %d\n", b - a);
    CMP(maquina, op1, op2);
}
void SWAP(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SWAP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, a, maquina);
    setValor(op1, b, maquina);
}
void MUL(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando MUL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b * a, maquina);
    printf("Resultado: %d\n", b * a);
    CMP(maquina, op1, op2);
}
void DIV(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando DIV...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    if (a != 0)
    {
        setValor(op1, b / a, maquina);
        maquina->registros[AC] = b % a; // resto
        printf("Resultado: cosciente %d resto %d\n", b / a, b % a);
        CMP(maquina, op1, op2);
    }
    else
        printf("error divisor no pude ser cero\n");
}
void CMP(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando CMP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resto = b - a;

    maquina->registros[CC] = 0; // reseteo CC

    if (resto == 0)
        maquina->registros[CC] |= 1 << 0;
    else
    {
        if (resto < 0)
            maquina->registros[CC] |= 1 << 1;
    }
    printf("Resultado de la comparación: %d (CC = %d)\n", resto, maquina->registros[CC]);
}
void SHL(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SHL...\n");
}
void SHR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SHR...\n");
}
void AND(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando AND...\n");
}
void OR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando OR...\n");
}
void XOR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando XOR...\n");
}
void LDL(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando LDL...\n");
}
void LDH(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando LDH...\n");
}
void RND(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando RND...\n");
}

void SYS(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando SYS...\n");
}
void JMP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JMP...\n");
}
void JZ(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JZ...\n");
}
void JP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JP...\n");
}
void JN(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JN...\n");
}
void JNZ(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNZ...\n");
}
void JNP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNP...\n");
}
void JNN(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNN...\n");
}
void NOT(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando NOT...\n");
}

void STOP(t_MV *maquina)
{
    printf("Ejecutando STOP...\n");
}
