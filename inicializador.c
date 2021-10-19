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




void prepareMemory(int n){//Inicializa cada casilla con un 0 en su 'estado'
	for (i=0; i<n; i++)
	{
		Memoria[i].estado = 0;
		printf("Escrito %d\n", i);
	}
}

void guardarInput(int num) { //Funcion que guarda el tamanno de memoria elegido por el usuario.
    FILE *fptr;
	fptr = fopen("data.temp","w");

   	if(fptr == NULL)
   	{
    	printf("Error!");   
    	exit(1);             
	}

	fprintf(fptr,"%d",num);
	fclose(fptr);
}


/*

================================================

FUNCION PRINCIPAL

================================================

*/


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
	Memoria = getMemoryCasilla(Id_Memoria);
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
