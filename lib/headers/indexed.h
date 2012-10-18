#include	"pf_const.h"
#include	"record.h"

int I_CREATE(const char* name, const campo * reg, const campo * primary_key);
int I_OPEN(const char* name, int mode);
int I_CLOSE(int handler);
int I_ADD_INDEX(int handler, const campo * key);
int I_DROP_INDEX(int handler, int index_id);
int I_IS_INDEX(int handler, const campo * key);
int I_START(int handler, int index_id, char * operator, const void * ref_reg);
int I_READ(int handler, void* reg);
int I_READNEXT(int handler, int index_id, void * reg);
int I_WRITE(int handler, const void * reg);
int I_UPDATE(int handler, const void * reg);
int I_DELETE(int handler, const void * reg);
int I_DESTROY(const char * name);
