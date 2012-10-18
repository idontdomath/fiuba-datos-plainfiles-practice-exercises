typedef struct campo
{
	const char * name;
	const type;
	int lenght;
};

#define	CHAR 		1
#define INT 		2
#define	LONG 		3
#define FLOAT 		4  
#define DOUBLE 		5
#define	UNSIGNED	0x80

int REG_SIZEOF(const campo);
int REG_SET(const void * buffer, const campo* esq, const char * desc, ...);
int REG_GET(const void *bufffer, const campo* esq, const char * desc, ...);
