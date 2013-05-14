#include "../../lib/headers/record.h"
#include "../../lib/headers/indexed.h"
#include "../../lib/headers/sequential.h"
#include "../../lib/headers/relative.h"

/* 
	Ejercicio 1:

	Se tiene un archivo indexado que contiene informacion sobre libros que se venden en un negocio 
    (ISBN,Titulo,Autores,Precio,Stock).  
	Se quiere pasar la informacion a un archivo relativo la informacion de aquellos libros que tienen 
	stock quedando estos ordenados en el relativo por titulo.  
	Se pide un programa eficiente en C que realice lo pedido. 

*/
/*
	Estrategia de resolucion:
	* acceso al indexado por indice secundario, procesamiento de # de stock y cuenta de registros totales
	* uso de archivo secuencial de intercambio entre indexado y relativo.
	* creacion del relativo basado en total de registros y procesamiento desde el secuencial.
*/
int main(int argc, char** argv) {

	campo schema_book[] = {{"ISBN", INT, 1}, {"TITULO", CHAR, 50}, 
							{"AUTORES", CHAR, 50}, {"PRECIO", UNSIGNED | INT, 1}, 
							{"STOCK", UNSIGNED | INT, 1}, {0,0,0}};
	
	campo secondary_index[] = {{"TITULO", CHAR, 50}, {0,0,0}};

	//argv[1] contine el nombre del archivo indexado a procesar.
	//argv[2] contine el nombre del archivo relativo a crear.
	if (argc != 3) {
		return -1;
	}

	int record_counter = 0;
	int fd_idx, fd_sec, fd_rel;
	int sec_idx;
	int stock;
	void * record;

	//pedimos memoria para el registro de intercambio
	record = malloc(REG_SIZEOF(schema_book));
	if (record == NULL) {
		return -2; 
	}

	//argv[1] tiene el nombre fisico del archivo indexado 
	fd_idx = I_OPEN(argv[1], READ);
	if (fd_idx == RES_NULL) {
		free(record);
		return -1;
	}

	//pedido o creacion de indice secundario
	sec_idx = I_IS_INDEX(fd_idx, secondary_index);
	if (sec_idx == RES_NO_EXISTE_INDICE) {
		sec_idx = I_ADD_INDEX(fd_idx, secondary_index);
	}

	//creacion del secuencial de intercambio
	if (S_CREATE("temp.sec") == RES_ERROR) {	
		I_CLOSE(fd_idx);
		free(record);
		return -1	
	}

	fd_sec = S_OPEN("temp.sec",WRITE);
	if (fd_idx == RES_NULL) {
		I_CLOSE(fd_idx);
		free(record);
		return -1
	}

	//fijamos un valor en el registro para evitar que no este inicializado.
	REG_SET(record, schema_book, "TITULO", "");


	I_START(fd_idx, sec_idx, "<", record);
	//tener en cuenta el caso de RES_ERROR
	while (I_READNEXT(fd_idx, sec_idx, record) != RES_EOF) {
		REG_GET(record,schema_book,"STOCK",&stock);
		if (stock > 0) {
			S_WRITE(fd_sec, record, REG_SIZEOF(schema_book));
			record_counter++;
		}
	}	

	I_CLOSE(fd_idx);

	//argv[1] tiene el nombre fisico del archivo relativo 
	if (R_CREATE(argv[2], REG_SIZEOF(schema_book), 
			record_counter) == RES_ERROR) {
		//no elimino el secuencial por posible uso posterior
		free(record);
		return -1;	
	}

	//de aqui en adelante se pueden hacer mejoras con
	//validaciones similares.

	fd_rel = R_OPEN(argv[2], WRITE);

	S_CLOSE(fd_sec);
	fd_sec = S_OPEN("temp.sec",READ);

	record_counter = 0;

	while (S_READ(fd_sec, record) != RES_EOF) {
		R_WRITE(fd_rel, record_counter, record);
		record_counter++;
	}

	R_CLOSE(fd_rel);
	S_CLOSE(fd_sec);
	S_DESTROY("temp.sec");

	free(record); 

	return 0;
	
} 
	

