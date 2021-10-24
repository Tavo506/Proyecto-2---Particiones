#include "proy2.h"
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include<fcntl.h>
#include <assert.h>

const int Memoria_id = 33;
const int Procesos_id = 69;

/*

================================================

FUNCIONES PARA MEMORIA COMPARTIDA

================================================

*/


key_t getKey(int mem_id){ //Funcion que obtiene la key para la memoria compartida
	key_t clave;
	//Obtiene la key para la memoria compartida de la memoria :v
	clave = ftok ("/bin/ls", mem_id);
	if (clave == -1)
	{
		printf("No consigo clave para memoria compartida\n");
		exit(0);
	}
	return clave;
}

int setMemoryCasilla(key_t Clave_Memoria, int tamano){//Crea la memoria compartida

	//Crea el espacio para la memoria
	int Id_Memoria = shmget (Clave_Memoria, sizeof(Casilla)*tamano, 0777);
	if (Id_Memoria == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}
    return Id_Memoria;

}

int setMemoryProceso(key_t Clave_Procesos, int tamano){

    //Crea el espacio para la lista de procesos
	int Id_Procesos = shmget (Clave_Procesos, sizeof(Proceso)*tamano, 0777);
	if (Id_Procesos == -1)
	{
		printf("No consigo Id para memoria compartida\n");
		exit(0);
	}
    return Id_Procesos;
}

Casilla* getMemoryCasilla(int id_mem){ //Funcion que obtiene la memoria compartida
	
	Casilla* memoria = (Casilla *)shmat (id_mem, 0, 0);
	if (memoria == NULL)
	{
		printf("No consigo memoria compartida\n");
		exit(0);
	}
	return memoria;
}

Proceso* getMemoryProceso(int id_mem){ //Funcion que obtiene la memoria compartida
	
	Proceso* memoria = (Proceso *)shmat (id_mem, 0, 0);
	if (memoria == NULL)
	{
		printf("No consigo memoria compartida\n");
		exit(0);
	}
	return memoria;
}




/*

================================================

FUNCIONES PARA MANEJO DE ARCHIVOS

================================================

*/
FILE* abrirArchivo(const char* nombre, char *modo){
	FILE *archivo;
	if ((archivo = fopen(nombre,modo)) == NULL){
		printf("Error! opening file");
		exit(1);
	}
	return archivo;
}
void encabezadoBitacora(){
	FILE* fichero;     
	fichero = abrirArchivo("bitacora.txt","w");
	fprintf (fichero, "%s","PID\t\tTipo\t\t\tAccion\t\t\t\tHora\t\t\t\t\t\tLineas asignadas");
    fclose(fichero);
}

int getSize(){
    FILE *fptr;
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

/*

==================================================

Funciones manejo de horas

==================================================


*/

char* getTime(){

	time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    assert(strftime(s, sizeof(s), "%c", tm));
	char *time = s;
	return time;

}
/*

================================================

FUNCIONES PARA LOS PROCESOS

================================================

*/


void printProceso(Proceso *p){
	printf(" id: %d\n tamaño: %d\n tiempo: %d\n base: %d\n estado: %d\n\n", p->id, p->tamano, p->tiempo, p->base, p->estado);
}




