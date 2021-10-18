const int Memoria_id = 33;
const int Procesos_id = 69;

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