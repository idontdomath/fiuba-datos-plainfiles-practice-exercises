/* ************************************************************************* *
 * 1) (15 pts) (**) Se tiene un archivo indexado con información sobre los   *
 * productos que vende una determinado cadena de comercios:                  *
 * (codigo_producto, nombre, descripcion, precio, stock). A fin de crear     *
 * una campaña publicitaria se pide crear un archivo relativo que            *
 * contenga la información de todos los productos cuyo precio este entre     *
 * dos valores fijos dados y para los cuales el stock sea mayor a una cierta *
 * cantidad descartando aquellos productos cuya descripcion contiene la      *
 * palabra “oferta”. A este archivo relativo se lo quiere construir ordenado *
 * por precio (de mayor a menor). Utilizando las primitivas                  *
 * correspondientes programar en lenguaje “C” una rutina que cree el         *
 * archivo relativo a partir del indexado.                                   *
 * ************************************************************************* */

int main(int argc,char** argv){
	
	// Receiving 6 parameters. [programName,indexedFilename,destinationFilename,basePrice,topPrice,baseStock].
	if( argc != 6 ) return RES_ERROR;
	
	// Filenames.
	char* nameIndex = argv[1];
	char* nameDest = argv[2];
	
	// Stock and price information.
	int basePrice = argv[3];
	int topPrice = argv[4];	
	int baseStock = argv[5];

	// Schemes.
	campo schemeProduct[] = {{"CODIGO",UNSIGNED | INT,1},{"NOMBRE",CHAR,20},{"DESCRIPCION",CHAR,50},{"PRECIO",UNSIGNED | FLOAT,1},{"STOCK",UNSIGNED | INT,1},{0,0,0}};
	campo keySecondary[] = {{"PRECIO",UNSIGNED | FLOAT,1},{"STOCK",UNSIGNED | INT,1},{0,0,0}};

	size_t sizeProduct;
	sizeProduct = REG_SIZEOF(schemeProduct);
	
	// Ammount of records processed.
	int recordCount = 0;
	
	// Actual record information.
	void* actualProduct = (void*) malloc(sizeProduct);
	int actualPrice = 0;
	char* actualDescription = (char*) malloc(50*sizeof(char));
	
	// File descriptors and variables.
	int fdIdx;
	int fdRel;
	int fdSec;
	int indexId;
	
	// Opening indexed file and checking for errors.
	fdIdx = I_OPEN(nameIndex,READ);
	if( fdIdx == RES_NULL || fdIdx == RES_ERROR ){
		free(actualProduct);
		free(actualDescription);
		return RES_ERROR;
	}
	
	// Creting auxiliary sequential file and checking for errors.
	if( S_CREATE("aux.dat") != RES_OK ){
		free(actualProduct);
		free(actualDescription);
		I_CLOSE(fdIdx);
		return RES_ERROR;
	}
	
	// Opening auxiliary file and checking for errors.
	fdSec = S_OPEN("aux.dat",WRITE);
	if( fdSec == RES_NULL || fdSec == RES_ERROR ){
		free(actualProduct);
		free(actualDescription);
		I_CLOSE(fdIdx);
		S_DESTROY("aux.dat");
		return RES_ERROR;	
	}
	
	// Getting index id and checking for errors.
	indexId = I_IS_INDEX(fdIdx,keySecondary);
	if( indexId == RES_NO_EXISTE_INDICE ){
		
		indexId = I_ADD_INDEX(fdIdx,keySecondary);
		if( indexId == RES_ERROR ){
			free(actualProduct);
			free(actualDescription);
			I_CLOSE(fdIdx);
			S_CLOSE(fdSec);
			S_DESTROY("aux.dat");
			return RES_ERROR;
		}
		
	}
	
	// Starting sequential reading from base values and checking for errors.
	REG_SET(actualProduct,schemeProduct,"PRECIO",basePrice,"STOCK",baseStock);
	if( I_START(fdIdx,indexId,">",actualProduct == RES_ERROR ){
		free(actualProduct);
		free(actualDescription);
		I_CLOSE(fdIdx);
		S_CLOSE(fdSec);
		S_DESTROY("aux.dat");
		return RES_ERROR;		
	}
	
	int escape = 0;
	
	// Reading desired products.
	while( (I_READNEXT(fdIdx,indexId,actualProduct) == RES_OK) && (escape == 0) ){
		
		// Checking for top price condition.
		REG_GET(actualProduct,schemeProduct,"PRECIO",&actualPrice);
		if( actualPrice >= topPrice ) escape = 1;
		
		REG_GET(actualProduct,schemeProduct,"DESCRIPCION",actualDescription);
		if( (!containsSubstring(actualDescription,"oferta")) && (escape == 0) ){
			recordCount++;
			S_WRITE(fdSec,actualProduct,1);
		}
		
	}
	
	free(actualDescription);
	I_CLOSE(fdIdx);
	
	// Creating relative file and checking for errors.
	if( R_CREATE(nameDest,sizeProduct,recordCount) != RES_OK ){
		free(actualProduct);
		S_CLOSE(fdSec);
		S_DESTROY("aux.dat");
		return RES_ERROR;	
	}
	
	// Opening relative file and checking for errors.
	fdRel = R_OPEN(nameDest,WRITE);
	if( fdRel == RES_NULL || fdRel == RES_ERROR ){
		free(actualProduct);
		S_CLOSE(fdSec);
		S_DESTROY("aux.dat");
		R_DESTROY(nameDest);
		return RES_ERROR;
	}
	
	S_CLOSE(fdSec);
	// Opening auxiliary file in read mode and checking for errors.
	fdSec = S_OPEN("aux.dat",READ);
	if( fdSec == RES_NULL || fdSec == RES_ERROR ){
		free(actualProduct);
		S_CLOSE(fdSec);
		S_DESTROY("aux.dat");
		R_CLOSE(fdRel);
		R_DESTROY(nameDest);
		return RES_ERROR;
	}
	
	// Reading desired products.
	while( S_READ(fdSec,actualProduct) == RES_OK ){
		
		// Storing products from last to first (from n-1 to 0th position).
		R_WRITE(fdRel,--recordCount,actualProduct);
	
	}
	
	// Clean.
	free(actualProduct);
	S_CLOSE(fdSec);
	S_CLOSE(fdRel);
	S_DESTROY("aux.dat");
	
	// Program finished with no errors.
	return RES_OK;

}
