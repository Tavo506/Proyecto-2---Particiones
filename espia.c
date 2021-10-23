#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include<fcntl.h>

#ifndef MAX
#define MAX 100
#endif

//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;



//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;

//Otras variables

int tamano;
FILE *fptr;
Proceso *Memoria_Proceso = NULL;
Casilla *Memoria_Casilla = NULL;

//Funciones para el uso del espia 

const char* convertEstado(int id){

    switch(id){
        case 0:
            return "Con acceso a memoria";
        case 1:
            return "En ejecucion";
        case 2:
            return "Bloqueado";
        default:
            return "...";
    };
}

void visualizarMemoria(){       //Visualiza los espacios de la memoria simulada
    
    for(int i=0; i<tamano; i++){
        printf("\n\n------Celda %d------\n",i);
        if(Memoria_Casilla[i].estado == 1){
            printf("\n%d", Memoria_Casilla[i].proceso);
        }
    }
    printf("\n\n--------------------");
}


void visualizarProcesos(){

    printf("\nPID\tEstado");
    for(int i=0; i<tamano; i++){
        if(Memoria_Proceso[i].id != 0){
            printf("\n%d\t",Memoria_Proceso[i].id);   
            printf("%s",convertEstado(Memoria_Proceso[i].estado));
        }
    }
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
	tamano = getSize();
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
            visualizarMemoria();
            break;
        case 2:
            visualizarProcesos();
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