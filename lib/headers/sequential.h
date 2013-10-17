#include	"pf_const.h"
#include	"record.h"

int S_CREATE(const char * name);
int S_OPEN(const char * name, int mode);
int S_READ(int handler, void * reg);
int S_CLOSE(int handler);
int S_WRITE(int handler, const void * reg, unsigned long cant);
int S_DESTROY(const char * name);

