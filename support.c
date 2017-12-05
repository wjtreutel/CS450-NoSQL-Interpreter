#include <stdlib.h>
#include <stdio.h>
#include "support.h"

int compareIntegers (int x, int y, char *op) {
    switch (op[0]) {
        case '<':
            if (op[1] == '\0')     return (x < y);
            else if (op[1] == '=') return (x <= y);
            else if (op[1] == '>') return (x != y);
            else { printf("Error: %s is not a recognized operation.\n",op); exit(1); }
            break;
        case '>':
            if (op[1] == '\0') return (x > y);
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

