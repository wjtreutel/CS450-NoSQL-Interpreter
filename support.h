/*
 * 
 *
 */


typedef struct parameterList {
	int size;
	char **fieldList;
	char **operationList;
	char **valueList;
	} Param;

typedef struct fieldList {
	int size;
	char **name;
	} Projection; 


// x is db entry, y is threshold
int compareIntegers (int,int,char *);
