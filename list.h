/*
 * 
 *
 */


#ifndef PARAM_NODE
#define PARAM_NODE
typedef struct param {
	char *field;
	char *operation;
	int value;
	struct param *next;
	} Pnode;
#endif

#ifndef PARAM_LIST
#define PARAM_LIST
typedef struct paramlist  {
	int size;
	Pnode *head;
	Pnode *tail;
	} PList;
#endif

#ifndef FIELD_LSIT
#define FIELD_LIST
typedef struct fieldList {
	int size;
	char **name;
	} Projection; 
#endif


// x is db entry, y is threshold
int compareIntegers (int,int,char *);

PList *newPList(void);
void insertParam(PList *,char *,char *,int);
void addToFront(PList *,char *);
void freeList(PList *);
