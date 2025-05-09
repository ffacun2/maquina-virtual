#include "operaciones.h"
#include "disassembler.h"
#include "generador_imagen.h"

int lectura_vmx(t_MV *maquina, char **param, int size_param);
int verifico_tamano(char tamano);
int verifico_tamano2(short segmento_sizes[]);

/*
    compilo el programa:
    gcc main.c mv.c operaciones.c disassembler.c splitter.c -o main.exe

    ejecuto el programa con el nombre de archivo traducido:
    main.exe sample.vmx
*/
int main(int argc, char **argv)
{
    t_MV mv;
    t_instruccion *instrucciones;
    int instruccion_size = 0;

    mv.nombreVMI = NULL;    // Inicializo el nombre del archivo vmi
    mv.nombreVMX = NULL;    // Inicializo el nombre del archivo vmx
    mv.flag_ejecucion = 1;  // Inicializo el flag de ejecución
    mv.flag_breakpoint = 0; // Inicializo el flag breakpoint
    mv.flag_d = 0;          // Inicializo el flag de disassembler
    // Verifico que se haya ingresado el nombre del archivo
    if (argc > 1)
    {
        // Lectura de los argumentos
        int i = 0;
        while (i < argc && strcmp(argv[i], "-p") != 0)
        {
            if (strstr(argv[i], ".vmx"))
            {
                mv.nombreVMX = argv[i]; // Guardar el nombre del archivo vmx
            }
            else if (strstr(argv[i], ".vmi"))
            {
                mv.nombreVMI = argv[i]; // Guardar el nombre del archivo vmi
            }
            else if (strncmp(argv[i], "m=", 2) == 0)
            {
                mv.memory_size = atoi(argv[i] + 2) * 1024; // Guardar el tamaño de memoria
            }
            else if (strcmp(argv[i], "-d") == 0)
            {
                mv.flag_d = 1; // Activar el flag de disassembler
            }
            i++;
        }
        i++;

        char **param = argv + i;
        int size_param = argc - i;

        // Inicializar la máquina virtual
        if (mv.nombreVMX != NULL && mv.nombreVMI == NULL && lectura_vmx(&mv, param, size_param))
        {
            genero_array_instrucciones(&mv, &instrucciones, &instruccion_size);
        }
        else if (mv.nombreVMX == NULL && mv.nombreVMI != NULL && lectura_vmi(&mv))
        {
            genero_array_instrucciones(&mv, &instrucciones, &instruccion_size);
        }

        if (mv.flag_d && mv.flag_ejecucion)
        {
            // Si se activa el flag de disassembler, se escribe el disassembler
            escribirDisassembler(instrucciones, instruccion_size);
        }

        ejecutar_maquina(&mv, instrucciones, instruccion_size); // Ejecutar la máquina virtual
        free(instrucciones);
        

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
    maquina virtual (data segment).
    Si es version 1: Se lee del archivo el codigo y se carga en la memoria
    de la maquina virtual, y se inicializa la tabla de segmentos.
    Si es version 2: Se lee el header y se desarma para obtener los tamaños
    de cada segmento, se cargan los segmentos en la memoria de la maquina virtual
    y se inicializa la tabla de segmentos junto con los registros.
    @param maquina: puntero a la maquina virtual
    @param param: array de parametros a cargar en la memoria de la maquina virtual
    leidos en la linea de comandos.
    @param cant_param: cantidad de parametros a cargar en la memoria de la maquina virtual
    @return: 1 si la lectura fue exitosa, 0 si hubo un error.

*/
int lectura_vmx(t_MV *maquina, char **param, int cant_param)
{
    FILE *archivo = fopen(maquina->nombreVMX, "rb");
    char modelo[6];
    char version;
    char header[19];
    short high, low;
    short tamano;
    short tamanio_segmentos[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int size_param = 0;
    
    // Verifico que el archivo se haya abierto correctamente
    if (archivo == NULL)
    {
        printf("Error al abrir el archivo\n");
        maquina->flag_ejecucion = 0; // Desactivo la ejecución de la máquina virtual
        return 0;
    }

    fread(modelo, sizeof(char), 5, archivo);   // Leo el modelo (VMX25) del archivo
    fread(&version, sizeof(char), 1, archivo); // Leo la version del archivo
    printf("Modelo: %s, Version: %d\n", modelo, version);
    if (strcmp(modelo, "VMX25") != 0 || (version != 1 && version != 2))
    {
        fclose(archivo);
        maquina->flag_ejecucion = 0; // Desactivo la ejecución de la máquina virtual
        printf("Aca");
        return 0;
    }
    else
    {
        fseek(archivo, 0, SEEK_SET); // Muevo el puntero al byte 14 del archivo
        if (version == 1)
        {
            maquina->version = 1;                    // Asigno la version a la maquina virtual
            fread(header, sizeof(char), 8, archivo); // Leo el header del archivo
            header[8] = '\0';                        // Aseguro que el header sea una cadena de caracteres
            // Leo tamaño de datos e inicializo la máquina virtual
            high = header[6] & 0x0FF;     // Leo el byte alto del tamaño de datos
            low = header[7] & 0x0FF;      // Leo el byte bajo del tamaño de datos
            tamano = ((high << 8) | low); // armo el tamaño de datos

            if (verifico_tamano(tamano) == 0)
            {
                fclose(archivo);
                maquina->flag_ejecucion = 0; // Desactivo la ejecución de la máquina virtual
                printf("Error: Tamaño de code segment excede la memoria de la máquina virtual\n");
                return 0;
            }
            // Leo los datos del archivo y los guardo en la memoria de la máquina virtual
            fread(maquina->memoria, 1, tamano, archivo);
            // Si el tamaño es correcto, inicializo la máquina virtual
            inicializar_maquina(maquina, tamano);
        }
        else
        {
            
            maquina->version = 2;                     // Asigno la version a la maquina virtual
            fread(header, sizeof(char), 18, archivo); // Leo el header del archivo
            header[18] = '\0';                        // Aseguro que el header sea una cadena de caracteres

            // Desarmo el header para obtener el tamaño de cada segmento y agruparlo en un array
            tamanio_segmentos[1] = (header[14] << 8) | (header[15] & 0x0FF); // Leo el tamaño del segmento de consantes
            tamanio_segmentos[2] = (header[6] << 8) | (header[7] & 0x0FF);   // Leo el tamaño del segmento de codigo
            tamanio_segmentos[3] = (header[8] << 8) | (header[9] & 0x0FF);   // Leo el tamaño del segmento de datos
            tamanio_segmentos[4] = (header[10] << 8) | (header[11] & 0x0FF); // Leo el tamaño del segmento memoria dinamica
            tamanio_segmentos[5] = (header[12] << 8) | (header[13] & 0x0FF); // Leo el tamaño del segmento de stack

            char constant[tamanio_segmentos[1]]; // Defino el tamaño del segmento de constantes
            char code[tamanio_segmentos[2]];     // Defino el tamaño del segmento de código

            // Leo todo el codigo maquina del archivo
            fread(code, sizeof(char), tamanio_segmentos[2], archivo);
            // Leo todo el codigo de constantes del archivo
            fread(constant, sizeof(char), tamanio_segmentos[1], archivo);

            // Cargo el semento de parametros en la memoria de la máquina virtual
            size_param = cargoParamSegment(maquina, param, cant_param);

            tamanio_segmentos[0] = size_param;

            if (verifico_tamano2(tamanio_segmentos) == 0)
            {
                fclose(archivo);
                maquina->flag_ejecucion = 0; // Desactivo la ejecución de la máquina virtual
                printf("Error: Tamaño de segmentos excede la memoria de la máquina virtual\n");
                return 0;
            }

            inicializar_maquina2(maquina, tamanio_segmentos, (header[16] << 8) | (header[17] & 0x0FF)); // Inicializo la máquina virtual

            // Cargo el segmento de código en la memoria de la máquina virtual
            cargoCodeSegment(maquina, code, tamanio_segmentos[2]);
            // Cargo el segmento de constantes en la memoria de la máquina virtual
            cargoConstSegment(maquina, constant, tamanio_segmentos[1]);

            for (int i = 0; i < 8; i++)
            {
                printf("Segmento[%d]: base: %04X, tamano: %04X\n", i, maquina->tabla_segmentos[i].base, maquina->tabla_segmentos[i].tamano);
            }

            for (int i = 0; i < 7; i++)
            {
                printf("Registro[%d]: %08X\n", i, maquina->registros[i]);
            }

            int x = 0;
            for (int i = 0; i < 50; i++)
            {
                if (x == 4)
                {
                    printf("\n");
                    x = 0;
                }
                printf("%02X ", maquina->memoria[i] & 0x0FF);
                x++;
            }
            printf("\n\n");
        }
    }
    fclose(archivo);
    return 1;
}



/*
    Verifico que el tamaño de los datos leidos no sea mayor a el tamaño
    de memoria de la maquina virtual
    @param tamano: tamaño de los datos leidos
    @return: 1 si el tamaño es correcto, 0 si es incorrecto
*/
int verifico_tamano(char tamano)
{
    if (tamano < TAMANO_MEMORIA)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
    Verifico que el tamaño de los segmentos leidos no sea mayor a el tamaño
    de memoria de la maquina virtual
    @param segmento_sizes: array con los tamaños de los segmentos
    @return: 1 si el tamaño es correcto, 0 si es incorrecto
*/
int verifico_tamano2(short segmento_sizes[])
{
    int total_size = 0;
    for (int i = 0; i < 6; i++)
    {
        if (segmento_sizes[i] > 0)
        {
            total_size += segmento_sizes[i];
        }
    }
    return total_size <= TAMANO_MEMORIA;
}
