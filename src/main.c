#include "operaciones.h"
#include "disassembler.h"

int lectura_archivo(char *nombre_archivo, t_MV *maquina);
int verifico_header(char *header);
int verifico_tamano(char tamano);

/*
    compilo el programa:
    gcc main.c mv.c operaciones.c disassembler.c splitter.c -o main.exe

    ejecuto el programa con el nombre de archivo traducido:
    main.exe sample.vmx
*/
int main(int argc, char **argv)
{
    t_MV maquina;
    t_instruccion *instrucciones;
    int instruccion_size = 0;

    // Verifico que se haya ingresado el nombre del archivo
    if (argc > 1)
    {
        if (lectura_archivo(argv[1], &maquina))
        {
            genero_array_instrucciones(&maquina, &instrucciones, &instruccion_size);
            if (argc > 2 && strcmp(argv[2], "-d") == 0)
            {
                escribirDisassembler(instrucciones, instruccion_size);
            }
            ejecutar_maquina(&maquina, instrucciones, instruccion_size); // Ejecutar la máquina virtual
            free(instrucciones);
        }
    }
    else
    {
        printf("No se ha ingresado el nombre del archivo\n");
        return 1;
    }

    return 0;
}

/*
    Hago lectura del archivo traducido, primero el header para verificar
    identificador y version, y luego los datos que iran a la memoria de la
    maquina virtual (data segment)
*/
int lectura_archivo(char *nombre_archivo, t_MV *maquina)
{
    FILE *archivo = fopen(nombre_archivo, "rb");
    char header[9];
    short high, low;
    short tamano;
    short lenNombreArchivo = strlen(nombre_archivo);

    // Verifico que el nombre del archivo tenga la extension .vmx
    if (strcmp(nombre_archivo + lenNombreArchivo - 4, ".vmx") != 0)
    {
        printf("El archivo no es un archivo vmx\n");
        return 0;
    }

    // Verifico que el archivo se haya abierto correctamente
    if (archivo == NULL)
    {
        printf("Error al abrir el archivo\n");
        return 0;
    }

    // Leer el header del archivo
    fread(header, sizeof(char), 8, archivo);
    // Me aseguro de que sea una cadena
    header[8] = '\0';

    // Verifico que el header sea correcto
    if (verifico_header(header) == 0)
    {
        printf("El header del archivo no es correcto, %s\n. El identificador o la version no son correctas.", header);
        return 0;
    }
    else
    {
        // Si el header es correcto, leo tamaño de datos e inicializo la máquina virtual
        high = header[6] & 0x0FF;                        // Leo el byte alto del tamaño de datos
        low = header[7] & 0x0FF;                         // Leo el byte bajo del tamaño de datos
        tamano = ((high << 8) | low); // armo el tamaño de datos
        
        if (verifico_tamano(tamano) == 0)
        {
            printf("El tamaño de datos es incorrecto, %d\n", tamano);
        }
        else
        {
            // Leo los datos del archivo y los guardo en la memoria de la máquina virtual
            fread(maquina->memoria, 1, tamano, archivo);
            // Si el tamaño es correcto, inicializo la máquina virtual
            inicializar_maquina(maquina, tamano);
        }
    }
    fclose(archivo);
    return 1;
}

/*
    Verifico que el header del archivo pasado por el traductor coincida con
    el identificador y version de la maquina virtual
*/
int verifico_header(char *header)
{
    char identificador[6];
    short version = header[5];

    strncpy(identificador, header, 5);
    identificador[5] = '\0';

    return (strncmp(identificador, IDENTIFICADOR, 5) == 0 && version == VERSION_MV);
}

/*
    Verifico que el tamaño de los datos leidos no sea mayor a el tamaño
    de memoria de la maquina virtual
*/
int verifico_tamano(char tamano)
{
    if (tamano < TAMANO_MEMORIA)
    {
        // El tamaño de datos es correcto
        return 1;
    }
    else
    {
        // El tamaño de datos es incorrecto
        return 0;
    }
}