#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// La union ya está definida en sys/sem.h
#else
// Tenemos que definir la union
union semun 
{ 
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};
#endif

//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;


//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;

FILE *fptr;


//Variables para los semaforos#####
key_t Clave_Semaforo;
int Id_Semaforo;
struct sembuf Operacion;
union semun arg;


int main()
{
	//
	//	Igual que en p1.cc, obtenemos una clave para la memoria compartida
	//
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


	Clave_Semaforo = getKey(Semaforo_id);
    Id_Semaforo = setMemorySemaforo(Clave_Semaforo);


    Operacion.sem_num = 1;
    Operacion.sem_op = 2;
    Operacion.sem_flg = 0;

    semop (Id_Semaforo, &Operacion, 1);

	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Memoria
	shmctl (Id_Procesos, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Procesos

	printf("Procesos finalizados y memoria liberada!! :D\n");
	return 0;
}


