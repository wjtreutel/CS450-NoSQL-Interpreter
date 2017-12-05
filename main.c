#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strtok
#include "scanner.h"
#include "support.h"
#include "dictionary.h"

int main (void) {
	FILE *data = fopen("data.txt","r");
	if (data == NULL) { printf("Data File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	FILE *query = fopen("queries.txt","r");
	if (query == NULL) { printf("Query File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	char *florb;
	char *key; int val;
	int i,id = 0,dbSize = 4; // will also serve as count of documents
	int count = 0; // for counting function

	Field ***db = malloc(sizeof(Field **) * 4),***temp;
	if (db == NULL) {
		printf("Allocation of database failed.\nExiting . . .\n");
		exit(1);
		}

	florb = readLine(data);

	// Read in a new document
	while (!feof(data)) {
		if (id == dbSize) { 
			dbSize = dbSize * 2;
			temp = realloc(db,sizeof(Field **) * (dbSize));
			if (temp == NULL) {
				printf("Reallocation of database failed.\nExiting . . .\n");
				exit(1);
				}
			db = temp;
			}
		db[id] = malloc(sizeof(Field *) * HASH);
		install(db[id],"_id",id);
		key = strtok(florb,": ");

		// Parse fields
		while (key != NULL) {
			val = atoi(strtok(NULL,": "));
			install(db[id],key,val);

			key = strtok(NULL,": ");  // Get ready for next loop
			}
			
		id++; // Increment _id
		free(florb);
		florb = readLine(data);
		}

	printf("===QUERIES===\n");

	char *curr = readLine(query),*collection,*operation,*origQuery,*version;
	while(!feof(query)) {
		origQuery = myStrdup(curr);
		printf("Processing '%s'\n",curr);
		collection = strtok(curr,".");
		if (strcmp("final",collection) == 0) {
			operation = strtok(NULL,"(");

			if (strcmp(operation,"query") == 0) {
				printf("QUERY NOT IMPLEMENTED\n");
				/*
				curr = operation;
				curr = strtok(NULL,"[]=<>");
				printf("CURR: %s (%c)\n",curr,curr[4]);
				curr = strtok(NULL,"");
				printf("CURR2: %s\n",curr);
				*/
				}



			else if (strcmp(operation, "count") == 0) {
				count = 0;
				curr = operation;
				curr = myStrdup(strtok(NULL,"[]"));
				printf("CURR: %s\n",curr);

				version = strtok(NULL,"[]), ");
				if (version == NULL) printf("Use All Versions\n");
				else printf("Use the last %d versions\n",atoi(version));
				printf("CURR2: %s / Operation: %s\n",curr,operation);
				
				for (i = 0; i < id; i++) {
					if (lookup(db[i],curr) != NULL) count++;
					}
				if (count > 0) printf("%s: %d\n",curr,count);
				}



			else if (strcmp(operation, "sort") == 0) {
				printf("SORT not implemented.\n");
				}



			else if (strcmp(operation, "insert") == 0) {
				printf("INSERT not implemented.\n");
				// Maybe start reading key-value pairs, inserting them into a new document?
				// At the end, just up the id number
				}

			else { 
				printf("'%s' is not a valid operation.\n",origQuery);
				}

			printf("\n\n");
			}
				
		free(curr);
		free(origQuery);

		curr = readLine(query);
		}

	int j;
	for (i = 0; i < dbSize; i++) {
		for (j = 0; j < HASH; j++) {
			if (db[i][j] != NULL) { free(db[i][j]->key); free(db[i][j]); }
			}
			free(db[i]);
		}

	free(db);

	fclose(query);
	fclose(data);
	
	return 0;
	}

