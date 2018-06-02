// Emma Daues
// CS 4280
// P4

// "node.h"

#ifndef NODE_H
#define NODE_H

typedef struct node {
	
	char* label;
	// stores label of the node,
	// based on what function the
	// node came from
	
	struct token * t1;
	struct token * t2;
	struct token * t3;
	// tokens for pointing to terminal data
	// (operators, IDs, numbers)
	
	struct node * child1;
	struct node * child2;
	struct node * child3;
	struct node * child4;
	// node pointers for pointing to 
	// noterminal children
	
} node;

struct node * getNode(char*);
// allocates memory for new node

#endif