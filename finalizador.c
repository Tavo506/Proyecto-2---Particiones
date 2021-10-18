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


void getKeys(){

	//Obtiene la key para la memoria compartida de la memoria :v
	Clave_Memoria = ftok ("/bin/ls", Memoria_id);
	if (Clave_Memoria == -1)
	{
		printf("No consigo clave para memoria compartida\n");
		exit(0);
	}

	//Obtiene la key para la memoria compartida de los procesos
	Clave_Procesos = ftok ("/bin/ls", Procesos_id);
	if (Clave_Procesos == -1)
	{
		printf("No consigo clave para memoria compartida\n");
		exit(0);
	}

}



void setMemory(int tamano){

	//Crea el espacio para la memoria
	Id_Memoria = shmget (Clave_Memoria, sizeof(int)*tamano, 0777);
	if (Id_Memoria == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}

	//Crea el espacio para los procesos
	Id_Procesos = shmget (Clave_Procesos, sizeof(int)*tamano, 0777);
	if (Id_Procesos == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}

}



int main()
{



	//
	//	Igual que en p1.cc, obtenemos una clave para la memoria compartida
	//
	getKeys();

	//
	//	Igual que en p1.cc, obtenemos el id de la memoria. Al no poner
	//	el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está
	//	creada.
	//
	setMemory(100);


	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
	shmctl (Id_Procesos, IPC_RMID, (struct shmid_ds *)NULL);
	return 0;
}


