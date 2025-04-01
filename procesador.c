typedef struct
{
    int mnemonico, op1, op2;
} Decodificador;

typedef int MAR;

typedef struct
{
    int A, B, N, Z, funcion, resultado;
} ALU;

void operar(ALU alu)
{
    switch (alu.funcion)
    {
    case 0:
        alu.resultado = alu.A + alu.B;
        break;

    case 1:
        alu.resultado = alu.A & alu.B;
        break;

    case 2:
        alu.resultado = alu.A;
        break;

    case 3:
        alu.resultado = ~alu.A;
        break;

    default:
        break;
    }
}

typedef struct
{
    Decodificador decodificador;
    MAR mar;
    ALU alu;
} Procesador;
