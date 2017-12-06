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

	char *curr;
	char *key; int val;
	int i,j,id = 0,dbSize = 4; // will also serve as count of documents
	int count = 0,x; // for counting function. x is just a handy temp variable

	Field ***db = malloc(sizeof(Field **) * 4),***temp;
	if (db == NULL) {
		printf("Allocation of database failed.\nExiting . . .\n");
		exit(1);
		}



	/* Process the DATA.TXT file, make database */
	curr = readLine(data);

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
		key = strtok(curr,": ");

		// Parse fields
		while (key != NULL) {
			val = atoi(strtok(NULL,": "));
			install(db[id],key,val);

			key = strtok(NULL,": ");  // Get ready for next loop
			}
			
		id++; // Increment _id
		free(curr);
		curr = readLine(data);
		}




	/* Process the QUERIES.TXT file */

	curr = readLine(query);
	char *collection,*operation,*origQuery,*version,*fieldName,*threshold,*queryOp;
	PList *conditions; Pnode *iter;

	while(!feof(query)) {
		origQuery = myStrdup(curr);
		printf("Processing '%s'\n",curr);
		printf("=====QUERY=====\n");
		
		/* PARSE COLLECTION NAME */
		collection = strtok(curr,".");
		if (strcmp("final",collection) == 0) {
			operation = strtok(NULL,"(");
			curr = operation;
			curr = curr + 7; // Get past the "query(["

			/* PARSE OPERATION NAME */ 
			if (strcmp(operation,"query") == 0) {
				conditions = newPList();
				while (curr[0] != ',') {
						if (curr[0] == ']') break;
						else if (curr[0] == ' ') curr++; // Get rid of the space that comes from th
						// Get the field, queryOp, and value for each condition
						queryOp = strpbrk(curr,"<=>"); // Operation is being reused here
						fieldName = myStrdup(curr);
						fieldName = strtok(fieldName,"<=>");
						threshold = myStrdup(fieldName);
						threshold = strtok(NULL,",] ");


						// Find out what the queryOp is
						switch (queryOp[0]) {
							case '<':
								if (queryOp[1] == '=') 
									queryOp = "<=";
								else if (queryOp[1] == '>')
									queryOp = "<>";
								else
									queryOp = "<";
								break;
							case '>':
								if (queryOp[1] == '=') 
									queryOp = ">=";
								else queryOp = ">";
								break;
							case '=':
								queryOp = "=";
								break;
							default:
								printf("Error! '%c' is not a valid queryOp!\n",operation[0]);
							}
						
				
						insertParam(conditions,fieldName,queryOp,atoi(threshold));
						curr = strtok(NULL,"");
						}


				printf("Time to process the projection\n");
				curr = strtok(NULL,"]");
				printf("CURR: %s\n",curr);
				curr = curr + 3;
				printf("CURR: %s\n",curr);
				fieldName = strtok(curr,",");
				printf("FIELD: %s\n",fieldName);

				for (i = 0; i < 0; i++) {
				//for (i = 0; i < id; i++) {
					iter = conditions->head;
					/* Check the document values against each parameter */
					x = 1;

					while (iter != NULL && x == 1) {
						if (lookup(db[i],iter->field) == NULL) x = 1;
						else {
							x = compareIntegers(lookup(db[i],iter->field)->value,iter->value,iter->operation);
							}
						iter = iter->next;
						}

					if (iter != NULL) continue;
					printf("Document #%d SUCCESS!\n",i);

					}
				
			

				//freePList(conditions);
				}



			else if (strcmp(operation, "count") == 0) {
				count = 0;
				curr = operation;
				curr = myStrdup(strtok(NULL,"[]"));

				version = strtok(NULL,"[]), ");
				if (version == NULL) printf("Use All Versions\n");
				else printf("Use the last %d versions\n",atoi(version));
				
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
				
		//free(curr);
		free(origQuery);

		curr = readLine(query);
		}

	for (i = 0; i < id; i++) {
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

