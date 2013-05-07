#include "../../lib/headers/record.h"
#include "../../lib/headers/indexed.h"
#include "../../lib/headers/direct.h"
#include <stdlib.h>
#include <stdio.h>

/*
Se tiene un archivo indexado con la informacion de las notas en los parciales 
de los alumnos de Organizacion de Datos. Este archivo indexado tiene la 
siguiente informacion: Año, cuatrimestre, 
instancia (Parcial/Recuperatorio/2do Recuperatorio), padron, nota.

En un archivo directo, se tiene la informacion de los alumnos: padron, 
apellido, nombres, carrera. La clave de este archivo es padron. 

Se desea implementar una funcion que dado un año y cuatrimestre, 
devuelva un listado por pantalla con todas las notas de cada alumno 
(1 a 3 notas), de la forma: padron, apellido, nombres, [instancia,nota]. 
(**) (15 pts)
*/
//opcion de hacerlo con una clave primaria declarada/ordenada como necesitan
//si esta justificado
int generate_report(int year, int term) {

	campo schema_grades[] = {{"ANIO", INT, 1}, {"CUATRIMESTRE", INT, 1}, {"INSTANCIA", INT, 1}, {"PADRON", INT, 1}, {"NOTA", INT, 1}, {0,0,0}};

	campo schema_students[] = {{"PADRON", LONG, 1}, {"NOMBRES", CHAR, 80}, {"APELLIDO", CHAR, 80}, {"CARRERA", INT, 1}, {0,0,0}};
	
	campo secondary_index[] = {{"ANIO", INT, 1}, {"CUATRIMESTRE", INT, 1}, {"PADRON", LONG, 1}, {"INSTANCIA", INT, 1}, {0,0,0}};

	//FIXME: chequeos de errores
	void * buffer_grades = malloc(REG_SIZEOF(schema_grades));
	void * buffer_students = malloc(REG_SIZEOF(schema_students));

	//FIXME: chequeos de errores
	int fd_idx = I_OPEN("grades.idx", READ);
	int fd_dir = D_OPEN("students.dir", READ);

	//creacion del indice secundario de no existir
	int idx = I_IS_INDEX(fd_idx, secondary_index);
	if (idx == RES_NO_EXISTE_INDICE) {
		idx = I_ADD_INDEX(fd_idx, secondary_index);
		//podemos verificar que no se haya podido crear
	}


	REG_SET(buffer_grades, schema_grades, "ANIO, CUATRIMESTRE, PADRON, INSTANCIA", year, term, 0, 0);
	int last_student_id = -1;
	int a_year, a_term, a_student_id, a_instance, a_grade;
	char a_name[80] = "";
	char a_lastname[80] = "";

	I_START(fd_id, idx, ">=", buffer_grades);
	int status = I_READNEXT(fd_idx, idx, buffer_grades);
	
	while (status != RES_EOF) {
		REG_GET(buffer_grades, schema_grades, "ANIO, CUATRIMESTRE, PADRON, INSTANCIA, NOTA", &a_year, &a_term, &a_student_id, &an_instance, &a_grade);
		//corte de control
		if (a_year != year || a_term != term) {
			break;
		}

		if (last_student_id != a_student_id) {
			//en este caso es la primera ver que leemos ese padron
			//por eso leemos del directo
			REG_SET(buffer_students, schema_students, "PADRON", a_student_id);
			//podriamos verificar que no hay inconsistencia entre los archivos
			D_READ(fd_dir, buffer_students);
			REG_GET(buffer_students, schema_students, "NOMBRE,APELLIDOS", &a_name, &a_lastname);
			last_student_id = a_student_id;
		}
		//de la forma: padron, apellido, nombres, [instancia,nota]
		printf("%d, %s, %s, [%d, %d]\n",a_student_id, a_lastname, a_name, an_instance, a_grade);
		status = I_READNEXT(fd_idx, idx, buffer_grades);

	}

	I_CLOSE(fd_idx);
	D_CLOSE(fd_dir);

	free(buffer_grades);
	free(buffer_students);

	return 0;

}