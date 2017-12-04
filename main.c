#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strtok
#include "scanner.h"
#include "support.h"

int main (void) {
	FILE *in = fopen("data.txt","r");
	if (in == NULL) { printf("Data File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	char *florb;
	char *key; int val;
	int id = 0,dbSize = 4; // will also serve as count of documents

	Field ***db = malloc(sizeof(Field **) * 4),***temp;
	if (db == NULL) {
		printf("Allocation of database failed.\nExiting . . .\n");
		exit(1);
		}

	florb = readLine(in);

	// Read in a new document
	while (!feof(in)) {
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
		printf("====NEW DOCUMENT====\n");
		printf("KEY: _id / VALUE: %d\n",id);
		install(db[id],"_id",id);
		key = strtok(florb,": ");

		// Parse fields
		while (key != NULL) {
			printf("KEY: %s / ",key);
			val = atoi(strtok(NULL,": "));
			printf("VALUE: %d\n",val);
			install(db[id],key,val);

			key = strtok(NULL,": ");  // Get ready for next loop
			}
			
		id++; // Increment _id
		florb = readLine(in);
		}

	printf("ID: %d\n",lookup(db[0],"_id")->value);

	return 0;
	}

Field **newDoc (void) { return malloc(sizeof(Field *) * HASH); }
