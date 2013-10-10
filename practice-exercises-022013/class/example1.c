#include "../../lib/headers/record.h"
#include "../../lib/headers/indexed.h"

/*
Un archivo indexado contiene informacion sobre la temperatura registrada en diferentes ciudades del 
mundo (ciudad, dia, hora, temperatura). Se quiere programar una funcion que reciba el nombre de una 
ciudad y devuelva cual es la hora promedio a la cual ocurre la temperatura maxima en dicha ciudad 
cada dia. Es decir que para cada dia se debe tomar la temperatura maxima para la ciudad elegida y 
luego realizar un promedio de la hora a la cual ocurrieron dichas temperaturas.
*/

/*
- suponemos tenemos que recorrer todos los registros de un indexado
- cortes de control por dia y ciudad
- suponemos que hay una unica temperatura maxima
- suponemos que las horas van de 0 a 23
*/
float temperature_report(char * filename, char * city_name) {

	campo schema_city[] = {{"CIUDAD", CHAR, 80}, {"DIA", INT, 1}, {"HORA", INT, 1},
							 {"TEMPERATURA", FLOAT, 1}, {0,0,0}};

	void * record = malloc(REG_SIZEOF(schema_city));

	fd_idx = I_OPEN(filename, READ);
	if (fd_idx == RES_NULL) {
		free(record);
		return -1;
	}

	//seteamos la informacion que nos dan de entrada
	REG_SET(record,schema_city,"CIUDAD,DIA,HORA",city_name,0,0);

	//usamos el indice primario dado que con el podemos acceder a la informacion
	//se podria hacer un check de control
	I_START(fd_idx, 0, ">=", record);

	int res;
	char[80] current_city = ''
	int current_day;
	int current_hour;
	int day;

	int max_temp = 0;
	int max_temp_hour = 0;

	int days_count = 0
	int max_temp_acum = 0 

	//leemos el primer registro
	//quedan pendientes controles de errores
	res = I_READNEXT(fd_idx, 0, record);	
	REG_GET(record,schema_city,"CIUDAD,DIA,HORA,TEMPERATURA",&current_city,&current_day,&current_hour,&current_temp);
	day = current_day;
	while (res != RES_EOF && current_city == city_name) {
		while (res != RES_EOF && current_city == city_name && current_day == day) {
			if (current_temp >= max_temp) {
				//encontramos una nueva temperatura maxima
				max_temp = current_temp;
				max_temp_hour = current_hour;
			}
			//quedan pendientes controles de errores
			res = I_READNEXT(fd_idx, 0, record);	
			REG_GET(record,schema_city,"CIUDAD,DIA,HORA,TEMPERATURA",&current_city,&current_day,&current_hour,&current_temp);
		}
		//dado que el dia cambio
		max_temp_acum += max_temp;
		days_count++;
		max_temp = 0;
		//imprimimos los resultados para ese dia
		day = current_day;
	}

	I_CLOSE(fd_idx);
	free(fd_idx);

	return max_temp_acum / days_count;

}