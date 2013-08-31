/*  **************************************************************************** *
 *  Una empresa de seguros tiene un archivo indexado con informacion             *
 *  de sus clientes. (cod_cliente,nombres,apellido,domicilio,cod_postal, activo) *
 *  donde activo es "S" o "N" y un archivo indexado con informacion de las       *
 *  sucursales de la empresa (cod_sucursal,domicilio,cod_postal). Se quiere      *
 *  generar un listado que indique para cada cliente activo cual es la o las     *
 *  sucursales que le corresponden de acuerdo a su codigo postal.                *
 *  **************************************************************************** */

int clientListing(void){

	// Handlers de los archivos indexados.
	int fdClientes;
	int fdSucursales;
	
	// Archivo destino donde escribire el listado.
	FILE* destino = fopen("listado.txt",w);

	// Esquemas de los registros.
	campo esquemaCliente[] = {{"CODIGO",UNSIGNED | INT,5},{"NOMBRES",CHAR,20},{"APELLIDO",CHAR,20},{"DOMICILIO",CHAR,20},{"CP",UNSIGNED|INT,5},{"ACTIVO",CHAR,1}{0,0,0}};
	campo esquemaSucursal[] = {{"CODIGO",UNSIGNED | INT,5},{"DOMICILIO",CHAR,20},{"CP",UNSIGNED | INT,5},{0,0,0}};
	
	// Valores de los registros que voy a necesitar guardar.
	unsigned int cpCliente;
	unsigned int cpSucursal;
	unsigned int codigoSucursal;
	unsigned int codigoCliente;
	char* nombreCliente = (char*) malloc(20*sizeof(char));
	char* apellidoCliente = (char*) malloc(20*sizeof(char));
	char* domicilioSucursal = (char*) malloc(20*sizeof(char));
	
	// Pido memoria para los registros de el cliente y la sucursal que se esta leyendo.
	size_t tamanioCliente = REG_SIZEOF(esquemaCliente);
	size_t tamanioSucursal = REG_SIZEOF(esquemaSucursal);
	void* cliente = (void*) malloc(tamanioCliente);
	void* sucursal = (void*) malloc(tamanioSucursal);

	// Abro los archivos indexados.
	fdClientes = I_OPEN("clientes.idx",READ);
	fdSucursales = I_OPEN("sucursales.idx",READ);
	
	// Utilizo la clave secundaria de Codigo Postal para recorrer las sucursales.
	campo claveSec[] = {{"CP",UNSIGNED | INT,5},{0,0,0}};
	
	// El archivo de clientes lo recorro mediante la clave principal.
	int indexIdClientes = 0;
	
	// Me fijo si el indice ya existe en los archivos. Si en alguno no existia, lo creo.
	int indexIdSucursales = I_IS_INDEX(fdSucursales,claveSec);
	if( indexIdSucursales == RES_NO_EXISTE_INDICE ) I_ADD_INDEX(fdSucursales,claveSec);
	
	// El primer cliente tiene codigo > 0.
	REG_SET(cliente,esquemaCliente,"CODIGO",0);

	// No hay ningun cliente. Ningun codigo es >0. Salgo del programa.
	if( I_START(fdClientes,indexIdClientes,">",cliente) != RES_OK ) return 0;

	// Mientras haya clientes con codigo >0.
	while( I_READ(fdClientes,cliente) == RES_OK ){
		
		// Estas llamadas se pueden poner todas en una sola.
		REG_GET(cliente,esquemaCliente,"CP",&cpCliente);
		REG_GET(cliente,esquemaCliente,"CODIGO",&codigoCliente);
		REG_GET(cliente,esquemaCliente,"NOMBRES",nombreCliente);
		REG_GET(cliente,esquemaCliente,"APELLIDO",apellidoCliente);
		
		// Header del listado para cada cliente. Informacion del mismo. "Codigo - Apellido, Nombres (CP):"
		fwrite(destino,"%ud - %s, %s (%ud):\n",codigoCliente,apellidoCliente,nombreCliente,cpCliente);
		
		// Voy a tener que recorrer las sucursales que tengan el mismo codigo postal que el cliente.
		REG_SET(sucursal,esquemaSucursal,"CP",cpCliente);
		
		// El cliente no tiene ninguna sucursal en su area. Sigo con el proximo cliente.
		if( I_START(fdSucursales,indexIdSucursales,"=",sucursal) != RES_OK ){
			fwrite(destino,"Fuera del area de cobertura\n");
		}

		else{
			
			// Mientras haya sucursales con el mismo codigo postal que el cliente.
			while( I_READ(fdSucursales,sucursal) == RES_OK ){
			
				// Obtengo informacion acerca de las sucursales.
				REG_GET(sucursal,esquemaSucursal,"CODIGO",&codigoSucursal);
				REG_GET(sucursal,esquemaSucursal,"DOMICILIO",domicilioSucursal);
				// Escribo el codigo de la sucursal seguido por el domicilio de la misma. "Codigo - Domicilio"
				fwrite(destino,"%ud - %s\n",codigoSucursal,domicilioSucursal);
			
			}
			
		}	

	}

	fclose(destino);
	I_DROP_INDEX(fdClientes,indexIdClientes);
	I_DROP_INDEX(fdSucursales,indexIdSucursales);
	I_CLOSE(fdClientes);
	I_CLOSE(fdSucursales);
	free(cliente);
	free(sucursal);
	free(nombreCliente);
	free(apellidoCliente);
	free(domicilioSucursal);
	
	return 1;
	
}
