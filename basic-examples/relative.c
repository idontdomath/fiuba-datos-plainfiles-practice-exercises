#include "../lib/headers/pf_const.h"
#include "../lib/headers/record.h"
#include "../lib/headers/relative.h"
#include <stdlib.h>
#include <stdio.h>

//codigo de ejemplo de organizacion sequencial
//en general es verbose por motivos didacticos.
//se omite ejemplo de uso de R_DESTROY.
//se han suprimido algunas verificaciones de errores, 
//por estar en los ejemplos de secuencial

campo schema_students[] = {{"PADRON", INT, 1}, {"NOMBRE", CHAR, 20}, {"APELLIDO", CHAR, 40}, {0,0,0}};

//creacion de un sequencial
int relative_creation(const char * filename, int record_number) {

	int res = R_CREATE(filename, REG_SIZEOF(schema_students), record_number);

	if (res == RES_ERROR || res == RES_EXISTE) {
		return -1;
	}

	return 0;
}

//lectura por referencia en un relativo por numero de celda
int relative_read_by_cell_reference(const char * filename, int cell_number) {

	char[20] name;
	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = R_OPEN(filename, READ);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	//falta verificacion de errores
	R_READ(fd, cell_number, reg);
	REG_GET(reg, schema_students, "PADRON,NOMBRE", &student_id, &name);
	printf("%d %s", student_id, name);

	//liberamos recursos
	free(reg);
	R_CLOSE(fd);

	return 0;
}

//lectura secuencial en un relativo
int relative_sequential_read(const char * filename) {

	int student_id;
	char[20] name;
	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = R_OPEN(filename, READ);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	//posicionamos en record pointer en la primer celda
	int status == R_SEEK(fd, 0);
	if (status == RES_OK) {
		//leemos el primer registro al registro de intercambio
		status = R_READNEXT(fd,reg);
		while (status != RES_ERROR && status != RES_EOF) {	
			//extraemos los datos del registro de intercambio con REG_GET
			REG_GET(reg, schema_students, "PADRON,NOMBRE", &student_id, &name);
			printf("%d %s", strudent_id, name);
			status = R_READNEXT(fd,reg);
		}
	}

	//liberamos recursos usados
	free(reg);
	S_CLOSE(fd);

	if (status != RES_EOF) {
		//se produjo un error
		return -1;
	}

	return 0;
}

//escritura de un sequencial, la funcion escribe o actualiza la celda
int relative_write(const char * filename, int cell_number, int student_id, char * name, char * surname) {

	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = R_OPEN(filename, WRITE);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}
	//movemos los datos a escribir al registro de intercambio
	REG_GET(reg, schema_students, "PADRON,NOMBRE,APELLIDO", &student_id, &name, &surname);

	//escribimos en la celda pasada por parametros
	int status = R_WRITE(fd, cell_number, reg);
	if (status == RES_EXISTE) {
		//en este caso forzamos el update dado que la celda existe
		//en otros casos podrian querer hacer algo distinto
		status = R_UPDATE(fd, cell_number, reg);
	}
		
	//liberamos recursos usados
	free(reg);
	R_CLOSE(fd);

	if (status == RES_ERROR) {
		//se produjo un error
		return -1;
	}

	return 0;

}
