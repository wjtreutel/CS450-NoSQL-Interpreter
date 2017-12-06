/*
 * 
 *
 */


typedef struct param {
	char *field;
	char *operation;
	int value;
	struct param *next;
	} Pnode;

typedef struct paramlist  {
	int size;
	Pnode *head;
	Pnode *tail;
	} PList;

typedef struct fieldList {
	int size;
	char **name;
	} Projection; 


// x is db entry, y is threshold
int compareIntegers (int,int,char *);

PList *newPList(void);
void insertParam(PList *,char *,char *,int);
