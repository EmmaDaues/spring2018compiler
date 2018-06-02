// Emma Daues
// CS 4280
// P4

// "node.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "filept.h"
#include "node.h"

struct node * getNode(char* name) {
	
	struct node * newNode;

	// allocate memory for node
	newNode = (node *)malloc(sizeof(node));
	newNode->label = (char *)malloc(7);

	// label node
	strcpy(newNode->label, name);

	// set token pointers to NULL
	newNode->t1 = NULL;
	newNode->t2 = NULL;
	newNode->t3 = NULL;
	
	// set node children to NULL
	newNode->child1 = NULL;
	newNode->child2 = NULL;
	newNode->child3 = NULL;
	newNode->child4 = NULL;
	
	return newNode;
}