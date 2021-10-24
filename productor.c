#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <pthread.h>


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

tpuntero cabeza; //Indica la cabeza de la lista enlazada, si la perdemos no podremos acceder a la lista


//Variables para los semaforos#####
key_t Clave_Semaforo;
int Id_Semaforo;
struct sembuf Operacion;
struct sembuf OperacionFinalizar;
union semun arg;


int seguir = 1;


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




/*

================================================

FUNCIONES

================================================

*/

void agregarEspacio(tpuntero *cabeza, int base, int tamano){
	tpuntero nuevo; //Creamos un nuevo nodo
    nuevo = malloc(sizeof(Espacio)); //Utilizamos malloc para reservar memoria para ese nodo
    nuevo->base = base; //Le asignamos el valor ingresado por pantalla a ese nodo
    nuevo->tamano = tamano; //Le asignamos el valor ingresado por pantalla a ese nodo
    nuevo->next = *cabeza; //Le asignamos al siguiente el valor de cabeza
    *cabeza = nuevo; //Cabeza pasa a ser el ultimo nodo agregado
}


//Retorna la lista de las memorias disponibles segun la memoria solicitada
void getEspacios(int pTamano){
	

	int base = -1;
	int contador = 0;

	for (int i = 0; i < tamano; ++i)
	{
		if (Memoria_Casilla[i].estado == 0)
		{

			contador ++;

		}if (i+1 == tamano) {

			if (contador >= pTamano)
			{
				Espacio *hueco = malloc(sizeof(Espacio));


				base = i+1 - contador;
				printf("(%d, %d)\n",base, contador);
				agregarEspacio(&cabeza, base, contador);
			}

		}if (Memoria_Casilla[i].estado == 1){
			if (contador >= pTamano)
			{
				Espacio *hueco = malloc(sizeof(Espacio));


				base = i - contador;
				printf("(%d, %d)\n",base, contador);
				agregarEspacio(&cabeza, base, contador);
			}

			contador = 0;
		}
	}
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


void borrarLista(tpuntero *cabeza){ 
    tpuntero actual; //Puntero auxiliar para eliminar correctamente la lista
  
    while(*cabeza != NULL){ //Mientras cabeza no sea NULL
        actual = *cabeza; //Actual toma el valor de cabeza
        *cabeza = (*cabeza)->next; //Cabeza avanza 1 posicion en la lista
        free(actual); //Se libera la memoria de la posicion de Actual (el primer nodo), y cabeza queda apuntando al que ahora es el primero
    }
}


int bestFit(int pTamano){
	
	getEspacios(pTamano);
	tpuntero aux = cabeza;
	int base = -1;
	int min;

	if (cabeza != NULL)
	{
		min = cabeza->tamano;
		base = aux->base;

		while(aux != NULL){

			if(aux->tamano < min)
			{
				min = aux->tamano;
				base = aux->base;
			}
			aux = aux->next;
		}
		borrarLista (&cabeza);
	}
	return base;
}



int worstFit(int pTamano){
	getEspacios(pTamano);
	tpuntero aux = cabeza;
	int base = -1;
	int max;

	if (cabeza != NULL)
	{
		max = cabeza->tamano;
		base = aux->base;

		while(aux != NULL){

			if(aux->tamano > max)
			{
				max = aux->tamano;
				base = aux->base;
			}
			aux = aux->next;
		}
		borrarLista (&cabeza);
	}
	return base;
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


void waitS(){
	Operacion.sem_op = -1;

	semop (Id_Semaforo, &Operacion, 1);
}

void signalS(){
	Operacion.sem_op = 1;

	semop (Id_Semaforo, &Operacion, 1);
}


void agregarBitacora(int id_Proceso, char* mensaje, int success){
	FILE* fichero;     
			fichero = abrirArchivo("bitacora.log","a+");
			if (success)
			{
				fprintf (fichero, mensaje,
					Memoria_Proceso[id_Proceso].id,
					getTime(),
					Memoria_Proceso[id_Proceso].base,
					Memoria_Proceso[id_Proceso].base + Memoria_Proceso[id_Proceso].tamano);
			}else{
				fprintf (fichero, mensaje,
					Memoria_Proceso[id_Proceso].id,
					getTime(),
					"na");
			}
			
    		fclose(fichero);

}



/*

##################################################################

REGIONES CRITICAS

###################################################################

*/


void columnaReady( int id_Proceso ){

	//Debe haber otro semaforo aca y revisar si hay un spy

	int base;

	pthread_mutex_lock(&mutex);	//REGION CRÌTICA

	waitS();

	//Alocar proceso en memoria

		Memoria_Proceso[id_Proceso].estado = 0;

		//Analizar si cabe
		base = getBase(Memoria_Proceso[id_Proceso].tamano);

		if (base >= 0)
		{
			Memoria_Proceso[id_Proceso].base = base;	//Actualiza la base
			alocarMemoria(base, id_Proceso);			//Asigna la memoria al proceso
			agregarBitacora(id_Proceso,"\n%d\t\tAsignacion    \t\tAlocado en memoria\t\t%s\t\t(%d,%d)", 1);

		}else{	//En caso de que no quepa el hilo se mata pero antes se hace unlock

			//Se escribe en la bitacora que no cupo
			printf("El proceso %d no cupo en memoria\nNecesitaba %d espacio\n\n", Memoria_Proceso[id_Proceso].id, Memoria_Proceso[id_Proceso].tamano);
			agregarBitacora(id_Proceso,"\n%d\t\tCancelado     \t\tSin espacio suficiente\t\t%s\t\t%s", 0);
			eliminarProceso(id_Proceso);

			signalS();
			pthread_mutex_unlock(&mutex);
			return;
		}

	signalS();

	pthread_mutex_unlock(&mutex);


	Memoria_Proceso[id_Proceso].estado = 1;	//Cuando entra en el sleep es que esta ejecutando

	printProceso(&Memoria_Proceso[id_Proceso]);

	sleep( Memoria_Proceso[id_Proceso].tiempo);

	Memoria_Proceso[id_Proceso].estado = 2;	//Cuando sale del sleep vuelve a esperar por la region critica


	pthread_mutex_lock(&mutex);	//REGION CRìTICA

		waitS();

		Memoria_Proceso[id_Proceso].estado = 0;
		desalocarMemoria(base, id_Proceso);	//Desalocar proceso en memoria
		agregarBitacora(id_Proceso,"\n%d\t\tDesasignacion\t\tLiberando memoria\t\t%s\t\t(%d,%d)", 1);
		printf("Memoria liberada de %d\n", Memoria_Proceso[id_Proceso].id);
		eliminarProceso(id_Proceso);

		signalS();
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


void pararProcesos(){

}



void * checkParar(){

	semop (Id_Semaforo, &OperacionFinalizar, 1);

	seguir = 0;
	exit(0);

}



void productorDeProcesos(){
	
	int nextProcess;

	pthread_t hiloParar;
	pthread_create(&hiloParar, NULL, checkParar, NULL);

	while( seguir ){
	//for (int i = 0; i < 5; ++i){

		//Crear hilo
		pthread_t hilo;
		pthread_create(&hilo, NULL, prepararProceso, NULL);

		//nextProcess = (rand() % (60 - 30 + 1)) + 30; //Obtiene un random entre 30-60
		nextProcess = (rand() % (10 - 5 + 1)) + 5; //Obtiene un random entre 5-10 SOLO PARA PRUEBAS!!!!
		sleep(nextProcess);
	}

	pararProcesos();
}






/*

================================================

MAIN

================================================

*/

int main()
{

	Clave_Memoria = getKey(Memoria_id);//Obtener clave de Memoria
	Clave_Procesos = getKey(Procesos_id);//Obtener clave de Procesos
	

	tamano = getSize();
	Id_Memoria = setMemoryCasilla(Clave_Memoria, tamano);
	Id_Procesos = setMemoryProceso(Clave_Procesos, tamano);

	
	Memoria_Casilla = getMemoryCasilla(Id_Memoria);
	Memoria_Proceso = getMemoryProceso(Id_Procesos);

	
	Clave_Semaforo = getKey(Semaforo_id);
	Id_Semaforo = setMemorySemaforo(Clave_Semaforo);

	//
	//	Se inicializa el semáforo con un valor conocido. Si lo ponemos a 0,
	//	es semáforo estará "rojo". Si lo ponemos a 1, estará "verde".
	//	El 0 de la función semctl es el índice del semáforo que queremos
	//	inicializar dentro del array de 10 que hemos pedido.
	//
	arg.val = 1;
	semctl (Id_Semaforo, 0, SETVAL, &arg);

	Operacion.sem_num = 0;
	//Operacion.sem_op = -1;	Esto irá cambiando en los wait y signal, por lo que no vale de nada ponerlo aquí
	Operacion.sem_flg = 0;


	OperacionFinalizar.sem_num = 1;
	OperacionFinalizar.sem_op = -2;	
	OperacionFinalizar.sem_flg = 0;


	printf( "Escoja el algoritmo a utilizar: \n\n 1. First Fit \n 2. Best fit \n 3. Worst Fit \n\n Debe ser un num entre 1, 2 o 3: ");

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