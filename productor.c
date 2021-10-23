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

void leerDatosMemoria(int tamano){
	for (i=0; i<tamano; i++)
	{
		printf("Leido casilla %d\n", i);
		printf("Ocupado por: %d\n\n", Memoria_Casilla[i].proceso);
	}
}


/*

================================================

FUNCIONES

================================================

*/


//Retorna la lista de las memorias disponibles segun la memoria solicitada
void getEspacios(int pTamano){

}



int firstFit(int pTamano){
	int base = -1;
	int contador = 0;

	for (int i = 0; i < tamano; ++i)
	{
		if (Memoria_Casilla[i].estado == 0 && contador < pTamano)
		{

			contador ++;

		}else {

			if (contador >= pTamano)
			{
				base = i - contador;
				return base;
			}

			contador = 0;
		}
	}
	return base;
}



int bestFit(int pTamano){
	getEspacios(pTamano);
	return -1;
}



int worstFit(int pTamano){
	getEspacios(pTamano);
	return -1;
}



int getBase(int pTamano){
	int base = -1;

	switch (tipoAlgoritmo){
		
		case 1: 	//First Fit
			base = firstFit(pTamano);
			break;

		case 2: 	//Best Fit
			base = bestFit(pTamano);
			break;

		case 3: 	//Worst Fit
			base = worstFit(pTamano);
			break;		
	}
	return base;
}


// Agrega el proceso a la memoria
void alocarMemoria( int base, int id_Proceso ){

    for ( int i = base ; i < Memoria_Proceso[id_Proceso].tamano + base; i++ ){
        Memoria_Casilla[i].estado = 1;
        Memoria_Casilla[i].proceso = Memoria_Proceso[id_Proceso].id;
    }
}


// Quitar el proceso a la memoria
void desalocarMemoria( int base, int id_Proceso ){

    for ( int i = base ; i < Memoria_Proceso[id_Proceso].tamano + base; i++ ){
        Memoria_Casilla[i].estado = 0;
        Memoria_Casilla[i].proceso = -1;
    }
}


// Settea el proceso a los valores default (lo elimina)
void eliminarProceso(int id_Proceso){
	Memoria_Proceso[id_Proceso].id = 0;
	Memoria_Proceso[id_Proceso].base = -1;
	Memoria_Proceso[id_Proceso].tamano = -1;
	Memoria_Proceso[id_Proceso].tiempo = -1;
	Memoria_Proceso[id_Proceso].estado = -1;
}

void columnaReady( int id_Proceso ){

	//Debe haber otro semaforo aca y revisar si hay un spy

	int base;

	pthread_mutex_lock(&mutex);	//REGION CRÌTICA

	//Alocar proceso en memoria

		Memoria_Proceso[id_Proceso].estado = 0;

		//Analizar si cabe
		base = getBase(Memoria_Proceso[id_Proceso].tamano);

		if (base >= 0)
		{
			Memoria_Proceso[id_Proceso].base = base;	//Actualiza la base
			alocarMemoria(base, id_Proceso);			//Asigna la memoria al proceso

		}else{	//En caso de que no quepa el hilo se mata pero antes se hace unlock

			//Se escribe en la bitacora que no cupo
			printf("El proceso %d no cupo en memoria\n", Memoria_Proceso[id_Proceso].id);
			eliminarProceso(id_Proceso);
			pthread_mutex_unlock(&mutex);
			return;
		}


	pthread_mutex_unlock(&mutex);


	Memoria_Proceso[id_Proceso].estado = 1;	//Cuando entra en el sleep es que esta ejecutando

	printProceso(&Memoria_Proceso[id_Proceso]);

	sleep( Memoria_Proceso[id_Proceso].tiempo);

	Memoria_Proceso[id_Proceso].estado = 2;	//Cuando sale del sleep vuelve a esperar por la region critica


	pthread_mutex_lock(&mutex);	//REGION CRìTICA


		Memoria_Proceso[id_Proceso].estado = 0;
		desalocarMemoria(base, id_Proceso);	//Desalocar proceso en memoria
		eliminarProceso(id_Proceso);
		printf("Memoria liberada de %d\n", Memoria_Proceso[id_Proceso].id);

	pthread_mutex_unlock(&mutex);
}



void *prepararProceso(){

	int indice = buscarProcesoLibre();

	Memoria_Proceso[indice].id = syscall(SYS_gettid);

	int tamanoLinea = (rand() % (10 - 1 + 1)) + 1; //Obtiene un random entre 30-60

	int tiempo = (rand() % (60 - 20 + 1)) + 20; //Obtiene un random entre 30-60
	
	Memoria_Proceso[indice].tamano = tamanoLinea;

	Memoria_Proceso[indice].tiempo = tiempo;

	Memoria_Proceso[indice].estado = 2;

	columnaReady(indice);

}

void productorDeProcesos(){

	//while( 1 ){
	int nextProcess;

	for (int i = 0; i < 4; ++i){

		//Crear hilo
		pthread_t hilo;
		pthread_create(&hilo, NULL, prepararProceso, NULL);

		//nextProcess = (rand() % (60 - 30 + 1)) + 30; //Obtiene un random entre 30-60
		nextProcess = (rand() % (10 - 5 + 1)) + 5; //Obtiene un random entre 5-10 SOLO PARA PRUEBAS!!!!
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
	//leerDatos(tamano);



	printf( "Escoja el algoritmo a utilizar: \n\n 1. Best Fit \n 2. First fit \n 3. Worst Fit \n\n Debe ser un num entre 1, 2 o 3: ");

	scanf( "%d",&tipoAlgoritmo);

	printf("\n");


//Iniciar programa
	productorDeProcesos();


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