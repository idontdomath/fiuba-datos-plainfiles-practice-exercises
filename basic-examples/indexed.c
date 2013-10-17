#include "../lib/headers/pf_const.h"
#include "../lib/headers/record.h"
#include "../lib/headers/indexed.h"
#include <stdlib.h>
#include <stdio.h>

//codigo de ejemplo de organizacion directa
//en general es verbose por motivos didacticos.
//se omite ejemplo de uso de D_DESTROY.

campo schema_students[] = {{"PADRON", INT, 1}, {"NOMBRE", CHAR, 20}, {"APELLIDO", CHAR, 40}, {0,0,0}};
campo schema_students_key[] = {{"PADRON", INT, 1}, {0,0,0}};

//creacion de un secuencial
int indexed_creation(const char * filename) {

	int res = I_CREATE(filename,schema_students,schema_students_key);

	if (res == RES_ERROR || res == RES_EXISTE) {
		return -1;
	}

	return 0;
}