#include	"pf_const.h"
#include	"record.h"

int D_CREATE(const char* name, const campo * reg, const campo * key, int max_reg);
int D_OPEN(const char* name, int mode);
int D_CLOSE(int handler);
int D_READ(int handler, void* reg);
int D_WRITE(int handler, const void * reg);
int D_UPDATE(int handler, const void * reg);
int D_DELETE(int handler, const void * reg);
int D_DESTROY(const char * name);