#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strtok
#include "scanner.h"
#include "tree.h"
#include "dictionary.h"
#include "list.h"

int processQuery(Document *,PList *,PList *);

int main (void) {
	FILE *data = fopen("data.txt","r");
	if (data == NULL) { printf("Data File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	FILE *query = fopen("queries.txt","r");
	if (query == NULL) { printf("Query File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	char *curr;
	char *key; int val;
	int i,j,id = 0,dbSize = 4; // will also serve as count of documents
	int count = 0; // for counting function

	Field **newAttrList;
	Document *newDoc;
	Document **db = malloc(sizeof(Document *) * HASH);
	if (db == NULL) {
		printf("Allocation of database failed.\nExiting . . .\n");
		exit(1);
		}
	
	for (i = 0; i < dbSize; i++) db[i] = NULL;


	/* Process the DATA.TXT file, make database */
	curr = readLine(data);

	// Read in a new document
	while (!feof(data)) {
		newAttrList = malloc(sizeof(Field *) * HASH);

		install(newAttrList,"sysid",id);
		key = strtok(curr,": ");

		// Parse fields
		while (key != NULL) {
			val = atoi(strtok(NULL,": "));
			install(newAttrList,key,val);

			key = strtok(NULL,": ");  // Get ready for next loop
			}
			
		newDoc = newDocument(lookup(newAttrList,"DocID")->value);
		
		newDoc->attributes = newAttrList;
		insertDocument(db,newDoc);
		id++;

		
		curr = readLine(data);
		}


		Document *iter1,*iter2;
		// DOCUMENT SCANNER LOOP
		/*
		for (i = 0; i < HASH; i++) { 
			if (db[i] == NULL) continue;
			iter1 = db[i];
			while (iter1 != NULL) {
				printf("ID: %d (vn: %d)\n",iter1->docID,iter1->version);
				iter2 = iter1->older;

				int z = 18;
				//while (iter2 != NULL) {
				for (j = 0; j < z; j++) {
					if (iter2 == NULL) break;
					printf("\tID: %d (vn: %d)\n",iter2->docID,iter2->version);
					iter2 = iter2->older;
					}
				iter1 = iter1->next;

				}
			}
		*/
			
//!!!!!!!!!!! 

	/* Process the QUERIES.TXT file */
	curr = readLine(query);
	char *collection,*operation,*origQuery,*version,*fieldName,*threshold,*queryOp;
	PList *conditions,*projection;

	while(!feof(query)) {
		//while (strlen(curr) == 0 && !(feof(query))) curr = readLine(query);
		origQuery = myStrdup(curr);
		printf("%s\n",curr);
		
		/* PARSE COLLECTION NAME */
		collection = strtok(curr,".");
		if (strcmp("final",collection) == 0) {
			operation = strtok(NULL,"(");

			/* PARSE OPERATION NAME */ 
			if (strcmp(operation,"query") == 0) {
				curr = operation;
				curr = curr + 7; // Get past the "query(["
			

				version = NULL;
				conditions = newPList(); projection = newPList();
				while (curr[0] != ',' && curr[0] != ']' && curr[0] != '\0') {
						if (curr[0] == ']') { break; }
						else if (curr[0] == ' ') curr++; // Get rid of the space that comes from th
						// Get the field, queryOp, and value for each condition
						queryOp = strpbrk(curr,"<=>"); // Operation is being reused here
						fieldName = myStrdup(curr);
						fieldName = strtok(fieldName,"<=>");
						threshold = myStrdup(fieldName);
						threshold = strtok(NULL,",] ");


						if (threshold == NULL) { printf("SyntaxError: %s is not a valid condition.\n",curr); break; }

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


				
				projection = newPList();
				
				//curr = strtok(NULL,"]");

				if (curr[0] != '\0') {
						curr += 3;
						if (curr[0] == '[') curr++;
						while (curr && curr[0] != ')' && curr[0] != ',' && curr[0] !=']') {
							fieldName = strtok(myStrdup(curr),",]");
							if (fieldName[0] == ' ') fieldName++;
							insertParam(projection,fieldName,NULL,0);
							curr = strtok(NULL,"");
							}

						if (curr[0] == ']') curr++;
						if (curr[0] == ',') { 
							curr += 3;
							if (curr[0] == ']') version = NULL;
							else { curr = strtok(curr,"]"); version = curr; }
					
							}
						else { version = "1"; }
					}
				else version = "1";
				

				// All Versions
				if (version == NULL) {
					for (i = 0; i < HASH; i++) {
						iter1 = db[i];
						while (iter1 != NULL) {
							iter2 = iter1;
							while (iter2 != NULL) {
								processQuery(iter2,conditions,projection);
								iter2 = iter2->older;
								}
							iter1 = iter1->next;
							}
						}
					}

				// Last <Version> Versions
				else {
					for (i = 0; i < HASH; i++) {
						iter1 = db[i];
						while (iter1 != NULL) {
							iter2 = iter1;
							j = 0;
							while (iter2 != NULL && j < atoi(version)) {
								j += processQuery(iter2,conditions,projection);
								iter2 = iter2->older;
								}
							iter1 = iter1->next;
							}
						}
					}
				
			

				//freePList(conditions);
				}



			else if (strcmp(operation, "count") == 0) {
				version = NULL;
				count = 0;
				curr = operation;
				curr = myStrdup(strtok(NULL,"[]"));

				version = strtok(NULL,"]), ");
				if (version) version++;

				if (version == NULL) version = "0";
				else if (strlen(version) == 0) version = "ALL";

				
				for (i = 0; i < HASH; i++) { 
					if (db[i] == NULL) continue;
					iter1 = db[i];
					while (iter1 != NULL) {
						if (lookup(iter1->attributes,curr) != NULL) { ++count; }

						/* Look through past version */
						iter2 = iter1->older;
						if (strcmp(version,"ALL") == 0) {
							while (iter2 != NULL) {
								if (lookup(iter2->attributes,curr) != NULL) { ++count; }
								iter2 = iter2->older;
								}
							}
						else {
							for (j = 1; j < atoi(version); j++) {
								if (iter2 == NULL) break;
								if (lookup(iter2->attributes,curr) != NULL) { ++count; }
								iter2 = iter2->older;
								}
							}

						iter1 = iter1->next;
						}
					}

				if (count > 0) printf("count_%s: %d\n",curr,count);
				}



			else if (strcmp(operation, "sort") == 0) {
				printf("SORT not implemented.\n");
				}



			else if (strcmp(operation, "insert") == 0) {
			
				newAttrList = malloc(sizeof(Field *) * HASH);

				curr = operation;
				curr = strtok(NULL,": ");
				while (curr != NULL) {
					fieldName = myStrdup(curr);
					curr = strtok(NULL,") ");
					val = atoi(curr);
					install(newAttrList,fieldName,val);
					curr = strtok(NULL,": ");
					}

				if (lookup(newAttrList,"DocID") == NULL) {
					printf("ERROR: No DocID specified for new document.\nExiting . . .\n");
					exit(1);
					}
				
				install(newAttrList,"sysid",id); id++;
				newDoc = newDocument(lookup(newAttrList,"DocID")->value);
				newDoc->attributes = newAttrList;
				
				insertDocument(db,newDoc);
				}

			else { 
				printf("TypeError: %s is not a function\n",origQuery);
				}

			printf("\n\n");
			}
				
		free(origQuery);

		curr = readLine(query);
		}


	// Free stuff maybe?
    /*
	for (i = 0; i < id; i++) {
		for (j = 0; j < HASH; j++) {
			//if (db[i][j] != NULL) { free(db[i][j]->key); free(db[i][j]); }
			}
			free(db[i]);
		}

	free(db);
    */

	fclose(query);
	fclose(data);
	
	return 0;
	}


int processQuery(Document *currDoc,PList *conditions,PList *projection) {
	Pnode *iter; Field **attr = NULL; Field *curr;
	int i,x = 1,eligible = 0,vn = 0;
	iter = conditions->head;
	// Check the document values against each parameter 

	//if (conditions->head == NULL) printf("No Conditions\n");
	//if (projection->head == NULL) printf("All Attributes\n");

	if (conditions->head == NULL) {
		eligible = 1;
		}

	else {
		while (iter != NULL && x == 1) {

	    	if (lookup(currDoc->attributes,iter->field) == NULL) { iter = iter->next; continue; }

			else {
			    x = compareIntegers(lookup(currDoc->attributes,iter->field)->value,iter->value,iter->operation);
				eligible = x;
				}
	
				if (x == 1) eligible = 1; // field must exist, x must satisfy /a/ condition
				iter = iter->next;
				}

		if (iter != NULL) { currDoc = currDoc->next; return 0; }
		}



	if (eligible == 1) {
		iter = projection->head;

		if (iter == NULL) { 
			printf("vn: %d ",currDoc->version);
			attr = currDoc->attributes;
			for (i = 0; i < HASH; i++) {
				curr = attr[i];
				while (curr != NULL) {
					printf("%s:%d ",curr->key,curr->value);
					curr = curr->next;
					}
				}
			printf("\n");

			return 1;
			}


		while (iter != NULL) {
			//if (currDoc->attributes == NULL) printf("NO ATTRIBUTES\n");
			//if (iter->field == NULL) printf("NO FIELD\n");

			if (lookup(currDoc->attributes,iter->field) != NULL) {
				if (vn == 0) { printf("vn:%d ",currDoc->version); vn = 1; }

				printf("%s:%d ",iter->field,lookup(currDoc->attributes,iter->field)->value);
				}
			iter = iter->next;
			}
		if (vn) printf("\n");
		}

	return vn;
    }

