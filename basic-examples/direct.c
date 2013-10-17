#include "../lib/headers/pf_const.h"
#include "../lib/headers/record.h"
#include "../lib/headers/direct.h"
#include <stdlib.h>
#include <stdio.h>

//codigo de ejemplo de organizacion directa
//en general es verbose por motivos didacticos.
//se omite ejemplo de uso de D_DESTROY.

campo schema_students[] = {{"PADRON", INT, 1}, {"NOMBRE", CHAR, 20}, {"APELLIDO", CHAR, 40}, {0,0,0}};
campo schema_students_key[] = {{"PADRON", INT, 1}, {0,0,0}};

//creacion de un secuencial
int direct_creation(const char * filename, int number_records) {

	int res = D_CREATE(filename,schema_students,schema_students_key,number_records);

	if (res == RES_ERROR || res == RES_EXISTE) {
		return -1;
	}

	return 0;
}

//lectura por referencia en un relativo por numero de celda
//
//IMPORTANTE: tengase en cuenta que si se quisieran obtener todos los registros
//del archivo directo, hay que pedir al directo todas las posibles claves con un ciclo, 
//verificando tambien los errores sobre claves que no puedan existir
int direct_read(const char * filename, int student_id) {

	char[20] name;
	void * reg = malloc(REG_SIZEOF(schema_students));
	int status;

	int fd = D_OPEN(filename, READ);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	//movemos al registro de intercambio el id del estudiante que queremos encontrar
	REG_SET(reg, schema_students,"PADRON",student_id);

	status = D_READ(fd, reg);

	if (status == RES_NO_EXISTE || status == RES_ERROR) {
		free(reg);
		D_CLOSE(fd);
		return -2;
	}
	
	//obtenemos los datos del registro de intercambio
	REG_GET(reg, schema_students, "PADRON,NOMBRE", &student_id, &name);
	printf("%d %s", student_id, name);

	//liberamos recursos
	free(reg);
	D_CLOSE(fd);

	return 0;
}


//escritura de un sequencial, la funcion escribe o actualiza la celda
int relative_write(const char * filename, int student_id, char * name, char * surname) {

	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = D_OPEN(filename, WRITE);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	//movemos los datos a escribir al registro de intercambio
	REG_GET(reg, schema_students, "PADRON,NOMBRE,APELLIDO", &student_id, &name, &surname);

	//escribimos en la celda pasada por parametros
	int status = D_WRITE(fd, reg);

	if (status == RES_ARCHIVO_LLENO) {
		free(reg);
		D_CLOSE(fd);
	}

	if (status == RES_EXISTE) {
		//en este caso forzamos el update dado que la celda existe
		//en otros casos podrian querer hacer algo distinto
		status = R_UPDATE(fd, reg);
	}
	
	//es necesario hacer validaciones de la operacion respecto a errores posibles

	//liberamos recursos usados
	free(reg);
	D_CLOSE(fd);

	return 0;

}