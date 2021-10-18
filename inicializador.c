#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include<fcntl.h>

//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;
Casilla* Memoria;
int i,j;

//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;

FILE *fptr;

key_t getKey(int mem_id){
	key_t clave;
	//Obtiene la key para la memoria compartida de la memoria :v
	clave = ftok ("/bin/ls", mem_id);
	if (Clave_Memoria == -1)
	{
		printf("No consigo clave para memoria compartida\n");
		exit(0);
	}
	return clave;
}

void setMemory(int tamano){

	//Crea el espacio para la memoria
	Id_Memoria = shmget (Clave_Memoria, sizeof(Casilla)*tamano, 0777 | IPC_CREAT);
	if (Id_Memoria == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}

	//Crea el espacio para la lista de procesos
	Id_Procesos = shmget (Clave_Memoria, sizeof(Proceso)*tamano, 0777 | IPC_CREAT);
	if (Id_Procesos == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}
}

Casilla* getMemory(int id_mem){ //Funcion que obtiene la memoria compartida
	
	Casilla* memoria = (Casilla *)shmat (id_mem, 0, 0);
	if (memoria == NULL)
	{
		printf("No consigo memoria compartida\n");
		exit(0);
	}
	return memoria;
}

void prepareMemory(int n){
	for (i=0; i<n; i++)
	{
		Memoria[i].estado = 0;
		printf("Escrito %d\n", i);
	}
}

void guardarInput(int num) {
	fptr = fopen("data.temp","w");

   	if(fptr == NULL)
   	{
    	printf("Error!");   
    	exit(1);             
	}

	fprintf(fptr,"%d",num);
	fclose(fptr);
}


int main()
{	//
	//	Conseguimos una clave para la memoria compartida. Todos los
	//	procesos que quieran compartir la memoria, deben obtener la misma
	//	clave. Esta se puede conseguir por medio de la función ftok.
	//	A esta función se le pasa un fichero cualquiera que exista y esté
	//	accesible (todos los procesos deben pasar el mismo fichero) y un
	//	entero cualquiera (todos los procesos el mismo entero).
	//

	Clave_Memoria = getKey(Memoria_id);//Obtener clave de Memoria
	Clave_Procesos = getKey(Procesos_id);//Obtener clave de Procesos

	//
	//	Creamos la memoria con la clave recién conseguida. Para ello llamamos
	//	a la función shmget pasándole la clave, el tamaño de memoria que
	//	queremos reservar (100 enteros en nuestro caso) y unos flags.
	//	Los flags son  los permisos de lectura/escritura/ejecucion 
	//	para propietario, grupo y otros (es el 777 en octal) y el 
	//	flag IPC_CREAT para indicar que cree la memoria.
	//	La función nos devuelve un identificador para la memoria recién
	//	creada.
	//	 

	int tamano;
    printf("Ingrese el tamaño de la memoria: ");
    scanf("%d", &tamano);

	setMemory(tamano);

	guardarInput(tamano);

	//
	//	Una vez creada la memoria, hacemos que uno de nuestros punteros
	//	apunte a la zona de memoria recién creada. Para ello llamamos a
	//	shmat, pasándole el identificador obtenido anteriormente y un
	//	par de parámetros extraños, que con ceros vale.
	//
	Memoria = getMemory(Memoria_id);
	//
	//	Ya podemos utilizar la memoria.
	//	Escribimos cosas en la memoria. Los números de 1 a 10 esperando
	//	un segundo entre ellos. Estos datos serán los que lea el otro
	//	proceso.
	//
	prepareMemory(tamano);
	//
	//	Terminada de usar la memoria compartida, la liberamos.
	//
	shmdt ((char *)Memoria);
	//shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
}
