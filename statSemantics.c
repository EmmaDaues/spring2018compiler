// Emma Daues
// CS 4280
// P4

// "statSemantics.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "node.h"
#include "token.h"
#include "statSemantics.h"
#include "targetFilept.h"

typedef struct stackElement {
	
	char* ident;
	// stores instance of identifier
	
	int linenumber;
	// stores line number where IDENT_tk 
	// is found
	
} stackElement;

// intialize stack
static int top = -1;
static struct stackElement * stack[100];

// initialize array for temporary variables
static int tempVariableIndex = -1;
static char * tempVar[15] = {"T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10", "T11", "T12", "T13", "T14", "T15"};

static int tempVarCount[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int inoutIndex = -1;
static char * inMarkers[5] = {"In1", "In2", "In3", "In4", "In5"}; 
static char * outMarkers[5] = {"Out1", "Out2", "Out3", "Out4", "Out5"};

int specialVariableFlag = 0; // T100

// global variable that keeps track total
// number of IDENTS within blocks

static int localCount = 0;

/* function definitions */
static void push(char*, int);
static void pop();
static int stackisEmpty();
static int find(char *);
static void statsemError(int, int, char*);

static void writeToBottomFile(char*, char*);

// ************************************************************
// when calling with root in main(), make sure (*varCount) = -1
// (signifies global scope)
// ************************************************************
void statSemantics(struct node** currentNode, int *varCount) {
	
	if ((*currentNode) == 0) { 
		return;
	}
	
	/* get node label */
	char * nodeLabel = (*currentNode)->label;

	struct token* tk1 = (token*)malloc(sizeof(token));
	struct token* tk2 = (token*)malloc(sizeof(token));
	struct token* tk3 = (token*)malloc(sizeof(token));
	tk1 = (*currentNode)->t1;
	tk2 = (*currentNode)->t2;
	tk3 = (*currentNode)->t3;
		
	// *****
	// VARS - 3 tokens (IDENT, EQ, NUM) or NO TOKENS
	// *****
	if ((strcmp(nodeLabel, "VARS") == 0)) {
		
		if (tk1) { // token exists
			int n = find(tk1->instance);
		
			if (*varCount >= 0) { // nodes exist within a <block> (local)
				
				if (*varCount > 0) { // if there are already variables in the stack for <block>
			
					if ((n < 0) ||           // identifier doesnt exist in stack OR
						(n >= *varCount)) {  // identifier exists, but is out of scope
					
						push(tk1->instance, tk1->linenumber); // add to STATSEM stack
					
						// *******************
						// STORAGE ALLOCATION
						// *******************
						fprintf(targetFilept, "PUSH\n");
						fprintf(targetFilept, "LOAD %s\n", tk3->instance);
						fprintf(targetFilept, "STACKW %d\n", n);

						localCount++;
						(*varCount)++;
				
					}
					else { // variable already exists in scope
					
						statsemError(-1, tk1->linenumber, tk1->instance);
					}			
				}
				else { // no variables exist in this scope yet
			
					push(tk1->instance, tk1->linenumber); // add to STATSEM stack
					
					// *******************
					// STORAGE ALLOCATION
					// *******************
					fprintf(targetFilept, "PUSH\n");
					fprintf(targetFilept, "LOAD %s\n", tk3->instance);
					fprintf(targetFilept, "STACKW 0\n");
				
					localCount++;
					(*varCount)++;
			
				}
			}
		
			else { // nodes exist outside a <block> (global)
			
				if (n > 0) { // identifier already exists in scope
		
					statsemError(-1, tk1->linenumber, tk1->instance);
				}
				else {
					push(tk1->instance, tk1->linenumber); // add to STATSEM stack
			
					// *******************
					// STORAGE ALLOCATION
					// *******************
					fprintf(targetFilept, "PUSH\n");
					writeToBottomFile(tk1->instance, tk3->instance);
					
				}
			}
		}
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
		
	// *****
	// MVARS - 1 token (IDENT) or NO TOKENS
	// *****
	else if ((strcmp(nodeLabel, "MVARS") == 0)) {
		
		if (tk1) { // token exists
			int n = find(tk1->instance);
		
			if (*varCount >= 0) { // nodes exist within a <block> (local)
	
				if (*varCount > 0) { // check new identifier with elements already existing in scope
			
			
					if ((n < 0) ||           // identifier doesnt exist in stack OR
						(n >= *varCount)) {  // identifier exists, but is out of scope
				
						push(tk1->instance, tk1->linenumber); // add to STATSEM stack
					
						// *******************
						// STORAGE ALLOCATION
						// *******************
						fprintf(targetFilept, "PUSH\n");
						
						localCount++;
						(*varCount)++;
				
					}
					else { // variable already exists in scope
				
						statsemError(-1, tk1->linenumber, tk1->instance);
					}			
				}
				else { // no identifiers exist in this scope yet
			
					push(tk1->instance, tk1->linenumber); // add to STATSEM stack
					// *******************
					// STORAGE ALLOCATION
					// *******************
					fprintf(targetFilept, "PUSH\n");
					
					localCount++;
					(*varCount)++;
			
				}
			}
			else { // nodes exist outside a <block> (global)
			
				if (n > 0) { // identifier already exists in scope
			
					statsemError(-1, tk1->linenumber, tk1->instance);
				}
				else {
					push(tk1->instance, tk1->linenumber); // add to STATSEM stack
				
					// *******************
					// STORAGE ALLOCATION
					// *******************
					fprintf(targetFilept, "PUSH\n");
				}
			}
		}
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
		
	}
	
	// *****
	// IN - 1 token (IDENT)
	// *****
	else if ( (strcmp(nodeLabel, "IN") == 0)) {
		
		int v = find(tk1->instance);
		
		if (v < 0) { // variable not found among declared
			statsemError(-4, tk1->linenumber, tk1->instance);
		}
		else {
			
			if (v >= localCount) { // variable being read is a global variable
				
				fprintf(targetFilept, "READ %s\n", tk1->instance);
			}	 
			else {  // variable being read is a local variable
					
				// create new temp variable
				tempVariableIndex++;
				writeToBottomFile(tempVar[tempVariableIndex], "0");
				
				fprintf(targetFilept, "READ %s\nLOAD %s\n", tempVar[tempVariableIndex], tempVar[tempVariableIndex]);
					
				fprintf(targetFilept, "STACKW %d\n", v);
				
			}
		}
			
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
	
	// *****
	// OUT - NO TOKENS
	// *****
	else if ((strcmp(nodeLabel, "OUT") == 0)) {
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
		
		fprintf(targetFilept, "WRITE %s\n", tempVar[tempVariableIndex]);
		
	}
	
	// *****
	// M - 1 token (PCENT) or NO TOKEN
	// *****
	else if ((strcmp(nodeLabel, "M") == 0)) {
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);

		if (tk1) { // token exisits, number is NEGATIVE
		
			// special variable stores current value of ACC
			specialVariableFlag = 1;
			fprintf(targetFilept, "STORE T100\n");
					
			fprintf(targetFilept, "LOAD %s\n", tempVar[tempVariableIndex]);
			fprintf(targetFilept, "MULT -1\n");
			fprintf(targetFilept, "STORE %s\n", tempVar[tempVariableIndex]);
			
			// restore previous value of ACC
			fprintf(targetFilept, "LOAD T100\n");
		}
	}
		
	// *****
	// R - 1 token (IDENT or NUM) or NO TOKEN
	// *****
	else if ( (strcmp(nodeLabel, "R") == 0)) { // detect R node
		
		if (tk1) {  // token exists
			if (tk1->tokenID == IDENT_tk) { // node's token is an IDENT
			
				int v = find(tk1->instance);
			
				if (v < 0) { // variable not found among declared
						statsemError(-4, tk1->linenumber, tk1->instance);
				}
				else { 
			
					if (v >= localCount) { // is a global variable
					
						// continue traversing
						statSemantics(&(*currentNode)->child1, varCount);
						statSemantics(&(*currentNode)->child2, varCount);
						statSemantics(&(*currentNode)->child3, varCount);
						statSemantics(&(*currentNode)->child4, varCount);
				
						// create new temp variable
						tempVariableIndex++;
						
						writeToBottomFile(tempVar[tempVariableIndex], "0");
						fprintf(targetFilept, "COPY %s %s\n", tempVar[tempVariableIndex], tk1->instance);

					}	
					else {  // is a local variable
					
						// continue traversing
						statSemantics(&(*currentNode)->child1, varCount);
						statSemantics(&(*currentNode)->child2, varCount);
						statSemantics(&(*currentNode)->child3, varCount);
						statSemantics(&(*currentNode)->child4, varCount);
					
				
						// create new temp variable
						tempVariableIndex++;
						writeToBottomFile(tempVar[tempVariableIndex], "0");
					
						fprintf(targetFilept, "STACKR %d\nSTORE %s\n", v, tempVar[tempVariableIndex]);

					}		
				}
				
			}
			else if (tk1->tokenID == NUM_tk) { // node's token is a NUMBER
				
				// create new temp variable
				tempVariableIndex++;
				writeToBottomFile(tempVar[tempVariableIndex], tk1->instance);
					
				// continue traversing
				statSemantics(&(*currentNode)->child1, varCount);
				statSemantics(&(*currentNode)->child2, varCount);
				statSemantics(&(*currentNode)->child3, varCount);
				statSemantics(&(*currentNode)->child4, varCount);
			
			}
		}			
		else { // no token
		
			// continue traversing
			statSemantics(&(*currentNode)->child1, varCount);
			statSemantics(&(*currentNode)->child2, varCount);
			statSemantics(&(*currentNode)->child3, varCount);
			statSemantics(&(*currentNode)->child4, varCount);
		
		}
	}
	
	// *****
	// X - 1 token (PLUS or MINUS or MULT or DIV) or NO TOKEN
	// *****
	else if ((strcmp(nodeLabel, "X") == 0)) {
		
		if (tk1) {  // node has token
			
			statSemantics(&(*currentNode)->child1, varCount); // work with next temp variable
			statSemantics(&(*currentNode)->child2, varCount);
			statSemantics(&(*currentNode)->child3, varCount);
			statSemantics(&(*currentNode)->child4, varCount);
			
			tempVariableIndex--;
			
			if(tk1->tokenID == PLUS_tk) {						// choose proper operator
				fprintf(targetFilept, "LOAD %s\nADD %s\nSTORE %s\n", tempVar[tempVariableIndex], tempVar[tempVariableIndex+1], tempVar[tempVariableIndex]);
			}
			else if(tk1->tokenID == MINUS_tk) {
				fprintf(targetFilept, "LOAD %s\nSUB %s\nSTORE %s\n", tempVar[tempVariableIndex], tempVar[tempVariableIndex+1], tempVar[tempVariableIndex]);
			}
			 else if(tk1->tokenID == MULT_tk) {
				fprintf(targetFilept, "LOAD %s\nMULT %s\nSTORE %s\n", tempVar[tempVariableIndex], tempVar[tempVariableIndex+1], tempVar[tempVariableIndex]);
			}
			else {
				fprintf(targetFilept, "LOAD %s\nDIV %s\nSTORE %s\n", tempVar[tempVariableIndex], tempVar[tempVariableIndex+1], tempVar[tempVariableIndex]);
			}
		}
		else {
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
		}
	}
		
	// *****
	// ASSIGN - 2 token (IDENT, EQ,)
	// *****
	else if ( (strcmp(nodeLabel, "ASSIGN") == 0)) {
		
		int v = find(tk1->instance);
	
		if (v < 0) { // variable not found among declared
			statsemError(-4, tk1->linenumber, tk1->instance);
		}
		else { // variable exists
			
			if (v >= localCount) { // variable being assigned is a global variable
						
				statSemantics(&(*currentNode)->child1, varCount);
				statSemantics(&(*currentNode)->child2, varCount);
				statSemantics(&(*currentNode)->child3, varCount);
				statSemantics(&(*currentNode)->child4, varCount);
				
				fprintf(targetFilept, "LOAD %s\n", tempVar[tempVariableIndex]);
				fprintf(targetFilept, "STORE %s\n", tk1->instance);
				
			}
			else {  // variable being assigned is a global variable
									
				statSemantics(&(*currentNode)->child1, varCount);
				statSemantics(&(*currentNode)->child2, varCount);
				statSemantics(&(*currentNode)->child3, varCount);
				statSemantics(&(*currentNode)->child4, varCount);
				
				fprintf(targetFilept, "LOAD %s\n", tempVar[tempVariableIndex]);
				fprintf(targetFilept, "STACKW %s\n", v);
				
			}
		}
	}	
	
	// *****
	// IFF - NO TOKENS
	// *****
	else if ((strcmp(nodeLabel, "IFF") == 0)) {
		
		// make new OUT marker
		inoutIndex++;
		
		// evaluate LHS of conditional
		statSemantics(&(*currentNode)->child1, varCount);
		
		fprintf(targetFilept, "LOAD %s\n", tempVar[tempVariableIndex]);
		
		// special variable stores current value of ACC
		specialVariableFlag = 1;
		fprintf(targetFilept, "STORE T100\n");
		
		// evaluate RHS of conditional
		statSemantics(&(*currentNode)->child3, varCount);
		
		// restore previous value of ACC
		fprintf(targetFilept, "LOAD T100\n");
		
		// subtract RHS from LHS
		fprintf(targetFilept, "SUB %s\n", tempVar[tempVariableIndex]);
	
		// evaluate with RO
		statSemantics(&(*currentNode)->child2, varCount);

		// continue traversing
		statSemantics(&(*currentNode)->child4, varCount);
		
		fprintf(targetFilept, "%s: NOOP\n", outMarkers[inoutIndex]);
		inoutIndex--;
	}
	
	// *****
	// LOOP - NO TOKENS
	// *****
	else if ((strcmp(nodeLabel, "LOOP") == 0)) {
		
		// make new OUT marker
		inoutIndex++;
		
		fprintf(targetFilept, "%s: ", inMarkers[inoutIndex]);
		
		// evaluate LHS of conditional
		statSemantics(&(*currentNode)->child1, varCount);
			
		fprintf(targetFilept, "LOAD %s\n", tempVar[tempVariableIndex]);
		
		// evaluate RHS of conditional
		statSemantics(&(*currentNode)->child3, varCount);
		
		// subtract RHS from LHS
		fprintf(targetFilept, "SUB %s\n", tempVar[tempVariableIndex]);
		
		// evaluate with RO
		statSemantics(&(*currentNode)->child2, varCount);

		// continue traversing
		statSemantics(&(*currentNode)->child4, varCount);
		
		fprintf(targetFilept, "BR %s\n", inMarkers[inoutIndex]);
		fprintf(targetFilept, "%s: NOOP\n", outMarkers[inoutIndex]);
		inoutIndex--;
	}
	
	// *****
	// RO - 1 token (LT or GT or EQ)
	// *****
	else if ((strcmp(nodeLabel, "RO") == 0)) {
		
		if (tk1->tokenID == LT_tk) {
			
			if((*currentNode)->child1 == NULL) { // RO reads "<" (<)
				
				// skip if first expression is NOT < to second expression
				// aka, a - b >= 0
			
				fprintf(targetFilept, "BRZPOS %s\n", outMarkers[inoutIndex]);
				statSemantics(&(*currentNode)->child1, varCount);
				
			}
			else { // RO reads "< <" (<=)
		
				// skip if first expression is NOT <= to second expression
				// aka, a - b > 0
				fprintf(targetFilept, "BRPOS %s\n", outMarkers[inoutIndex]);
				statSemantics(&(*currentNode)->child1, varCount);
		
				
			}
			
		}
		else if (tk1->tokenID == GT_tk) {
			
			if((*currentNode)->child1 == NULL) { // RO reads ">" (>)
				
				// skip if first expression is NOT > to second expression
				// aka, a - b <= 0
			
				fprintf(targetFilept, "BRZNEG %s\n", outMarkers[inoutIndex]);
				statSemantics(&(*currentNode)->child1, varCount);
				
			}
			else { // RO reads "> >" (>=)
				
				// skip if first expression is NOT >= to second expression
				// aka, a - b < 0
				
				fprintf(targetFilept, "BRNEG %s\n", outMarkers[inoutIndex]);
				statSemantics(&(*currentNode)->child1, varCount);
			}
		}
		else {
			
			if ((*currentNode)->child1 == NULL) { // RO reads "=" (!=)
		
				// skip if first expression IS = to second expression
				// aka, a - b == 0
			
				fprintf(targetFilept, "BRZERO %s\n", outMarkers[inoutIndex]);
				statSemantics(&(*currentNode)->child1, varCount);
			
			}
			else { // RO reads "= =" (==)
		
		
				// skip if first expression is NOT == to second expression
				// aka, a - b > 0 OR a - b < 0
				
				fprintf(targetFilept, "BRPOS %s\n", outMarkers[inoutIndex]); // ?????????
				fprintf(targetFilept, "BRNEG %s\n", outMarkers[inoutIndex]); // ?????????
				statSemantics(&(*currentNode)->child1, varCount);
				
			}
		}
		
		// continue traversing
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
		
	// *****
	// L - 1 token (LT) or NO TOKEN
	// *****
	else if ((strcmp(nodeLabel, "L") == 0)) {
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
		
	// *****
	// G - 1 token (GT)
	// *****
	else if ((strcmp(nodeLabel, "G") == 0)) {
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
		
	// *****
	// E - 1 token (EQ)
	// *****
	else if ((strcmp(nodeLabel, "E") == 0)) {

	// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
	}
	
	// *****
	// BLOCK - NO TOKENS
	// *****
	else if (strcmp(nodeLabel, "BLOCK") == 0) {  // detect BLOCK node

		int *newvarCount = (int *) malloc(sizeof(int)); // start new scope
		
		// continue traversing
		statSemantics(&(*currentNode)->child1, newvarCount);
		statSemantics(&(*currentNode)->child2, newvarCount);
		statSemantics(&(*currentNode)->child3, newvarCount);
		statSemantics(&(*currentNode)->child4, newvarCount);
	
		// remove local variables when block finishes
		int i;
		for (i = 0; i < (*newvarCount); i++) {
			pop();
				
			// ************************************
			// STORAGE ALLOCATION: POP FROM TARGET
			// ************************************
			fprintf(targetFilept, "POP\n");
			localCount--;
		}
		free(newvarCount);
	}
	
	else { // no tokens present in node
	
		// continue traversing
		statSemantics(&(*currentNode)->child1, varCount);
		statSemantics(&(*currentNode)->child2, varCount);
		statSemantics(&(*currentNode)->child3, varCount);
		statSemantics(&(*currentNode)->child4, varCount);
		
	}
}

static void push(char* name, int linenum) {
	
	struct stackElement * newVar = (stackElement*)malloc(sizeof(stackElement));
	newVar->ident = (char *)malloc(9);
	
	newVar->linenumber = linenum;
	strcpy(newVar->ident, name);

	top++; // increment top
	if (top >= 100) { // reached limit of stack
		statsemError(-2, linenum, " ");
	}
	else {
		stack[top] = newVar;
	}
	return;
}

static void pop() {
	
	if (stackisEmpty()) {
		statsemError(-3, 0, " ");
	}
	else {
		
		free(stack[top]);
		top--;
	}
	return;
}

static int find(char * name) {
	
	if (stackisEmpty()) {
		return -1;
	}
	else {
		int i;
		for (i = 0; i <= top; i++){ // search through stack
			
			if ( strcmp( (stack[top-i]->ident), name) == 0 ) { // match found
				return i;
			}
		}
		
		// if gone through stack without finding anything
		return -1;
	}
}

static int stackisEmpty() {
	return (top == -1);
}

static void statsemError(int errnum, int linenum, char* id) {

	if (errnum == -1) { // variable already exists in scope
		printf("StatSem Error (line %d): Variable %s already declared in scope\n", linenum, id);
		
		exit(0);
		return;
	}
	else if (errnum == -2) { // stack overflow
		printf("StatSem Error (line %d): Stack overflow\n", linenum);
		
		exit(0);
		return;
	}
	else if (errnum == -3) { // popped from empty stack
		printf("StatSem WARNING: Tried to pop from empty stack\n");
		
		exit(0);
		return;
	}
	if (errnum == -4) { // using variable without it being declared
		printf("StatSem Error (line %d): Variable %s has not been properly declared\n", linenum, id);
		
		exit(0);
		return;
	}
		
}

static void writeToBottomFile(char * name, char * value) {
	
	// search to see if variable 
	// has already been added
	
	int isInFile = 0;

	if (name[0] == 'T') { //check for repeats of temp variable declarations
	
		int i;
		for(i = 0; i < 15; i++) {
			
			if (strcmp(name, tempVar[i]) == 0) {
				
				if (tempVarCount[i] == 1) { // temp variable already exists
					isInFile = 1;
				}
				else { // temp variable is not in file yet
					tempVarCount[i] = 1;
				}
			}
		}
		
	}		

	if (isInFile == 0) { // variable is not yet added
		
		fprintf(bottomFilept, "%s %s\n", name, value);
	}
	else { } // do nothing
	
	return;
}