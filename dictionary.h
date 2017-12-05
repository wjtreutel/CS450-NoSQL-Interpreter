/* 
 * DICTIONARY.H
 * 
 * Basically everything is taken from C Programming Language book
 *
 */

#define HASH 149 

typedef struct field { 
	struct field *next;
	char *key;
	int value;
 } Field;

unsigned hash(char *);
Field *lookup(Field **,char *);
char *myStrdup(char *);

/* Install: Put (key,value) in hashtab)  */
Field *install(Field **,char *,int);
