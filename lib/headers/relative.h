#include	"pf_const.h"
#include	"record.h"

int R_CREATE(const char* name, int size_reg, int max_reg);
int R_OPEN(const char* name, int mode);
int R_CLOSE(int handler);
int R_SEEK(int handler, int nrec);
int R_READ(int handler, int nrec, void* reg);
int R_READNEXT(int handler, void * reg);
int R_WRITE(int handler, int nrec, const void * reg);
int R_UPDATE(int handler, int nrec, const void * reg);
int R_DELETE(int handler, int nrec);
int R_DESTROY(const char * name);
int R_GETMAXREGS(int handler);



