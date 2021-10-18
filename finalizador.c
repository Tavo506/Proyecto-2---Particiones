#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>


int main()
{
	key_t Clave;
	int Id_Memoria;


	//
	//	Igual que en p1.cc, obtenemos una clave para la memoria compartida
	//
	Clave = ftok ("/bin/ls", Memoria_id);
	if (Clave == -1)
	{
		printf("No consigo clave para memoria compartida");
		return 1;
	}

	//
	//	Igual que en p1.cc, obtenemos el id de la memoria. Al no poner
	//	el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está
	//	creada.
	//
	Id_Memoria = shmget (Clave, sizeof(int)*100, 0777 );
	if (Id_Memoria == -1)
	{
		printf("No consigo Id para memoria compartida");
		return 1;
	}


	shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
	return 0;
}


