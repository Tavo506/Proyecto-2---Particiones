#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int Memoria_id = 33;
const int Procesos_id = 69;

//Funciones en el inicializador
key_t getKey(int mem_id);
int setMemoryCasilla(key_t Clave_Memoria, int tamano);
int setMemoryProceso(key_t Clave_Procesos, int tamano);
Casilla* getMemoryCasilla(int id_mem);
Proceso* getMemoryProceso(int id_mem);
int getSize();

typedef struct linea
{
	
	char estado;	//0 -> Libre	1 -> Ocupado
	int linea;
	int proceso;

} Casilla;


typedef struct process 
{

	int id;
	int base;
	int tamano;
	int tiempo;

	char estado;	//0 -> En región crítica (accediendo a memoria)
					//1	-> Ejecutando (en el sleep)
					//2 -> Bloqueado (en el wait)

} Proceso;