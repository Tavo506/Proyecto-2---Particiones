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

int getSize(){
	int num;
	if ((fptr = fopen("data.temp","r")) == NULL){
		printf("Error! opening file");

		// Program exits if the file pointer returns NULL.
		exit(1);
	}
	fscanf(fptr,"%d", &num);
	fclose(fptr);
	return num;
}

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
	setMemory(tamano);

	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Memoria
	shmctl (Id_Procesos, IPC_RMID, (struct shmid_ds *)NULL);//Liberar Procesos

	printf("Procesos finalizados y memoria liberada!! :D\n");
	return 0;
}


