#include <stdlib.h>
#include <stdio.h>
#include "list.h"

int compareIntegers (int x, int y, char *op) {
	//printf("OP: %d\n",op);
    switch (op[0]) {
        case '<':
            if (op[1] == '\0')     return (x < y);
            else if (op[1] == '=') return (x <= y);
            else if (op[1] == '>') return (x != y);
            else { printf("Error: %s is not a recognized operation.\n",op); exit(1); }
            break;
        case '>':
            if (op[1] == '\0')     return (x > y); 
            else if (op[1] == '=') return (x >= y);
            else { printf("Error: %s is not a recognized operation.\n",op); exit(1); }
            break;
        case '=':
            if (op[1] == '\0') return (x == y);
        default:
            printf("Error: %s is not a recognized operation.\n",op);
            exit(1);
        }
    }


PList *newPList(void) {
	PList *new = malloc(sizeof(PList));
	if (new == 0) { 
		printf("ERROR: Could not allocate new parameter list. Exiting . . .\n");
		exit(1);
		}

	new->size = 0;
	new->head = 0;
	new->tail = 0;

	return new;
	}


void insertParam(PList *list,char *field,char *op,int value) {
	Pnode *new = malloc(sizeof(Pnode));
	if (new  == 0) {
		printf("ERROR: Could not allocate new parameter node. Exiting . . .\n");
		exit(1);
		}

	new->field = field;
	new->operation = op;
	new->value = value;

	if (list->size == 0) list->head = list->tail = new;

	else if (list->head == list->tail) {
		list->tail = new;
		list->head->next = list->tail;
		}

	else { list->tail->next = new; list->tail = new; }

	++list->size;

	return;
	}


void addToFront(PList *list,char *field) {
	Pnode *new = malloc(sizeof(Pnode));

	if (new == 0) {
		printf("ERROR: Could not allocate new parameter node. Exiting . . .\n");
		exit(1);
		}

	new->field = field;
	new->operation = NULL;
	new->value = 0;


	if (!list->head) { list->head = list->tail = new; }

	else if (list->tail == list->head) { new->next = list->tail; list->head = new; }

	else { new->next = list->head; list->head = new;  }

	++list->size;

	return;
	}
