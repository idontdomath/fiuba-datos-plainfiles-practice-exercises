#include "../../lib/headers/record.h"
#include "../../lib/headers/direct.h"

/*
	Ejercicio 2:

	Una empresa decidió hace un tiempo usar un archivo directo para almacenar datos sobre sus 
	clientes, los mismos se identifican por un numero de 6 caracteres, originalmente el archivo directo 
	fue creado para una capacidad de 20000 clientes pero las cosas no han marchado tan bien como 
	se esperaba y se sabe que la cantidad de clientes reales es mucho menor. 
	A efectos de no desperdiciar tanto espacio se quiere reorganizar el archivo directo de forma tal que 
	la capacidad del mismo sea solamente un 25% mayor a la cantidad de clientes que actualmente 
	tiene la empresa. 

*/
/*
	Estrategia de resolucion:
	* prueba de todas las claves posibles del dominio de id de cliente dado que
	se desconoce que cliente existe en el archivo y que no, y cuenta de registros existentes.
	* uso de archivo secuencial de intercambio entre indexado y relativo.
	* creacion de nuevo directo con mas del 25% para poder asegurar altas (deberia calcularse).

*/

int main(int argc, char** argv) {

	campo schema_client[] = {{"ID", UNSIGNED | INT, 1}, {"NOMBRE", CHAR, 50}, {"APELLIDO", CHAR, 50},
							 {"EMAIL", CHAR, 80}, {0,0,0}};
	campo primary_key[] = {{"ID", UNSIGNED | INT, 1}, {0,0,0}};

	int client_key = 0; //puedo llegar hasta 999999
	int record_counter = 0;

	//argv[1] nombre del archivo directo original
	//argv[2] nombre del archivo directo de salida
	if (argc != 3) {
		return -1;
	}

	//pedimos memoria para el registro de intercambio
	record = malloc(REG_SIZEOF(schema_book));
	if (record == NULL) {
		return -2; 
	}

	//apertura del directo original
	fd_dir = int D_OPEN(argv[1], READ);
	if (fd_dir == RES_NULL) {
		free(record);
		return -3;
	}

	//creacion de archivo secuencial de intercambio
	if (S_CREATE("temp.sec") == RES_ERROR) {	
		D_CLOSE(fd_dir);
		free(record);
		return -4;	
	}

	//a partir podria hacerse mejoras con las validaciones pertinentes
	//similares a las anteriores
	fd_sec = S_OPEN("temp.sec",WRITE);

	while (client_key <= 999999) {
		REG_SET(record,schema_client,"ID",client_key);
		if (D_READ(fd_dir, record) != RES_NO_EXISTE) {
			//es posible agregar a la logica codigos de error
			//se mantiene por simplicidad asi
			S_WRITE(fd_sec, record, REG_SIZEOF(schema_client));
			record_counter++;
		}

		client_key++;
	}

	D_CLOSE(fd_dir);
	S_CLOSE(fd_sec);

	fd_sec = S_OPEN("temp.sec",READ);

	if (D_CREATE(argv[2], schema_client, primary_key, 
		record_counter + ceil(record_counter * 0.35)) == RES_ERROR) {
		//mantengo secuencial para posterior procesamiento 
		S_CLOSE(fd_sec);
		free(record);
		return -5;
	}

	fd_dir = D_OPEN(argv[2], WRITE);

	while (S_READ(fd_sec, record) != RES_EOF) {
		D_WRITE(fd_dir, record);
	}

	S_CLOSE(fd_sec);
	S_DESTROY("temp.sec");

	D_CLOSE(fd_dir);

	free(record);

	return 0;

	
} 