#include <stdio.h>
#include <ctype.h>
#include "operaciones.h"
#include "mv.h"
#include "splitter.h"
#include <conio.h>

// A la hora de hacer operaciones, se trabaja con short, el tamaño maximo es 16bits
// nos permite detectar los valores negativos.
// Con int no pasa.
// Hay valores negativos en inmediatos y offset

void FuncionCC(t_MV *maquina, int resultado)
{                               // actualiza CC
    maquina->registros[CC] = 0; // reseteo CC

    if (resultado == 0)
        maquina->registros[CC] = 0x40000000;
    else
    {
        if (resultado < 0)
            maquina->registros[CC] = 0x80000000;
        else
            maquina->registros[CC] = 0x0;
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
    // printf("Ejecutando MOV...\n");
    int b = getValor(op2, *maquina);
    setValor(op1, b, maquina);
}
void ADD(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando ADD...\n");
    int a = getValor(op1, *maquina);
    int b = getValor(op2, *maquina);
    setValor(op1, a + b, maquina);
    // printf("Resultado ADD: %d\n", b + a);
    FuncionCC(maquina, b + a);
}
void SUB(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando SUB...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b - a, maquina);
    // printf("Resultado: %d\n", b - a);
    FuncionCC(maquina, b - a);
}
void SWAP(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando SWAP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, a, maquina);
    setValor(op2, b, maquina);
}
void MUL(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando MUL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    setValor(op1, b * a, maquina);
    // printf("Resultado: %d\n", b * a);
    FuncionCC(maquina, b * a);
}
void DIV(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando DIV...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    if (a != 0)
    {
        setValor(op1, b / a, maquina);
        maquina->registros[AC] = b % a; // resto
        // printf("Resultado: cosciente %d resto %d\n", b / a, b % a);
        FuncionCC(maquina, b / a);
    }
    else
        error(maquina, 2);
}
void CMP(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando CMP...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resto = b - a;
    FuncionCC(maquina, resto);
    // printf("Resultado de la comparacion: %d (CC = %d)\n", resto, maquina->registros[CC]);
}
void SHL(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando SHL...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b << a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d\n", resultado);
}
void SHR(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando SHR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b >> a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d\n", resultado);
}
void AND(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando AND...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b & a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void OR(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando OR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b | a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void XOR(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando XOR...\n");
    int b = getValor(op1, *maquina);
    int a = getValor(op2, *maquina);
    int resultado = b ^ a;
    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d (0x%X)\n", resultado, resultado);
}
void LDL(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando LDL...\n");
    setValor(op1, (getValor(op1, *maquina) & 0xFFFF0000) | (getValor(op2, *maquina) & 0x0000FFFF), maquina);
}
void LDH(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando LDH...\n");
    setValor(op1, (getValor(op1, *maquina) & 0x0000FFFF) | (getValor(op2, *maquina) << 16), maquina);
}
void RND(t_MV *maquina, t_operador op1, t_operador op2)
{
    // printf("Ejecutando RND...\n");

    int a = getValor(op2, *maquina);

    static int iniciado = 0;
    if (!iniciado)
    {
        srand(time(NULL)); // Inicializar el generador de números aleatorios solo una vez
        iniciado = 1;
    }
    if (a > 0)
    {
        int resultado = rand() % (a + 1);
        setValor(op1, resultado, maquina);
        // printf("Resultado: %d\n", resultado);
    }
    else
        printf("RDN no admite negativos\n");
}

void SYS(t_MV *maquina, t_operador op1)
{
    char *str, bin[33], caracter;
    int CX = maquina->registros[C] & 0xFFFF;
    int CL = maquina->registros[C] & 0xFF;
    int CH = (maquina->registros[C] >> 8) & 0xFF;
    int i, j, k, x, dirFisica;
    int salidas[32], z[32];
    // Splitter 1 corta de a bits
    // Splitter 2 corta de a bytes
    t_splitter splitter1, splitter2;
    FILE *imagen;
    splitter1 = constructorSplitter(maquina->registros[A] & 0xFF, 1);
    getSalidas(splitter1, salidas);
    setTamanio(&splitter2, 8);
    dirFisica = maquina->tabla_segmentos[(maquina->registros[D] >> 16) & 0xFFFF].base + maquina->registros[D] & 0xFFFF;
    switch (op1.valor)
    {
    case 1: // Modo lectura

        switch (maquina->registros[A] & 0xFF) // AL
        {
        case 1: // Leer en decimal
            for (i = 0; i < CL; i++)
            {
                printf("[%04X]: ", dirFisica + i * CH);
                scanf("%d", &x);
                setEntrada(&splitter2, x);
                getSalidas(splitter2, z);
                for (j = 0; j < CH; j++)
                    maquina->memoria[dirFisica + i * CH + j] = z[CH - j - 1];
            }
            break;
        case 2: // Leer caracter
            for (i = 0; i < CL; i++)
            {
                printf("[%04X]: ", dirFisica + i * CH);
                scanf("%s", str);
                for (j = 0; j < CH; j++)
                    maquina->memoria[dirFisica + i * CH + j] = str[j];
            }
            break;
        case 4: // Leer en octal
            for (i = 0; i < CL; i++)
            {
                printf("[%04X]: ", dirFisica + i * CH);
                scanf("%o", &x);
                setEntrada(&splitter2, x);
                getSalidas(splitter2, z);
                for (j = 0; j < CH; j++)
                    maquina->memoria[dirFisica + i * CH + j] = z[CH - j - 1];
            }
            break;
        case 8: // Leer en hexadecimal
            for (i = 0; i < CL; i++)
            {
                printf("[%4X]: ", dirFisica + i * CH);
                scanf("%x", &x);
                for (j = CH - 1; j >= 0; j--)
                {
                    maquina->memoria[dirFisica + i * CH + j] = x & 0xFF;
                    x = x >> 8;
                }
            }
            break;
        case 16: // Leer en binario
            for (i = 0; i < CL; i++)
            {
                printf("[%04X]: ", dirFisica + i * CH);
                scanf("%s", bin);
                x = deBinarioStringAInt(bin);
                setEntrada(&splitter2, x);
                getSalidas(splitter2, z);
                for (j = 0; j < CH; j++)
                    maquina->memoria[dirFisica + i * CH + j] = z[CH - j - 1];
            }
            break;

        default:
            break;
        }

        break;
    case 2: // Modo escritura
        for (i = 0; i < CL; i++)
        {
            printf("[%04X]: ", dirFisica + i * CH);
            for (k = 4; k >= 0; k--)
                if (salidas[k])
                {
                    x = 0;
                    for (j = 0; j < CH; j++)
                    {
                        x = x << 8;
                        x |= (maquina->memoria[dirFisica + i * CH + j] & 0x0FF);
                    }
                    switch (k)
                    {
                    case 4: // Escribe binario
                        printf("0b");
                        deIntABinarioString(x, bin);
                        printf("%s", bin);
                        printf(" ");
                        break;
                    case 3: // Escribe hexadecimal
                        printf("0x%08X ", x);
                        break;
                    case 2: // Escribe en octal
                        printf("0o%011o ", x);
                        break;
                    case 1: // Escribe caracteres
                        for (j = 0; j < CH; j++){
                            x = maquina->memoria[dirFisica + i * CH + j] & 0x00FF;
                            if (x > 0  && x < 255 && isprint(x))
                            printf("%c", x);
                            else
                            printf(".");
                        }
                        printf(" ");
                        break;
                    case 0: // Escribe decimal
                        printf("%d ", x);
                        break;

                    default:
                        break;
                    }
                }
            printf("\n");
        }
        break;

    case 3: // String read
        printf("[%04X]: ", dirFisica);
        scanf("%s",str);
        i = 0;
        while (str[i] != '\0' && i < CX)
        {
            maquina->memoria[maquina->registros[D] + i] = str[i];
            i++;
        }
        maquina->memoria[maquina->registros[D] + i] = '\0';
        break;
    
    case 4: // String write
        printf("[%04X]: %s", dirFisica, maquina->memoria[dirFisica]);
        break;
    
    case 7: // Clear screen
        clrscr();
        break;
    
    case 0xF: // Breakpoint
        maquina->flag_ejecucion = 0;
        imagen = fopen(maquina->nombreVMI, "wb");
        fwrite(maquina,sizeof(maquina),1,imagen);
        scanf("%c",&caracter);
        switch (caracter)
        {
        case 'g': // go
            maquina->flag_ejecucion = 1;
            break;
        case 'q': // quit
            STOP(maquina);
            break;
        case '\n': // Enter
            
            break;
        default:
            break;
        }
        
        break;
    }
}
void Salto(t_MV *mv, t_operador op1)
{                                                                                      // FUNCION QUE EJECUTA EL SALTO DE TODAS LAS FUNCIONES JUMP
    int valor = getValor(op1, *mv);                                                    // El valor del operando es la direccion logica a la que queremos saltar         // Extraer offset (los 16 bits bajos)
    short pos = mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base + valor; // Verificar que esté dentro del segmento de codigo
    short size = mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].tamano - mv->tabla_segmentos[(mv->registros[CS] >> 16) & 0x0FFFF].base;
    // printf("valor salto: %04X, posicion:%04X, size:%04X\n", valor, pos, size);

    if (pos > size)
        error(mv, 3);
    else
    {
        mv->registros[IP] = pos;
        // printf("Salto a direccion : 0x%04X\n", pos);
    }
}
void JMP(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JMP...\n");
    if (op1.tipo != INMEDIATO && op1.tipo != MEMORIA)
        printf("Error: JMP solo admite inmediatos o direcciones logicas (tipo MEMORIA)\n");
    else
        Salto(maquina, op1);
}
void JZ(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JZ...\n");

    if (maquina->registros[CC] & 0x40000000)
        Salto(maquina, op1);
    // else
    //  printf("No se cumple la condicion para el salto\n");
}
void JP(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JP...\n");
    if (maquina->registros[CC] == 0)
        Salto(maquina, op1);
    // else
    //     printf("No se cumple la condicion para el salto\n");
}
void JN(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JN...\n");
    if (maquina->registros[CC] & 0x80000000)
        Salto(maquina, op1);
    // else
    //     printf("No se cumple la condicion para el salto\n");
}
void JNZ(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JNZ...\n");
    if (!(maquina->registros[CC] & 0x40000000))
        Salto(maquina, op1);
    // else
    //     printf("No se cumple la condicion para el salto\n");
}
void JNP(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JNP...\n");
    if (!(maquina->registros[CC] == 0))
        Salto(maquina, op1);
    // else
    //     printf("No se cumple la condicion para el salto\n");
}
void JNN(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando JNN...\n");
    if (!(maquina->registros[CC] & 0x80000000))
        Salto(maquina, op1);
    // else
    //     printf("No se cumple la condicion para el salto\n");
}
void NOT(t_MV *maquina, t_operador op1)
{
    // printf("Ejecutando NOT...\n");

    int b = getValor(op1, *maquina);
    int resultado = ~b; // negacion bit a bit

    setValor(op1, resultado, maquina);
    FuncionCC(maquina, resultado);
    // printf("Resultado: %d (0x%X)\n", resultado, resultado);
}

void STOP(t_MV *mv)
{
    // printf("Ejecutando STOP...\n");
    mv->flag_ejecucion = 0;
    mv->registros[IP] = mv->tabla_segmentos[(mv->registros[CS]>>16) & 0x0FFFF].tamano & 0x0FFFF;
}
// Calcular la dirección física en el segmento de pila (SS)
int calcularDireccionFisica(t_MV *maquina, int segmento, int offset) {
    return maquina->tabla_segmentos[(segmento >> 16) & 0xFFFF].base + offset;
}

void escribirEnMemoria(t_MV *maquina, int direccion_fisica, int valor) {
    for (int i = 0; i < 4; i++) {
        maquina->memoria[direccion_fisica + i] = (valor >> (8 * i)) & 0xFF;
    }
}

int leerDesdeMemoria(t_MV *maquina, int direccion_fisica) {
    int valor = 0;
    for (int i = 0; i < 4; i++) {
        valor |= (maquina->memoria[direccion_fisica + i] & 0xFF) << (8 * i);
    }
    return valor;
}

void PUSH(t_MV *maquina, t_operador op1){
     printf("Ejecutando PUSH...\n");
     int valor = getValor(op1, *maquina);
     maquina->registros[SP] -= 4;  // Decrementar el Stack Pointer (SP) en 4 bytes  
     int direccion_fisica = calcularDireccionFisica(maquina, maquina->registros[SS], maquina->registros[SP]);

    // Guardar el valor en la pila
    escribirEnMemoria(maquina, direccion_fisica, valor);

}
void POP(t_MV *maquina, t_operador op1) {
    printf("Ejecutando POP...\n");
    int direccion_fisica = calcularDireccionFisica(maquina, maquina->registros[SS], maquina->registros[SP]);
    
    // Leer el valor de la pila (4 bytes)
    int valor = leerDesdeMemoria(maquina, direccion_fisica);

    maquina->registros[SP] += 4; // Incrementar el Stack Pointer (SP) en 4 bytes
    setValor(op1, valor, maquina);
}

void RET(t_MV *maquina) {
    printf("Ejecutando RET...\n");
    int direccion_fisica = calcularDireccionFisica(maquina, maquina->registros[SS], maquina->registros[SP]);
    // Leer la dirección de retorno desde la pila (4 bytes)
    int direccion_retorno = leerDesdeMemoria(maquina, direccion_fisica);

    maquina->registros[SP] += 4; // Incrementar el Stack Pointer (SP) en 4 bytes
    maquina->registros[IP] = direccion_retorno; // Actualizar el registro IP con la dirección de retorno
}

void CALL(t_MV *maquina, t_operador op1) {
    printf("Ejecutando CALL...\n");

    // Guardar la dirección de retorno (IP actual) en la pila
    int direccion_retorno = maquina->registros[IP];
    maquina->registros[SP] -= 4; // Decrementar el Stack Pointer (SP) en 4 bytes
    int direccion_fisica = calcularDireccionFisica(maquina, maquina->registros[SS], maquina->registros[SP]);
    escribirEnMemoria(maquina, direccion_fisica, direccion_retorno);

    int direccion_subrutina = getValor(op1, *maquina);
    maquina->registros[IP] = direccion_subrutina;// Actualizar el registro IP para saltar a la subrutina
}

void inicializo_vector_op(t_func0 func0[], t_func1 func1[], t_func2 func2[])
{
    func0[0] = &STOP;

    func1[0] = &SYS;
    func1[1] = &JMP;
    func1[2] = &JZ;
    func1[3] = &JP;
    func1[4] = &JN;
    func1[5] = &JNZ;
    func1[6] = &JNP;
    func1[7] = &JNN;
    func1[8] = &NOT;

    func2[0] = &MOV;
    func2[1] = &ADD;
    func2[2] = &SUB;
    func2[3] = &SWAP;
    func2[4] = &MUL;
    func2[5] = &DIV;
    func2[6] = &CMP;
    func2[7] = &SHL;
    func2[8] = &SHR;
    func2[9] = &AND;
    func2[10] = &OR;
    func2[11] = &XOR;
    func2[12] = &LDL;
    func2[13] = &LDH;
    func2[14] = &RND;
    func2[15] = &PUSH;
    func2[16] = &POP;
    func2[17] = &RET;
    func2[18] = &CALL;
}
