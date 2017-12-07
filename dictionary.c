/*
 * DICTIONARY.C
 *
 * Taken from C Programming Language book
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dictionary.h"


/* DICTIONARY FUNCTIONS */

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



/* DOCUMENT / DATABASE FUNCTIONS */
Document *newDocument(int docID) {
	Document *new = malloc(sizeof(Document));

	new->docID = docID;
	new->version = 1;

	// Hash it into the database

	new->attributes = NULL;
	new->next = NULL;
	new->older = NULL;
	
	return new;
	}


void insertDocument(Document **db,Document *newDoc) {
	unsigned hashval = newDoc->docID % HASH;
	Document *iter,*prev = NULL; // prev stores tail value

	iter = db[hashval];

	/* If there's no prior entry, make one */
	if (iter == NULL) {
		db[hashval] = newDoc;
		return;
		}

	while (iter != NULL) {
		if (iter->docID == newDoc->docID) {
			newDoc->next = iter->next;
			newDoc->older = iter;
			newDoc->version = newDoc->older->version + 1;

			// Update database to point to newDoc
			if (prev == NULL) db[hashval] = newDoc;
			else prev->next = newDoc;
			return;
			}

		prev = iter;
		iter = iter->next;
		}

		prev->next = newDoc;

	return;
	}
		
