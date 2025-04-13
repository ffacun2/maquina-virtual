#include "mv.h"

void escribirDisassembler(t_instruccion *instrucciones, int tamano);
char* identificarMnemonico(int opcode);
char* identificarRegistro(int codigo) ;
void imprime_byte(int valor, int cant_byte);
void imprimir_operador(t_operador operador);
