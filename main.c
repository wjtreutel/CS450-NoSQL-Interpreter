#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strtok
#include "scanner.h"
#include "dictionary.h"
#include "list.h"
#define INT_MAX 2147483647

int processQuery(Document *,PList *,PList *);

FILE *outfile;

int main (void) {
	FILE *data = fopen("data.txt","r");
	if (data == NULL) { fprintf(stderr,"Data File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	FILE *query = fopen("queries.txt","r");
	if (query == NULL) { fprintf(stderr,"Query File cannot be opened for reading. Exiting . . .\n"); exit(1); }

	outfile = fopen("wjtreutel.txt","w");
	if (outfile == NULL) { fprintf(stderr,"File 'wjtreutel.txt' could not be opened for writing.\n"); exit(1); }


	char *curr;
	char *key; int val;
	int i,j,k,id = 1,dbSize = 4; // will also serve as count of documents
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
		if (strcmp(curr,"") == 0) { if (!feof(data)) curr = readLine(data); continue; }
		
		newAttrList = malloc(sizeof(Field *) * HASH);

		install(newAttrList,"sysid",id);
		install(newAttrList,"vn",1);
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

		
		free(curr);
		curr = readLine(data);
		}



	/* Process the QUERIES.TXT file */
	Document *iter1,*iter2;
	curr = readLine(query);
	char *collection,*operation,*origQuery,*version,*fieldName = NULL, *threshold,*queryOp;
	PList *conditions,*projection;
	int min = INT_MAX,max = 0,x; // for sort

	while(!feof(query)) {
		if (strcmp(curr,"") == 0) { free(curr);  if (!feof(query)) curr = readLine(query); continue; }
		origQuery = myStrdup(curr);
		fprintf(outfile,"%s\n",curr);
		
		/* Remove trailing whitespace from line */
		for (i = strlen(curr) - 1; i > 0; i--) {
			if (curr[i] == ' ') curr[i] = '\0';
			else break;
			}

		/* PARSE COLLECTION NAME */
		collection = strtok(curr,".");
		if (strcmp("final",collection) == 0) {
			operation = strtok(NULL,"(");

			/* QUERY FUNCTION */ 
			if (strcmp(operation,"query") == 0) {
				curr = operation;
				curr = curr + 7; // Get past the "query(["
			

				version = NULL;
				conditions = newPList(); 
				while (curr[0] != ',' && curr[0] != ']' && curr[0] != '\0') {
						while (curr[0] == ' ') curr++; // Get rid of preceding whitespace
						if (curr[0] == ']') break; 
						// Get the field, queryOp, and value for each condition
						queryOp = strpbrk(curr,"<=>"); // Operation is being reused here
						fieldName = myStrdup(curr);
						fieldName = strtok(fieldName,"<=>");
						threshold = myStrdup(fieldName);
						threshold = strtok(NULL,",] "); 
					
						/* Remove leading and trailing whitespace from fieldName */
						while (fieldName[0] == ' ') fieldName++;
						for (i = strlen(fieldName) - 1; i > 0; i--) {
							if (fieldName[i] == ' ') fieldName[i] = '\0';
							else break;
							}

						if (threshold == NULL) { printf("SyntaxError: %s is not a valid condition.\n",curr); break; }

						// Find out what the queryOp is
						while (queryOp[0] == ' ') queryOp++;
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
								fprintf(stderr,"Error! '%c' is not a valid queryOp!\n",operation[0]);
							}

						insertParam(conditions,fieldName,queryOp,atoi(threshold));
						curr = strtok(NULL,"");
						}


				
				projection = newPList();
			
				// Parse out the version
				if (curr[0] != '\0') {
						curr += 3;
						if (curr[0] == '[') curr++;
						while (curr && curr[0] != ')' && curr[0] != ',' && curr[0] !=']') {
							fieldName = strtok(myStrdup(curr),",]");
							while (fieldName[0] == ' ') fieldName++;
							if (fieldName[0] == '\0') break;
							if (strcmp(fieldName,"sysid") == 0) addToFront(projection,fieldName);
							else insertParam(projection,fieldName,NULL,0);
							curr = strtok(NULL,"");
							}

						while (curr[0] == ' ' || curr[0] == ']') curr++;
						if (curr[0] == ',') { 
							curr += 3;
							if (curr[0] == ']') version = NULL;
							else { curr = strtok(curr,"]"); version = curr; }
					
							}
						else { version = "CURR"; }
					}
				else version = "CURR";

			
				// PRINT 'EM OUT, PRINT 'EM OUT	
				for (i = 0; i < HASH; i++) {
					iter1 = db[i];
					while (iter1 != NULL) {
						iter2 = iter1;
						if (version && strcmp(version,"CURR") == 0) {
							while (iter2 != NULL) {
								if (processQuery(iter2,conditions,projection)) break;
								iter2 = iter2->older;
								}
							}
						else if (version != NULL) {
							j = 0;
							//while (iter2 != NULL && j < atoi(version)) {
							for (j = 0; j < atoi(version); j++) {
								if (iter2 == NULL) break;
								processQuery(iter2,conditions,projection);
								iter2 = iter2->older;
								}
							}
						else {
							while (iter2 != NULL) {
								processQuery(iter2,conditions,projection);
								iter2 = iter2->older;
								}
							}
						iter1 = iter1->next;
						}
					}
				}



			/* COUNT FUNCTION */
			else if (strcmp(operation, "count") == 0) {
				version = NULL;
				count = 0;
				curr = operation;
				curr = myStrdup(strtok(NULL,"[]"));

				version = strtok(NULL,"]), ");
				if (version) version++;

				if (version == NULL) version = "CURR";
				else if (strlen(version) == 0) version = NULL;

				for (i = 0; i < HASH; i++) {
					iter1 = db[i];
					while (iter1 != NULL) {
						iter2 = iter1;
						if (version && strcmp(version,"CURR") == 0) {
							while (iter2 != NULL) {
								if (lookup(iter2->attributes,curr) != NULL) { count++; break; }
			
								iter2 = iter2->older;
								}
							}
						else if (version != NULL) {
							j = 0;
							//while (iter2 != NULL && j < atoi(version)) {
							for (j = 0; j < atoi(version); j++) {
								if (iter2 == NULL) break;
								if (lookup(iter2->attributes,curr) != NULL) { count++; }
								iter2 = iter2->older;
								}
							}
						else {
							while (iter2 != NULL) {
								if (lookup(iter2->attributes,curr) != NULL) { count++; }
								iter2 = iter2->older;
								}
							}
						iter1 = iter1->next;
						}
					}

				if (count > 0) fprintf(outfile,"count_%s: %d\n",curr,count);
				}


			/* SORT FUNCTION */
			else if (strcmp(operation, "sort") == 0) {
				count = 0; min = INT_MAX; max = 0;
				version = NULL;
				curr = operation;
				curr = myStrdup(strtok(NULL,"[]"));

				version = strtok(NULL,"]), ");
				if (version) version++;

				if (version == NULL) version = "CURR";
				else if (strlen(version) == 0) version = NULL;

				conditions = newPList(); projection = newPList();

				// FIND MIN AND MAX
				for (i = 0; i < HASH; i++) {
					iter1 = db[i];
					while (iter1 != NULL) {
						iter2 = iter1;
						if (version && strcmp(version,"CURR") == 0) {
							while (iter2 != NULL) {
								if (lookup(iter2->attributes,curr) != NULL) { 
									x = lookup(iter2->attributes,curr)->value;
									if (x < min) min = x;
									if (x > max) max = x;
									break;
									}
			
								iter2 = iter2->older;
								}
							}
						else if (version != NULL) {
							//while (iter2 != NULL && j < atoi(version)) {
							for (j = 0; j < atoi(version); j++) {
								if (iter2 == NULL) break;
								if (lookup(iter2->attributes,curr) != NULL) { 
									x = lookup(iter2->attributes,curr)->value;
									if (x < min) min = x;
									if (x > max) max = x;
									}
								iter2 = iter2->older;
								}
							}
						else {
							while (iter2 != NULL) {
								if (lookup(iter2->attributes,curr) != NULL) {
									x = lookup(iter2->attributes,curr)->value;
									if (x < min) min = x;
									if (x > max) max = x;
									}
								iter2 = iter2->older;
								}
							}
						iter1 = iter1->next;
						}
					}


					for (k = min; k <= max; k++) {
						for (i = 0; i < HASH; i++) {
							iter1 = db[i];
							while (iter1 != NULL) {
								iter2 = iter1;
								if (version && strcmp(version,"CURR") == 0) {
									while (iter2 != NULL) {
										if (lookup(iter2->attributes,curr) != NULL) { 
											x = lookup(iter2->attributes,curr)->value;
											if (x == k) processQuery(iter2,conditions,projection);
											break;
											}
					
										iter2 = iter2->older;
										}
									}
								else if (version != NULL) {
									j = 0;
									//while (iter2 != NULL && j < atoi(version)) {
									for (j = 0; j < atoi(version); j++) {
										if (iter2 == NULL) break;
										if (lookup(iter2->attributes,curr) != NULL) { 
											x = lookup(iter2->attributes,curr)->value;
											if (x == k) processQuery(iter2,conditions,projection);
											}
										iter2 = iter2->older;
										}
									}
								else {
									while (iter2 != NULL) {
										if (lookup(iter2->attributes,curr) != NULL) {
											x = lookup(iter2->attributes,curr)->value;
											if (x == k) processQuery(iter2,conditions,projection);
											}
										iter2 = iter2->older;
										}
									}
								iter1 = iter1->next;
								}
							}

					}
			}


			/* INSERT FUNCTION */
			else if (strcmp(operation, "insert") == 0) {
			
				newAttrList = malloc(sizeof(Field *) * HASH);

printf("NEW INSERT: {");
				curr = operation;
				curr = strtok(NULL,": ");
				while (curr != NULL) {
					fieldName = myStrdup(curr);
					curr = strtok(NULL,") ");
					val = atoi(curr);
					install(newAttrList,fieldName,val);

printf("%s:%d ",fieldName,val);
					curr = strtok(NULL,": ");
					}

printf("}\n");
				if (lookup(newAttrList,"DocID") == NULL) {
					fprintf(stderr,"ERROR: No DocID specified for new document.\nExiting . . .\n");
					exit(1);
					}
				
				install(newAttrList,"sysid",id); id++;
				newDoc = newDocument(lookup(newAttrList,"DocID")->value);
				newDoc->attributes = newAttrList;
				
				insertDocument(db,newDoc);
				}

			else { 
				fprintf(stderr,"TypeError: %s is not a function\n",origQuery);
				}

			fprintf(outfile,"\n");
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
	if (conditions->head == NULL) {
		eligible = 1;
		}

	else {
		while (iter != NULL && x == 1) {

	    	if (lookup(currDoc->attributes,iter->field) == NULL) { 
				iter = iter->next; continue; }

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
			fprintf(outfile,"vn: %d ",currDoc->version);
			fprintf(outfile,"sysid: %03d ",lookup(currDoc->attributes,"sysid")->value);
			attr = currDoc->attributes;
			for (i = 0; i < HASH; i++) {
				curr = attr[i];
				while (curr != NULL) {
					if (strcmp(curr->key,"vn") != 0 && strcmp(curr->key,"sysid") != 0) fprintf(outfile,"%s:%d ",curr->key,curr->value);
					curr = curr->next;
					}
				}
			fprintf(outfile,"\n");

			return 1;
			}


		while (iter != NULL) {
			if (lookup(currDoc->attributes,iter->field) != NULL) {
				if (vn == 0) { fprintf(outfile,"vn: %d ",currDoc->version); vn = 1; }

				if (strcmp(iter->field,"sysid") == 0) 
					fprintf(outfile,"sysid: %03d ",lookup(currDoc->attributes,"sysid")->value);
				else 
					if (strcmp(iter->field,"vn") != 0)
					fprintf(outfile,"%s: %d ",iter->field,lookup(currDoc->attributes,iter->field)->value);
				}
			iter = iter->next;
			}
		if (vn) fprintf(outfile,"\n");
		}

	return vn;
    }
