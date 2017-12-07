/* 
 * DICTIONARY.H
 * 
 * Basically everything is taken from C Programming Language book
 *
 */

#define HASH 149 

#ifndef FIELD
#define FIELD
typedef struct field { 
	struct field *next;
	char *key;
	int value;
 } Field;
#endif

#ifndef DOCUMENT
#define DOCUMENT
typedef struct document {
	int docID;
	int version;
	Field **attributes;
	struct document *older; // For older documents
	struct document *next; // for the next in the hashtable
	} Document;
#endif



/* Install: Put (key,value) in hashtab)  */
Field *install(Field **,char *,int);
unsigned hash(char *);
Field *lookup(Field **,char *);
char *myStrdup(char *);

Document *newDocument(int);
void insertDocument(Document **,Document *);
