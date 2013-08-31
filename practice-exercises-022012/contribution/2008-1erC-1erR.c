/* ************************************************************************* *
 * Sea un archivo secuencial maestro en donde cada registro indica           *
 * (cod_producto, precio, descripcion) y otro archivo secuencial de cambios  *
 * de la forma (cod_producto, precio, descripcion, operacion) en donde       *
 * operacion puede ser “A” (alta), “B” (baja) o “M” (modificacion). Ambos    *
 * archivos estan ordenados por cod_producto. A partir de ambos archivos se  *
 * quiere crear un nuevo archivo maestro que sea de organizacion directa.    *
 * Programar en lenguaje C un programa que se encargue de lo pedido          *
 * haciendo uso de las primitivas correspondientes de cada organizacion. (*) *
 * (15 pts)                                                                  *
 * ************************************************************************* */

int main(int argc,char** argv){
	
	// Schemes and key definitions.
	campo scheme_product[] = {{"CODIGO",UNISGNED | INT,1},{"PRECIO",UNSIGNED | FLOAT},{"DESCRIPCION",CHAR,30},{0,0,0}};
	campo scheme_modif[] = {{"CODIGO",UNISGNED | INT,1},{"PRECIO",UNSIGNED | FLOAT},{"DESCRIPCION",CHAR,30},{"OPERACION",CHAR,1},{0,0,0}};
	campo key[] = {{"CODIGO",UNSIGNED | INT,1},{0,0,0}};
	
	// File descriptors.
	int fd_master;
	int fd_modif;
	int fd_sec;
	int fd_dest;
	
	// Record variables.
	size_t size_prod = REG_SIZEOF(scheme_product);
	size_t size_modif = REG_SIZEOF(scheme_modif);
	void* record_product = (void*)(size_prod);
	void* record_modif = (void*)(size_modif);
	
	// Flux control variables.
	int read_master = 2;
	int status_master;
	int status_modif;
	int status_sec;
	int status_dest;
	
	// Product variables.
	char* action = (char*) malloc(sizeof(char));
	char* description = (char*) malloc(30*sizeof(char));
	unsigned int code_master = 0;
	unsigned int code_modif = 0;
	unsigned float price = 0;

	// Unique record counter.
	unsigned int record_counter = 0;
	
	// File opening and error verification.
	fd_master = S_OPEN("master.dat",READ);
	if( fd_master == RES_NULL || fd_master == RES_ERROR ){
	
		free(record_prduct);
		free(record_modif);
		free(action);
		free(description);
		return -1;

	}

	fd_modif = S_OPEN("modif.dat",READ);
	if( fd_modif == RES_NULL || fd_modif == RES_ERROR ){

		free(record_product);
		free(record_modif);
		free(action);
		free(description);
		S_CLOSE(fd_master);
		return -1;	

	}

	status_sec = S_CREATE("secondary.dat");
	if( status_sec != RES_OK ){

		free(record_product);
		free(record_modif);
		free(action);
		free(description);
		S_CLOSE(fd_master);
		S_CLOSE(fd_modif);
		return -1;

	}

	fd_sec = S_OPEN("secondary.dat",WRITE);
	if( fd_sec == RES_NULL || fd_sec == RES_ERROR ){

		free(record_product);
		free(record_modif);
		free(action);
		free(description);
		S_CLOSE(fd_master);
		S_CLOSE(fd_modif);
		S_DESTROY("secondary.dat");
		return -1;

	}
	
	// Begin file merge.
	while( read_master != -1 ){
		
		// Decide which file to read from.
		if( read_master == 1 || read_master == 2 )
			status_master = S_READ(fd_master,record_product);
		if( read_master == 0 || read_master == 2 )
			status_modif = S_READ(fd_modif,record_modif);
	
		// Next record variables.
		if( status_master == RES_OK )
			REG_GET(record_modif,scheme_master,"CODIGO",&code_master);
		else if( status_modif == RES_OK )
			REG_GET(record_modif,scheme_modif,"CODIGO",&code_modif,"OPERACION",action);
	
	
		// No more records to process.
		if( status_master != RES_OK && status_modif != RES_OK )
			read_master = -1;
		// No more actions in modifications file. Copy master file into secondary.
		else if( status_modif != RES_OK ){
			S_WRITE(fd_sec,record_product,1);
			read_master = 1;
			record_counter++;
		}

		// Record exists and action is to add new. Error handling.
		else if( RES_OK && code_master == code_modif && !strcmp("A",action) ) return -1;
	
		// Record does not exist and action is not add new. Error handling.
		else if( code_master > code_modif && strcmp("A",action) ) return -1;
	
		// Record modification.
		else if( code_master == code_modif && !strcmp("M",action) ){
	
			REG_GET(record_modif,scheme_modif,"PRECIO",&price,"DESCRIPCION",description);
			REG_SET(record_product,scheme_product,"PRECIO",price,"DESCRIPCION",description);
			S_WRITE(fd_sec,record_product,1);
			read_master == 2;
			record_counter++;

		}
	
		// Record add new action.
		else if( code_master > code_modif && !strcmp("A",action) ){
		
			REG_GET(record_modif,scheme_modif,"PRECIO",&price,"DESCRIPCION",description);
			REG_SET(record_product,scheme_product,"CODIGO",code_modif,"PRECIO",price,"DESCRIPCION",description);
			S_WRITE(fd_sec,record_product,1);
			read_master == 0;
			record_counter++;
	
		}
	
	}

	free(record_modif);
	free(description);
	free(action);

	S_CLOSE(fd_master);
	S_CLOSE(fd_modif);
	S_CLOSE(fd_sec);

	// Destination direct file size big enough to hold up to 50% more than the
	// ammount of records that we processed. Exercise does not talk about size constraints.
	status_dest = D_CREATE("dest.dir",scheme_product,key,record_counter*1.5);
	if( status_dest != RES_OK ){
	
		free(record_product);
		S_DESTROY("secondary.dat");
		return -1;

	}

	fd_dest = D_OPEN("dest.dir",WRITE);
	if( fd_dest == RES_NULL || fd_dest == RES_ERROR ){
	
		free(record_product);
		S_DESTROY("secondary.dat");
		D_DESTROY("dest.dir");
		return -1;

	}

	fd_sec = S_OPEN("secondary.dat",READ);
	if( fd_sec == RES_NULL || fd_sec == RES_ERROR ){

		free(record_product);
		S_DESTROY("secondary.dat");
		D_CLOSE(fd_dest);
		D_DESTROY("dest.dir");
		return -1;

	}

	// Read secondary file sequentially and write into the direct one, while there
	// are more registers to add.
	while( S_READ(fd_sec,record_product)==RES_OK ){
	
		if( D_WRITE(fd_dest,record_product)!=RES_OK ){
			free(record_product);
			S_CLOSE(fd_sec);
			D_CLOSE(fd_dest);
			S_DESTROY("secondary.dat");
			D_DESTROY("dest.dir");
			return -1;
		}

	}

	free(record_product);
	S_CLOSE(fd_sec);
	S_CLOSE(fd_dest);
	return 1;

}
