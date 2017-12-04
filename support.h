/* Support structures for NoSQL 457 Project 
 *
 * Written by William Treutel in December 2017
 *
 * Dictionary is taken from The C Programming Language book
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
