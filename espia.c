#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include<fcntl.h>

//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;


//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;

FILE *fptr;

Proceso *Memoria_Proceso = NULL;
Casilla *Memoria_Casilla = NULL;

//Funciones para el uso del espia 

void visualizarMemoria(){

}

void imprimirOpciones(){
    printf("\n\n1. Visualizar estado de la memoria\n");
    printf("2. Visualizar estado de los procesos\n");
    printf("3. Finalizar espionaje\n");
}



int main(){


    Clave_Memoria = getKey(Memoria_id);//Obtener clave de Memoria
	Clave_Procesos = getKey(Procesos_id);//Obtener clave de Procesos

	//
	//	Igual que en p1.cc, obtenemos el id de la memoria. Al no poner
	//	el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está
	//	creada.
	//
	int tamano = getSize();
	Id_Memoria = setMemoryCasilla(Clave_Memoria, tamano);
	Id_Procesos = setMemoryProceso(Clave_Procesos, tamano);
     //    Una vez creada la memoria, hacemos que uno de nuestros punteros
    //    apunte a la zona de memoria recién creada. Para ello llamamos a
    //    shmat, pasándole el identificador obtenido anteriormente y un
    //    par de parámetros extraños, que con ceros vale.
    //
    Memoria_Casilla = getMemoryCasilla(Id_Memoria);
    Memoria_Proceso = getMemoryProceso(Id_Procesos);

    int opcion;
    int running = 1;

    printf("Bienvenid@, soy su programa espia, a continuacion estan las opciones que puedo realizar");
    while(running){
        imprimirOpciones();
        scanf("%d", &opcion);
        switch (opcion)
        {
        case 1:
            /* code */
            break;
        case 2:
            break;
        case 3:
            running = 0;
            printf("Fue un placer haberle servido, espia terminado\n");
            break;

        
        default:
            break;
        }
        
    }

    //    Desasociamos nuestro puntero de la memoria compartida. Suponemos
    //    que p1 (el proceso que la ha creado), la liberará.
    //
    if (Id_Memoria != -1)
    {
        shmdt ((char *)Memoria_Casilla);
    }
    if (Id_Procesos != -1)
    {
        shmdt ((char *)Memoria_Proceso);
    }
    return 0;
}