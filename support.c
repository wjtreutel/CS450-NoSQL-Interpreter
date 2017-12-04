#include "support.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned hash(char *s) {
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++) {
		hashval = *s + 31 * hashval;
		 }
	return hashval % HASH;
	}

Field *lookup (Field **dict,char *s) {
	Field *np;

	for (np = dict[hash(s)]; np != NULL; np = np->next) {
		if (strcmp(s,np->key) == 0) return np;
		}
	return NULL;
	}

char *myStrdup(char *);

Field *install(Field **dict, char *key,int value) {
	Field *np;
	unsigned hashval;

printf("Installing (%s/%d)\n",key,value);
	
	/* If there's no prior entry, make one */
	if ((np = lookup(dict,key)) == NULL) {
		np = (Field *) malloc(sizeof(*np));
		if (np == NULL || (np->key = myStrdup(key)) == NULL) return NULL;
		
		hashval = hash(key);
		np->next = dict[hashval];
		dict[hashval] = np;
		}

	/* Assign value to new entry */
	/* A little simpler than the CPL algorithm, because the values aren't strings. */
	np->value = value;

	return np;
	}

char *myStrdup(char *s) {
	char *p;
	p = (char *) malloc(strlen(s)+1);
	if (p != NULL) strcpy(p,s);

	return p;
	}
