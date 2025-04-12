#include "operaciones.h"
#include "mv.h"
#include <time.h>
#include <stdlib.h>
#include "splitter.h"

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

int deBinarioStringAInt(char bin[])
{
    int exponente = 0;
    int resultado = 0;
    for (int i = strlen(bin) - 1; i >= 0; i--)
    {
        if (bin[i] != '0' && bin[i] != '1')
        {
            printf("ERROR: se pidio un numero binario y se ingreso cualquier cosa");
            break;
        }
        resultado += (bin[i] & 0xF) * pow(2, exponente);
        exponente++;
    }
    return resultado;
}

void deIntABinarioString(int nro, char bin[])
{
    bin[32] = '\0';
    for (int i = 31; i >= 0; i--)
    {
        bin[i] = '0' + (nro & 0x1);
        nro = nro >> 1;
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
    setValor(op2, b, maquina);
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
    char bin[33], *str;
    int CL = maquina->registros[C] & 0xFF;
    int CH = (maquina->registros[C] >> 8) & 0xFF;
    int i, j, x;
    int salidas[32];
    t_splitter splitter1, splitter2;
    printf("Ejecutando SYS...\n");
    splitter1 = constructorSplitter(maquina->registros[A] & 0xFF, 1);
    getSalidas(splitter1, salidas);
    setTamanio(&splitter2, 8);
    switch (op1.valor)
    {
    case 1: // Modo lectura

        switch (maquina->registros[A] & 0xFF) // AL
        {
        case 1: // Leer en decimal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                scanf("%d", &x);
                for (j = CH - 1; j >= 0; j--)
                {
                    maquina->memoria[maquina->registros[D] + i * CH + j] = x & 0xFF;
                    x = x >> 8;
                }
            }
            break;
        case 2: // Leer caracter
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                scanf("%s", str);
                for (j = 0; j < CH; j++)
                    maquina->memoria[maquina->registros[D] + i * CH + j] = str[j];
            }
            break;
        case 4: // Leer en octal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                scanf("%o", &x);
                for (j = CH - 1; j >= 0; j--)
                {
                    maquina->memoria[maquina->registros[D] + i * CH + j] = x & 0xFF;
                    x = x >> 8;
                }
            }
            break;
        case 8: // Leer en hexadecimal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                scanf("%x", &x);
                for (j = CH - 1; j >= 0; j--)
                {
                    maquina->memoria[maquina->registros[D] + i * CH + j] = x & 0xFF;
                    x = x >> 8;
                }
            }
            break;
        case 16: // Leer en binario
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                scanf("%s", bin);
                x = deBinarioStringAInt(bin);
                for (j = CH - 1; j >= 0; j--)
                {
                    maquina->memoria[maquina->registros[D] + i * CH + j] = x & 0xFF;
                    x = x >> 8;
                }
            }
            break;

        default:
            break;
        }

        break;
    case 2:                                   // Modo escritura
        switch (maquina->registros[A] & 0xFF) // AL
        {
        case 1: // Escribir en decimal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                x = 0;
                for (j = 0; j < CH; j++)
                {
                    x = x << 8;
                    x = x | maquina->memoria[maquina->registros[D] + i * CH + j];
                }
                printf("%d\n", x);
            }
            break;
        case 2: // Escribir caracter
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", maquina->registros[D] + i * CH);
                for (j = 0; j < CH; j++)
                    printf("%c", maquina->memoria[maquina->registros[D] + i * CH + j]);
                printf("\n");
            }
            break;
        case 4: // Escribir en octal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: 0o", maquina->registros[D] + i * CH);
                x = 0;
                for (j = 0; j < CH; j++)
                {
                    x = x << 8;
                    x = x | maquina->memoria[maquina->registros[D] + i * CH + j];
                }
                printf("%o\n", x);
            }
            break;
        case 8: // Escribir en hexadecimal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: 0x", maquina->registros[D] + i * CH);
                for (j = 0; j < CH; j++)
                    printf("%x", maquina->memoria[maquina->registros[D] + i * CH + j]);
                printf("\n");
            }
            break;
        case 16: // Escribir en binario
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: 0b", maquina->registros[D] + i * CH);
                for (j = 0; j < CH; j++)
                {
                    deIntABinarioString(maquina->memoria[maquina->registros[D] + i * CH + j], bin);
                    printf("%s", bin);
                }
                printf("\n");
            }
            break;

        default:
            break;
        }
        break;

    default:
        printf("ERROR: llamada a sistema operativo invalida");
        break;
    }
}
void Salto(t_MV *maquina, t_operador op1)
{                                                    // FUNCION QUE EJECUTA EL SALTO DE TODAS LAS FUNCIONES JUMP
    int direccionLogica = getValor(op1, *maquina);   // El valor del operando es la dirección lógica a la que queremos saltar
    short offset = direccionLogica & 0xFFFF;         // Extraer offset (los 16 bits bajos)
    short tam = maquina->tabla_segmentos[CS].tamano; // Verificar que esté dentro del segmento de código
    if (offset >= tam)
        printf("Error: salto fuera de los límites del código\n");
    else
    {
        maquina->registros[IP] = direccionLogica;
        printf("Salto a dirección lógica: 0x%08X\n", direccionLogica);
    }
}
void JMP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JMP...\n");
    if (op1.tipo != INMEDIATO && op1.tipo != MEMORIA)
        printf("Error: JMP solo admite inmediatos o direcciones lógicas (tipo MEMORIA)\n");
    else
        Salto(maquina, op1);
}
void JZ(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JZ...\n");

    if (maquina->registros[CC] & (1 << 0))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
}
void JP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JP...\n");
    if (!(maquina->registros[CC] & (1 << 1)))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
}
void JN(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JN...\n");
    if (maquina->registros[CC] & (1 << 1))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
}
void JNZ(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNZ...\n");
    if (!(maquina->registros[CC] & (1 << 0)))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
}
void JNP(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNP...\n");
    if ((maquina->registros[CC] & (1 << 1)) || (maquina->registros[CC] & (1 << 0)))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
}
void JNN(t_MV *maquina, t_operador op1)
{
    printf("Ejecutando JNN...\n");
    if (!(maquina->registros[CC] & (1 << 1)))
        Salto(maquina, op1);
    else
        printf("No se cumple la condición para el salto\n");
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
    exit(0);
}
