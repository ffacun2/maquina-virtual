#include "operaciones.h"
#include "disassembler.h"

int lectura_vmx(t_MV* maquina,char **param, int size_param);
int lectura_vmi(t_MV* maquina);
int verifico_tamano(char tamano);

/*
    compilo el programa:
    gcc main.c mv.c operaciones.c disassembler.c splitter.c -o main.exe

    ejecuto el programa con el nombre de archivo traducido:
    main.exe sample.vmx
*/
int main(int argc, char** argv) {
    t_MV mv;
    t_instruccion* instrucciones;
    int instruccion_size = 0;

    mv.nombreVMI = NULL; // Inicializo el nombre del archivo vmi
    mv.nombreVMX = NULL; // Inicializo el nombre del archivo vmx


    // Verifico que se haya ingresado el nombre del archivo
    if (argc > 1) {
        //Lectura de los argumentos
        int i = 0;
        while ( i < argc && strcmp(argv[i], "-p") != 0) {
            if (strstr(argv[i], ".vmx")) {
                mv.nombreVMX = argv[i]; // Guardar el nombre del archivo vmx
            }
            else if (strstr(argv[i], ".vmi")) {
                mv.nombreVMI = argv[i]; // Guardar el nombre del archivo vmi
            }
            else if (strncmp(argv[i], "m=", 2) == 0) {
                //El tamaño viene en Kib, lo multiplico por 1024 para pasarlo a bytes
                mv.memory_size = atoi(argv[i] + 2) * 1024; // Guardar el tamaño de memoria
            }
            else if (strcmp(argv[i], "-d") == 0) {
                mv.flag_d = 1; // Activar el flag de disassembler
            }
            i++;
        }
        i++;

        char **param = argv + i;
        int size_param = argc - i;
       
        // Inicializar la máquina virtual
        if (mv.nombreVMX != NULL && lectura_vmx(&mv, param, size_param)) {
            // genero_array_instrucciones(&mv, &instrucciones, &instruccion_size);
        }
        else if (mv.nombreVMI != NULL && lectura_vmi(&mv)) {
            // Si se activa el flag de disassembler, se escribe el disassembler
            // genero_array_instrucciones(&mv, &instrucciones, &instruccion_size);
        }
        
        if (mv.flag_d) {
            // Si se activa el flag de disassembler, se escribe el disassembler
            escribirDisassembler(instrucciones, instruccion_size);
        }
        
        // ejecutar_maquina(&mv, instrucciones, instruccion_size); // Ejecutar la máquina virtual
        // free(instrucciones);
    }
    else {
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
int lectura_vmx(t_MV* maquina, char** param, int cant_param) {
    FILE* archivo = fopen(maquina->nombreVMX, "rb");
    char modelo[6];
    char version;
    char header[19];
    short high, low;
    short tamano;
    short tamanio_segmentos[8];
    int size_param = 0;

    // Verifico que el archivo se haya abierto correctamente
    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return 0;
    }

    fread(modelo,sizeof(char),5,archivo); // Leo el modelo (VMX25) del archivo
    fread(&version,sizeof(char),1,archivo); // Leo la version del archivo

    if (strcmp(modelo, "VMX25") != 0 || (version != 1 && version != 2)) {
        printf("El header del archivo no es correcto, %s\n. El identificador o la version no son correctas, %d.\n",modelo,version);
        fclose(archivo);
        return 0;
    }
    else {
        if (version == 1) {
            //Leo tamaño de datos e inicializo la máquina virtual
            high = header[6] & 0x0FF;     // Leo el byte alto del tamaño de datos
            low = header[7] & 0x0FF;      // Leo el byte bajo del tamaño de datos
            tamano = ((high << 8) | low); // armo el tamaño de datos
                
            if (verifico_tamano(tamano) == 0) {
                printf("El tamaño de datos es incorrecto, %d\n", tamano);
                fclose(archivo);
                return 0;
            }
            else {
                // Leo los datos del archivo y los guardo en la memoria de la máquina virtual
                fread(maquina->memoria, 1, tamano, archivo);
                // Si el tamaño es correcto, inicializo la máquina virtual
                inicializar_maquina(maquina, tamano);
            }
        }
        else {
            //El archivo trae header -> codigo -> constantes
            fseek(archivo, 0, SEEK_SET); // Muevo el puntero al byte 14 del archivo
            fread(header, sizeof(char), 18, archivo); // Leo el header del archivo
            header[18] = '\0'; // Aseguro que el header sea una cadena de caracteres
            
            //Desarmo el header para obtener el tamaño de cada segmento y agruparlo en un array
            tamanio_segmentos[1] = (header[14] << 8) | (header[15] & 0x0FF); // Leo el tamaño del segmento de consantes
            tamanio_segmentos[2] = (header[6] << 8) | (header[7] & 0x0FF); // Leo el tamaño del segmento de codigo
            tamanio_segmentos[3] = (header[8] << 8) | (header[9] & 0x0FF); // Leo el tamaño del segmento de datos
            tamanio_segmentos[4] = (header[10] << 8) | (header[11] & 0x0FF); // Leo el tamaño del segmento memoria dinamica
            tamanio_segmentos[5] = (header[12] << 8) | (header[13] & 0x0FF); // Leo el tamaño del segmento de stack

            char constant[tamanio_segmentos[1]]; // Defino el tamaño del segmento de constantes
            char code[tamanio_segmentos[2]]; // Defino el tamaño del segmento de código
            
            //Leo todo el codigo maquina del archivo
            fread(&code, sizeof(char), tamanio_segmentos[2], archivo);
           //Leo todo el codigo de constantes del archivo
            fread(&constant, sizeof(char), tamanio_segmentos[1], archivo); 
          
            // Cargo el semento de parametros en la memoria de la máquina virtual
            size_param = cargoParamSegment(maquina, param, cant_param);

            tamanio_segmentos[0] = size_param;
            //inicializar_maquina2(maquina, tamanio_segmetos); // Inicializo la máquina virtual

            
            // Cargo el segmento de código en la memoria de la máquina virtual
            cargoCodeSegment(maquina, code, tamanio_segmentos[2]); 
            // Cargo el segmento de constantes en la memoria de la máquina virtual
            cargoConstSegment(maquina, constant, tamanio_segmentos[1]); 
            
           
            for (int i = 0; i < 50; i++)
            {
                printf("%02X ", maquina->memoria[i]);
            }
            
        }
    }
    fclose(archivo);
    return 1;
}

int lectura_vmi(t_MV* mv) {
    short high, low;
    short tamano;
    char modelo[6];
    char version;
    FILE* archivo = fopen(mv->nombreVMI, "rb");
    int registros[16];
    int segmentos[8];

    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return 0;
    }

    fread(modelo, sizeof(char), 5, archivo); // Leo el header del archivo
    modelo[5] = '\0'; // Aseguro que el header sea una cadena de caracteres

    fread(&version, sizeof(char), 1, archivo); // Leo la version del archivo

    if (strcmp(modelo, "VMI25") != 0 || version != 1) {
        printf("El header del archivo no es correcto, %s - %d",modelo,version);
        fclose(archivo);
        return 0;
    }
    else {
        fread(&high, sizeof(char), 1, archivo); // Leo el byte alto del tamaño de datos
        fread(&low, sizeof(char), 1, archivo); // Leo el byte bajo del tamaño de datos
        tamano = ((high << 8) | low); // armo el tamaño de datos
        tamano *= 1024; // Multiplico por 1024 para pasarlo a bytes

        fread(registros, sizeof(int), 16, archivo); // Leo los registros del archivo
        // inicializar_registros(mv, registros); // Inicializo los registros de la máquina virtual
        fread(segmentos, sizeof(int), 8, archivo); // Leo los segmentos del archivo
        // inicializar_segmentos(mv, segmentos); // Inicializo los segmentos de la máquina virtual
        char memoria[tamano];
        fread(memoria, sizeof(char), tamano, archivo); // Leo los datos del archivo
        // inicializar_memoria(mv, memoria, tamano); // Inicializo la memoria de la máquina virtual
    }
    fclose(archivo);
    return 1;
}

/*
    Verifico que el tamaño de los datos leidos no sea mayor a el tamaño
    de memoria de la maquina virtual
*/
int verifico_tamano(char tamano) {
    if (tamano < TAMANO_MEMORIA) {
        // El tamaño de datos es correcto
        return 1;
    }
    else {
        // El tamaño de datos es incorrecto
        return 0;
    }
}