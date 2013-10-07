#include "../lib/headers/pf_const.h"
#include "../lib/headers/record.h"
#include "../lib/headers/sequential.h"
#include <stdlib.h>
#include <stdio.h>

//codigo de ejemplo de organizacion sequencial
//en general es verbose por motivos didacticos.
//se omite ejemplo de uso de S_DESTROY.

campo schema_students[] = {{"PADRON", INT, 1}, {"NOMBRE", CHAR, 20}, {"APELLIDO", CHAR, 40}, {0,0,0}};

//creacion de un secuencial
int sequential_creation(const char * filename) {

	int res = S_CREATE(filename);

	if (res == RES_ERROR || res == RES_EXISTE) {
		return -1;
	}

	return 0;
}

//lectura de un secuencial
int sequential_read(const char * filename) {

	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = S_OPEN(filename, READ);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	int student_id;
	char[20] name;

	int status = S_READ(fd,reg);
	while (status != RES_ERROR && status != RES_EOF) {	
		//luego de cada llamada a read la informacion se encuentra 
		//en el registro de intercambio
		//extraemos los datos del registro de intercambio con REG_GET
		REG_GET(reg, schema_students, "PADRON,NOMBRE", &student_id, &name);
		printf("%d %s", student_id, name);
		status = S_READ(fd,reg);
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

//escritura de un secuencial
int sequential_write(const char * filename, int student_id, char * name, char * surname) {

	void * reg = malloc(REG_SIZEOF(schema_students));

	int fd = S_OPEN(filename, APPEND);

	if (fd == RES_ERROR) {
		free(reg);
		return -1;
	}

	//movemos los datos a escribir al registro de intercambio
	REG_GET(reg, schema_students, "PADRON,NOMBRE,APELLIDO", &student_id, &name, &surname);

	int status = S_WRITE(fd, reg, REG_SIZEOF(schema_students));

	//liberamos recursos usados
	free(reg);
	S_CLOSE(fd);

	if (status == RES_ERROR) {
		//se produjo un error
		return -1;
	}

	return 0;

}
