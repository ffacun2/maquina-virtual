#include "operaciones.h"
#include "mv.h"
#include <time.h>
#include <stdlib.h>

// A la hora de hacer operaciones, se trabaja con short, el tamaño maximo es 16bits
// nos permite detectar los valores negativos.
// Con int no pasa.
// Hay valores negativos en inmediatos y offset

void FuncionCC(t_MV *maquina, int resultado)
{                               // actualiza CC
    maquina->registros[CC] = 0; // reseteo CC

    if (resultado == 0)
        maquina->registros[CC] |= 1 << 0;
    else
    {
        if (resultado < 0)
            maquina->registros[CC] |= 1 << 1;
    }
}

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
    FuncionCC(maquina, b + a);
}
void SUB(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SUB...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b - a, maquina);
    printf("Resultado: %d\n", b - a);
    FuncionCC(maquina, b - a);
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
    FuncionCC(maquina, b * a);
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
        FuncionCC(maquina, b / a);
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
    FuncionCC(maquina, resto);
    printf("Resultado de la comparación: %d (CC = %d)\n", resto, maquina->registros[CC]);
}
void SHL(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SHL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b << a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d\n", resultado);
}
void SHR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando SHR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b >> a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d\n", resultado);
}
void AND(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando AND...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b & a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void OR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando OR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b | a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void XOR(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando XOR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b ^ a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void LDL(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando LDL...\n");
    cargaBaja(op1, getValor(op2, *maquina), maquina);
}
void LDH(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando LDH...\n");
    cargaAlta(op1, getValor(op2, *maquina), maquina);
}
void RND(t_MV *maquina, t_operador op1, t_operador op2)
{
    printf("Ejecutando RND...\n");

    int a = getValor(op2, *maquina);

    static int iniciado = 0;
    if (!iniciado)
    {
        srand(time(NULL)); // Inicializar el generador de números aleatorios solo una vez
        iniciado = 1;
    }

    int resultado = rand() % (a + 1);
    setValor(op1, resultado, maquina);
    printf("Resultado: %d\n", resultado);
}

void SYS(t_MV *maquina, t_operador op1)
{
    char bin[33];
    printf("Ejecutando SYS...\n");
    switch (op1.valor)
    {
    case 1:                                   // Modo lectura
        switch (maquina->registros[A] & 0xFF) // AL
        {
        case 1: // Leer en decimal
            scanf("%d", &maquina->memoria[maquina->registros[D]]);
            break;
        case 2: // Leer caracter
            scanf("%c", &maquina->memoria[maquina->registros[D]]);
            break;
        case 4: // Leer en octal
            scanf("%o", &maquina->memoria[maquina->registros[D]]);
            break;
        case 8: // Leer en hexadecimal
            scanf("%x", &maquina->memoria[maquina->registros[D]]);
            break;
        case 16: // Leer en binario
            scanf("%s", bin);
            break;

        default:
            break;
        }

        break;
    case 2: // Modo escritura
        printf(maquina->memoria[maquina->registros[D]]);
        break;

    default:
        printf("ERROR: llamada a sistema operativo invalida");
        break;
    }
}
void JMP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JMP...\n");
    if (op1.tipo != INMEDIATO && op1.tipo != MEMORIA) 
        printf("Error: JMP solo admite inmediatos o direcciones lógicas (tipo MEMORIA)\n");
    else{
        int direccionLogica = getValor(op1, *maquina);    // El valor del operando es la dirección lógica a la que queremos saltar
        short offset = direccionLogica & 0xFFFF;            // Extraer offset (los 16 bits bajos)
        short tam = maquina->tabla_segmentos[CS].tamano; // Verificar que esté dentro del segmento de código
        if (offset >= tam) 
          printf("Error: salto fuera de los límites del código\n");
       else{
          maquina->registros[IP] = direccionLogica;
          printf("Salto a dirección lógica: 0x%08X\n", direccionLogica);
       }
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

    int b = getValor(op1, *maquina);
    int resultado = ~b; // negación bit a bit

    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    printf("Resultado: %d (0x%X)\n", resultado, resultado);
}

void STOP(t_MV *maquina)
{
    printf("Ejecutando STOP...\n");
}
