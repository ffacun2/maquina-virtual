#include "operaciones.h"
#include "mv.h"

void FuncionCC(t_MV *maquina, int resultado){
    maquina->registros[CC]=0; //reseteo CC
    
    if(resultado == 0)
        maquina->registros[CC] |= 1 << 0;
    else{
        if(resultado < 0)
         maquina->registros[CC] |= 1 << 1;
    }
}
void MOV(t_MV *maquina, t_operador op1, t_operador op2) {
    printf("Ejecutando MOV...\n");
    int b = getValor(op2,*maquina);
    setValor(op1,b,maquina);
}
void ADD(t_MV *maquina, t_operador op1, t_operador op2) {
    printf("Ejecutando ADD...\n");
    int b = getValor(op1,*maquina);
    int a = getValor(op2,*maquina);
    setValor(op1,b+a,maquina);
    printf("Resultado: %d\n", b + a);
    FuncionCC(maquina, b + a);
}
void SUB(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando SUB...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b - a, maquina);
    printf("Resultado: %d\n", b - a);
    FuncionCC(maquina, b - a);
}
void SWAP(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando SWAP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, a, maquina);
    setValor(op1, b, maquina);
}
void MUL(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando MUL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b*a, maquina);
    printf("Resultado: %d\n", b*a);
    FuncionCC(maquina, b * a);
}
void DIV(t_MV *maquina, t_operador op1, t_operador op2){   
    printf("Ejecutando DIV...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    if(a != 0){
     setValor(op1, b/a, maquina);
     maquina->registros[AC] = b % a;  //resto   
     printf("Resultado: cosciente %d resto %d\n", b/a, b % a); 
     FuncionCC(maquina, b / a);
    }
    else
     printf("error divisor no pude ser cero\n");
        
}
void CMP(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando CMP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resto = b - a;
    FuncionCC(maquina, resto);
    printf("Resultado de la comparación: %d (CC = %d)\n", resultado, maquina->registros[CC]);
}
void SHL(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando SHL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b << a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d\n", resultado);
    
    
}
void SHR(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando SHR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b >> a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d\n", resultado);
}
void AND(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando AND...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b & a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
    
}
void OR(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando OR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b | a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void XOR(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando XOR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b ^ a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void LDL(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando LDL...\n");
}
void LDH(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando LDH...\n");
}
void RND(t_MV *maquina, t_operador op1, t_operador op2){
    printf("Ejecutando RND...\n");
}

void SYS(t_MV *maquina, t_operador op1){
    printf("Ejecutando SYS...\n");
}
void JMP(t_MV *maquina, t_operador op1){
    printf("Ejecutando JMP...\n");
}
void JZ(t_MV *maquina, t_operador op1){
    printf("Ejecutando JZ...\n");
}
void JP(t_MV *maquina, t_operador op1){
    printf("Ejecutando JP...\n");
}
void JN(t_MV *maquina, t_operador op1){
    printf("Ejecutando JN...\n");
}
void JNZ(t_MV *maquina, t_operador op1){
    printf("Ejecutando JNZ...\n");
}
void JNP(t_MV *maquina, t_operador op1){
    printf("Ejecutando JNP...\n");
}
void JNN(t_MV *maquina, t_operador op1){
    printf("Ejecutando JNN...\n");
}
void NOT(t_MV *maquina, t_operador op1){
    printf("Ejecutando NOT...\n");
}

void STOP(t_MV *maquina){
    printf("Ejecutando STOP...\n");
}
