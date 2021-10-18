#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;
int *Memoria = NULL;
int i,j;

//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;


void getKeys(){

	//Obtiene la key para la memoria compartida de la memoria :v
	Clave_Memoria = ftok ("/bin/ls", Memoria_id);
	if (Clave_Memoria == -1)
	{
		printf("No consigo clave para memoria compartida");
		exit(0);
	}

	//Obtiene la key para la memoria compartida de los procesos
	Clave_Procesos = ftok ("/bin/ls", Procesos_id);
	if (Clave_Procesos == -1)
	{
		printf("No consigo clave para memoria compartida");
		exit(0);	}

}



void setMemory(int tamano){

	//Crea el espacio para la memoria
	Id_Memoria = shmget (Clave_Memoria, sizeof(int)*tamano, 0777);
	if (Id_Memoria == -1)
	{
		printf("No consigo Id para memoria compartida");
		exit(0);	}

	//Crea el espacio para los procesos
	Id_Procesos = shmget (Clave_Procesos, sizeof(int)*tamano, 0777);
	if (Id_Procesos == -1)
	{
		printf("No consigo Id para memoria compartida");
		exit(0);	}

}




void getMemory(){
	
	Memoria = (int *)shmat (Id_Memoria, (char *)0, 0);
	if (Memoria == NULL)
	{
		printf("No consigo memoria compartida");
		exit(0);	}

}



int main()
{
	


	//
	//	Conseguimos una clave para la memoria compartida. Todos los
	//	procesos que quieran compartir la memoria, deben obtener la misma
	//	clave. Esta se puede conseguir por medio de la función ftok.
	//	A esta función se le pasa un fichero cualquiera que exista y esté
	//	accesible (todos los procesos deben pasar el mismo fichero) y un
	//	entero cualquiera (todos los procesos el mismo entero).
	//
	getKeys();


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
	setMemory(100);


	//
	//	Una vez creada la memoria, hacemos que uno de nuestros punteros
	//	apunte a la zona de memoria recién creada. Para ello llamamos a
	//	shmat, pasándole el identificador obtenido anteriormente y un
	//	par de parámetros extraños, que con ceros vale.
	//
	getMemory();

	//
	//	Vamos leyendo el valor de la memoria con esperas de un segundo
	//	y mostramos en pantalla dicho valor. Debería ir cambiando según
	//	p1 lo va modificando.
	//
	for (i=0; i<10; i++)
	{
		printf("Leido %d\n", Memoria[i]);
		sleep (1);
	}


	//
	//	Desasociamos nuestro puntero de la memoria compartida. Suponemos
	//	que p1 (el proceso que la ha creado), la liberará.
	//
	if (Id_Memoria != -1)
	{
		shmdt ((char *)Memoria);
	}
}