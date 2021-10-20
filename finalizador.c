#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;


//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;

FILE *fptr;


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

	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Memoria
	shmctl (Id_Procesos, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Procesos

	printf("Procesos finalizados y memoria liberada!! :D\n");
	return 0;
}


