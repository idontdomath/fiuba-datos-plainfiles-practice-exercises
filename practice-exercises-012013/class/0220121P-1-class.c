#include "../../lib/headers/record.h"
#include "../../lib/headers/indexed.h"
#include "../../lib/headers/relative.h"
#include "../../lib/headers/sequential.h"
#include <stdlib.h>
#include <stdio.h>

/*
Un observatorio astronomico cuenta con un archivo indexado con informacion sobre estrellas (numero de catalogo,nombre, color, magnitud, descripcion). Periodicamente se quiere generar un archivo relativo que dado un color y una magnitud (numero) almacene todas las estrellas de dicho color que tengan magnitud menor o igual a la indicada ordenado por magnitud de mayor a menor. Programar en lenguaje “C” un programa que usando las primitivas correspondientes genere el listado pedido. (***) (15 pts)
*/

int task(int color, int magnitude) {

	campo schema_stars[] = {{"CATALOGO", INT, 1}, {"NOMBRE",CHAR, 50}, {"COLOR", INT, 1}, {"MAGNITUD", INT, 1}, {"DESC", CHAR, 80}, {0,0,0}};

	campo secondary_index[] = {{"COLOR",INT,1}, {"MAGNITUD",INT,1}, {0,0,0}};

	//supongo que se guarda el registro como esta en el relativo
	int fd_idx, fd_rel, fd_sec;
	int record_counter = 0;
	int idx;

	int status;

	void * buffer_stars;
	buffer_stars = malloc(REG_SIZEOF(schema_stars));

	int cur_color, cur_magnitude;

	int cur_record;

	//apertura del indexado (faltan controler)
	fd_idx = I_OPEN("stars.idx", READ);

	//creacion del auxiliar (faltan controles)
	S_CREATE("temp.sec");
	fd_sec = S_OPEN("temp.sec",WRITE);  

	idx = I_IS_INDEX(fd_idx, secondary_index);
	//creacion de indice secundario
	if (idx == RES_NO_EXISTE_INDICE) {
		idx = I_ADD_INDEX(fd_idx, secondary_index);
	}

	REG_SET(buffer_stars, schema_stars, "COLOR", color, "MAGNITUD", magnitude);

	I_START(fd_idx, idx, "", buffer_stars);
	status = I_READNEXT(fd_idx, idx, buffer_stars);

	while (status != RES_EOF) {
		REG_GET(buffer_stars, schema_stars, "COLOR", &cur_color, "MAGNITUD", &cur_magnitude);
		
		if (cur_color != color || (cur_color == color && cur_magnitude > magnitude)) {
			break;
		}

		S_WRITE(fd_sec, buffer_stars, REG_SIZEOF(schema_stars));

		record_counter++;

	}

	S_CLOSE(fd_sec);
	I_CLOSE(fd_idx);

	fd_sec = S_OPEN("temp.sec",READ);  
	R_CREATE("output.rel", REG_SIZEOF(schema_stars), record_counter);
	R_OPEN("output.rel", WRITE);

	cur_record = record_counter;
	while (S_READ(fd_sec, buffer_stars) != RES_EOF) {
		//faltan controles de lectura/escritura
		R_WRITE(fd_rel, cur_record, buffer_stars);
		//se guarda de atras para adelante para lograr el ordenamiento
		//pedido
		cur_record--;
	}

	R_CLOSE(fd_rel);

	S_CLOSE(fd_sec);
	S_DESTROY("temp.sec");

	free(buffer_stars);

	return 0;
}

/**
Hay que armar (en caso de no existir) un indice secundario por color, magnitud e ir leyendo del mismo verificando la condicion de corte (cambio de color o que la magnitud sea mayor a la buscada). Se arma un archivo auxiliar y se cuentan los registros a almacenar. Luego se crea el relativo del tamaño indicado y leyendo el auxiliar se va escribiendo en el relativo de abajo hacia arriba para que quede ordenado de mayor a menor.
Si no usan el indice secundario descuento de 10 puntos. Si controlan mal la condicion de corte descuento de 5 puntos. Si queda mal ordenado el relativo final descuento de 5 puntos (acumulativos)
*/