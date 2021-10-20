#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <pthread.h>


//Variables para la memoria
key_t Clave_Memoria;
int Id_Memoria;
Casilla *Memoria_Casilla = NULL;

int i,j;

//Variables para los procesos
key_t Clave_Procesos;
int Id_Procesos;
Proceso *Memoria_Proceso = NULL;

//Otras variables
FILE *fptr;
int tipoAlgoritmo; // 1 -> Best Fit | 2 -> First Fir | 3 -> Worst Fit
int tamano;

long conProcess = 0;

pthread_mutex_t mutex;



/*

================================================

FUNCIONES EXTRA

================================================

*/

int buscarProcesoLibre(){ //Busca un proceso vacio

	for ( int i = 0; i < tamano; i++ ){
		if ( !Memoria_Proceso[i].id ){
			return i;
		}
	}
	return -1; //Caso altamente improbable
}

void leerDatos(int tamano){
	for (i=0; i<tamano; i++)
	{
		printf("Leido %d\n", Memoria_Proceso[i].id);
	}
}


/*

================================================

FUNCIONES

================================================

*/



void columnaReady( int id_Proceso ){

	//Debe haber otro semaforo aca y revisar si hay un spy

	pthread_mutex_lock(&mutex);

	//Alocar proceso en memoria

		//Analizar si cabe

		//Actualizar id y estado

		//Return en caso de fallar.

	pthread_mutex_unlock(&mutex);

	sleep( Memoria_Proceso[id_Proceso].tiempo);

	pthread_mutex_lock(&mutex);

	//Desalocar proceso en memoria

	pthread_mutex_unlock(&mutex);
}

void prepararProceso(){

	int indice = buscarProcesoLibre();

	Memoria_Proceso[indice].id = syscall(SYS_gettid);

	int tamanoLinea = (rand() % (10 - 1 + 1)) + 1; //Obtiene un random entre 30-60

	int tiempo = (rand() % (60 - 20 + 1)) + 20; //Obtiene un random entre 30-60
	
	Memoria_Proceso[indice].tamano = tamanoLinea;

	Memoria_Proceso[indice].tiempo = tiempo;

	//columnaReady(indice);

}

void productorDeProcesos(){

	while( 1 ){

		//Crear hilo
		pthread_t hilo;
		//pthread_create(&hilo, NULL, func);
		int nextProcess = (rand() % (60 - 30 + 1)) + 30; //Obtiene un random entre 30-60
		sleep(nextProcess);
	}
}






/*

================================================

MAIN

================================================

*/

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

	tamano = getSize();
	Id_Memoria = setMemoryCasilla(Clave_Memoria, tamano);
	Id_Procesos = setMemoryProceso(Clave_Procesos, tamano);

	//
	//	Una vez creada la memoria, hacemos que uno de nuestros punteros
	//	apunte a la zona de memoria recién creada. Para ello llamamos a
	//	shmat, pasándole el identificador obtenido anteriormente y un
	//	par de parámetros extraños, que con ceros vale.
	//
	Memoria_Casilla = getMemoryCasilla(Id_Memoria);
	Memoria_Proceso = getMemoryProceso(Id_Procesos);

	//
	//	Vamos leyendo el valor de la memoria con esperas de un segundo
	//	y mostramos en pantalla dicho valor. Debería ir cambiando según
	//	p1 lo va modificando.
	//
	leerDatos(tamano);

	//leerDatos(tamano);

/*
	printf( "Escoja el algoritmo a utilizar: \n\n 1. Best Fit \n 2. First fit \n 3. Worst Fit \n\n Debe ser un num entre 0, 1 o 2: ");

	scanf( "%d",&tipoAlgoritmo);


//Iniciar programa
	productorDeProcesos();

*/
	//
	//	Desasociamos nuestro puntero de la memoria compartida. Suponemos
	//	que p1 (el proceso que la ha creado), la liberará.
	//
	if (Id_Memoria != -1)
	{
		shmdt ((char *)Memoria_Casilla);
	}
	if (Id_Procesos != -1)
	{
		shmdt ((char *)Memoria_Proceso);
	}
}