#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int Memoria_id;
const int Procesos_id;

//Funciones en el inicializador
key_t getKey(int mem_id);
int setMemoryCasilla(key_t Clave_Memoria, int tamano);
int setMemoryProceso(key_t Clave_Procesos, int tamano);
int getSize();


typedef struct linea
{
	
	char estado;	//0 -> Libre	1 -> Ocupado
	int linea;
	int proceso;

} Casilla;


typedef struct process 
{

	pid_t id;
	int base;
	int tamano;
	int tiempo;

	char estado;	//0 -> En región crítica (accediendo a memoria)
					//1	-> Ejecutando (en el sleep)
					//2 -> Bloqueado (en el wait)

} Proceso;


typedef struct espacios
{

	int base;
	int tamano;

	struct espacios *next;

} Espacio;

typedef Espacio *tpuntero; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros

Casilla* getMemoryCasilla(int id_mem);
Proceso* getMemoryProceso(int id_mem);
void printProceso(Proceso *p);
void getEspacios(int pTamano);
void agregarEspacio(tpuntero *cabeza, int base, int tamano);