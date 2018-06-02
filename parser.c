// Emma Daues
// CS 4280
// P3

// "parser.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "token.h"
#include "scanner.h"
#include "filept.h"
#include "node.h"

char nextChar;
int lineNum = 1;

/* funcion declarations */
static void parserError(); 
static struct node * program();
static struct node * vars();
static struct node * block();
static struct node * mvars();
static struct node * stats();
static struct node * stat();
static struct node * mStat();
static struct node * in();
static struct node * out();
static struct node * iff();
static struct node * loop();
static struct node * assign();
static struct node * expr();
static struct node * M();
static struct node * X();
static struct node * R();
static struct node * RO();
static struct node * L();
static struct node * G();
static struct node * E();

// declare global token
static struct token *tk;

struct node * parser() {

	//memory for token
	tk = (token *)malloc(sizeof(token));
	tk->instance = (char *)malloc(9);	
		
	// set initial lookahead character
	nextChar = filter(filept);
		
	// first call to scanner
	tk = scanner();

	// declare ROOT node
	struct node * root;

	// and we're off!
	root = program();

	// check for EOF
	if (tk->tokenID == EOF_tk) {
		// continue, parse OK
		printf("Parse tree constructed.\n");
	}
	else { // error
		parserError("ROOT");
	}
	return root;

}

/*
<program> -> PROG_tk <vars> <block>
*/
static struct node * program() {

	struct node * newNode = getNode("PROGRAM");

	if (tk->tokenID == PROG_tk) { // predict <program> -> PROG_tk <vars> <block>
		
		tk = scanner(); // process PROG_tk (not stored by node)
		
		newNode->child1 = vars();  // process <vars> and <block>
		newNode->child2 = block();
		
		return newNode;
	}
	
	else parserError("PROGRAM");
}

/*
<vars> -> VAR_tk IDENT_tk EQ_tk NUM_tk <mvars> | empty
*/
static struct node * vars() {
	
	struct node * newNode = getNode("VARS");
	
	if (tk->tokenID == VAR_tk) { // predict <vars> -> VAR_tk IDENT_tk EQ_tk NUM_tk <mvars>
		
		tk = scanner(); // process VAR_tk (not stored in node)
		
		if (tk->tokenID == IDENT_tk) { 
			
			newNode->t1 = tk; // process IDENT_tk (stored in node)
			tk = scanner();
			
			if (tk->tokenID == EQ_tk) {

				newNode->t2 = tk; // process EQ_tk (stored in node)
				tk = scanner();
				
				if (tk->tokenID == NUM_tk) {
					newNode->t3 = tk; // process NUM_tk (stored in node)
					tk = scanner();
					
					newNode->child1 = mvars(); // process <mvars>
					return newNode;
				}
				else parserError("VARS");
			}
			else parserError("VARS");
		}
		else parserError("VARS");
	}
	else // predict <vars> -> empty

		return NULL;
}

/*
<block> -> START_tk <vars> <stats> STOP_tk 
*/
static struct node * block() {
	
	struct node * newNode = getNode("BLOCK");
	
	if (tk->tokenID == START_tk) { // predict <block> -> START_tk <vars> <stats> STOP_tk
	
		tk = scanner(); // process START_tk (not stored in node)
		
		newNode->child1 = vars(); // process <vars> and <stats>
		newNode->child2 = stats();
		
		if (tk->tokenID == STOP_tk) {
			tk = scanner(); // process STOP_tk (not stored in node)
			
			return newNode;
		}
		else parserError("BLOCK");
	}
	else parserError("BLOCK");
}
/*
<mvars> -> DOT_tk | COL_tk IDENT_tk <mvars>  
*/
static struct node * mvars() {
	
	struct node * newNode = getNode("MVARS");
	
	if (tk->tokenID == DOT_tk) { // predict <mvars> -> DOT_tk
		tk = scanner(); // process DOT_tk (not stored in node)
		
		return newNode;
	}
	else if (tk->tokenID == COL_tk) { // predict <mvars> -> COL_tk IDENT_tk <mvars>  
		tk = scanner(); // process COL_tk (not stored in node)
		
		if (tk->tokenID == IDENT_tk) {
			
			newNode->t1 = tk; // process IDENT_tk (stored in node)
			tk = scanner(); 
			
			newNode->child1 = mvars(); // process <mvars>
			return newNode;
		}
		else parserError("MVARS");
	}
	else parserError("MVARS");
}
/*
<stats> -> <stat> <mStat> 
*/
static struct node * stats() {
	
	struct node * newNode = getNode("STATS");
	
	newNode->child1 = stat(); // process <stat> and <mStat>
	newNode->child2 = mStat();
	
	return newNode;
}
/*
<stat> -> <in> | <out> | <block> | <iff> | <loop> | <assign> 

<in>     -> READ_tk IDENT_tk DOT_tk 
<out>    -> PRINT_tk <expr> DOT_tk 
<block>  -> START_tk <vars> <stats> STOP_tk 
<iff>    -> IFF_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat>
<loop>   -> ITER_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat> 
<assign> -> LET_tk IDENT_tk EQ_tk <expr> DOT_tk
*/
static struct node * stat() {
	
	struct node * newNode = getNode("STAT");
	
	if (tk->tokenID == READ_tk) { // predict <stat> -> <in>
		newNode->child1 = in();
		return newNode;
	}
	else if (tk->tokenID == PRINT_tk) { // predict <stat> -> <out>
		newNode->child1 = out();
		return newNode;
	}
	else if (tk->tokenID == START_tk) { // predict <stat> -> <block>
		newNode->child1 = block();
		return newNode;
	}
	else if (tk->tokenID == IFF_tk) { // predict <stat> -> <iff>
		newNode->child1 = iff();
		return newNode;
	}
	else if (tk->tokenID == ITER_tk) { // predict <stat> -> <loop>
		newNode->child1 = loop();
		return newNode;
	}
	else if (tk->tokenID == LET_tk) { // predict <stat> -> <assign>
		newNode->child1 = assign();
		return newNode;
	}
	else parserError("STAT");
	
	
}

/*
<mStat> -> <stat> <mStat> | empty

FIRST(<stat>) = { READ_tk, PRINT_tk, START_tk, IFF_tk, ITER_tk, LET_tk }
*/
static struct node * mStat() {
	
	struct node * newNode = getNode("MSTAT");
	
	if ((tk->tokenID == READ_tk) ||
		(tk->tokenID == PRINT_tk) ||
		(tk->tokenID == START_tk) ||
		(tk->tokenID == IFF_tk) ||
		(tk->tokenID == ITER_tk) ||
		(tk->tokenID == LET_tk) ) { // predict <mStat> -> <stat> <mStat>
		
		newNode->child1 = stat(); // process <stat> and <mStat>
		newNode->child2 = mStat();
		return newNode;
	}
	else // predict <mStat> -> empty
		return NULL;
}

/*
<in> -> READ_tk IDENT_tk DOT_tk 
*/
static struct node * in() {
	
	struct node * newNode = getNode("IN");
	
	if (tk->tokenID == READ_tk) { // predict <in> -> READ_tk ID_tk DOT_tk 
		tk = scanner(); // process READ_tk (not stored in node)
		
		if (tk->tokenID == IDENT_tk) {
			
			newNode->t1 = tk; // process IDENT_tk (stored in node)
			tk = scanner(); 
		
			if (tk->tokenID == DOT_tk) {
				tk = scanner(); // process DOT_tk (not stored in node)
				return newNode;
			}
			else parserError("IN");
		}
		else parserError("IN");
	}
	else parserError("IN");
}

/*
<out> -> PRINT_tk <expr> DOT_tk 
*/
static struct node * out() {
	
	struct node * newNode = getNode("OUT");
	
	if (tk->tokenID == PRINT_tk) { // predict <out> -> PRINT_tk <expr> DOT_tk
		tk = scanner(); // process PRINT_tk (not stored in node)
		
		newNode->child1 = expr(); // process <expr>
		
		if (tk->tokenID == DOT_tk) {
			tk = scanner(); // process DOT_tk (not stored in node)
			return newNode;
		}
		else parserError("OUT");
	}
	else parserError("OUT");
}

/*
<iff> -> IFF_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat>
*/
static struct node * iff() {
	
	struct node * newNode = getNode("IFF");
	
	if (tk->tokenID == IFF_tk) { // predict <iff> -> IFF_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat>
		tk = scanner(); // process IFF_tk (not stored in node)
		
		if (tk->tokenID == LPAREN_tk) {
			tk = scanner(); // process LPAREN_tk (not stored in node)
			
			newNode->child1 = expr(); // process <expr>
			newNode->child2 = RO(); // process <RO>
			newNode->child3 = expr(); // process <expr>
			
			if (tk->tokenID == RPAREN_tk) {
				tk = scanner(); // process RPAREN_tk (not stored in node)
				
				newNode->child4 = stat(); // process <stat>
				
				return newNode;
			}
			else parserError("IFF");
		}
		else parserError("IFF");
	}
	else parserError("IFF");
}

/*
<loop> -> ITER_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat> 
*/
static struct node * loop() {
	
	struct node * newNode = getNode("LOOP");
	
	if (tk->tokenID == ITER_tk) { // predict <loop> -> ITER_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat> 
		tk = scanner(); // process ITER_tk (not stored in node)
		
		if (tk->tokenID == LPAREN_tk) {
			tk = scanner(); // process LPAREN_tk (not stored in node)
			
			newNode->child1 = expr(); // process <expr>
			newNode->child2 = RO(); // process <RO>
			newNode->child3 = expr(); // process <expr>
			
			if (tk->tokenID == RPAREN_tk) {
				tk = scanner(); // process RPAREN_tk (not stored in node)
				
				newNode->child4 = stat(); // process <stat>
				
				return newNode;
			}
			else parserError("LOOP");
		}
		else parserError("LOOP");
	}
	else parserError("LOOP");
}

/*
<assign> -> LET_tk IDENT_tk EQ_tk <expr> DOT_tk
*/
static struct node * assign() {
	
	struct node * newNode = getNode("ASSIGN");
	
	if (tk->tokenID == LET_tk) { // predict <assign> -> LET_tk IDENT_tk EQ_tk <expr> DOT_tk
		tk = scanner(); // process LET_tk (not stored in node)
		
		if (tk->tokenID == IDENT_tk) {
						
			newNode->t1 = tk; // process IDENT_tk (stored in node)		
			tk = scanner(); 
		
			if (tk->tokenID == EQ_tk) {
				newNode->t2 = tk; // process EQ_tk (stored in node)
				tk = scanner();
				
				newNode->child1 = expr(); // process <expr>
				
				if (tk->tokenID == DOT_tk) {
					tk = scanner(); // process DOT_tk (not stored in node)
					return newNode;
				}
				else parserError("ASSIGN");
			}
			else parserError("ASSIGN");
		}
		else parserError("ASSIGN");
	}
	else parserError("ASSIGN");
}

/*
<expr> -> <M> <X>   
*/
static struct node * expr() {
	
	struct node * newNode = getNode("EXPR");
	
	newNode->child1 = M(); // process <M>
	newNode->child2 = X(); // process <X>
	return newNode;
}

/*
<M> -> PCENT_tk <M> | <R> 

<R> -> LPAREN_tk <expr> RPAREN_tk | IDENT_tk | NUM_tk      
*/
static struct node * M() {
	
	struct node * newNode = getNode("M");
	
	if (tk->tokenID == PCENT_tk) { // predict <M> -> PCENT_tk <M> 
		newNode->t1 = tk; // process PCENT_tk (stored in node)
		tk = scanner(); 
		
		newNode->child1 = M(); // process <M>
		return newNode;
	}
	else if ((tk->tokenID == LPAREN_tk) ||
			(tk->tokenID == IDENT_tk) ||
			(tk->tokenID == NUM_tk))  { // predict <M> -> <R>
			
			newNode->child1 = R(); // process <R>
			return newNode;
	}
	else parserError("M");
}
	
/*
<X> -> PLUS_tk <expr> | MINUS_tk <expr> | DIV_tk <expr> | MULT_tk <expr> | empty 
*/
static struct node * X() {
	
	struct node * newNode = getNode("X");
	
	if (tk->tokenID == PLUS_tk) { // predict <X> -> PLUS_tk <expr>
		newNode->t1 = tk; // process PLUS_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = expr(); // process <expr>
		return newNode;
	}
	else if (tk->tokenID == MINUS_tk) { // predict <X> -> MINUS_tk <expr>
		newNode->t1 = tk; // process MINUS_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = expr(); // process <expr>
		return newNode;
	}
	else if (tk->tokenID == DIV_tk) { // predict <X> -> DIV_tk <expr>
		newNode->t1 = tk; // process DIV_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = expr(); // process <expr>
		return newNode;
	}
	else if (tk->tokenID == MULT_tk) { // predict <X> -> MULT_tk <expr>
		newNode->t1 = tk; // process MULT_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = expr(); // process <expr>
		return newNode;
	}
	
	else // predict <X> -> empty
		return NULL;
}
/*
<R> -> LPAREN_tk <expr> RPAREN_tk | IDENT_tk | NUM_tk 
*/
static struct node * R() {
	
	struct node * newNode = getNode("R");
	
	if (tk->tokenID == LPAREN_tk) { // predict <R> -> LPAREN_tk <expr> RPAREN_tk
		tk = scanner(); // process LPAREN_tk (not stored in node)
			
		newNode->child1 = expr(); // process <expr>
				
		if (tk->tokenID == RPAREN_tk) {
			tk = scanner(); // process RPAREN_tk (not stored in node)
				
			return newNode;
		}
		else parserError("R");
	}
	else if (tk->tokenID == IDENT_tk) { // predict <R> -> IDENT_tk
		newNode->t1 = tk; // process IDENT_tk (stored in node)
		tk = scanner(); 
		return newNode;
	}
	else if (tk->tokenID == NUM_tk) { // predict <R> -> NUM_tk
		newNode->t1 = tk; // process NUM_tk (stored in node)
		tk = scanner();
		return newNode;
	}
	else parserError("R");
}

/*
<RO> -> LT_tk <L> | GT_tk <G> | EQ_tk <E> 
*/
static struct node * RO() {
	
	struct node * newNode = getNode("RO");
	
	if (tk->tokenID == LT_tk) { // predict <RO> -> LT_tk <L>
		newNode->t1 = tk; // process LT_tk (stored in node)
		tk = scanner(); 
		
		newNode->child1 = L(); // process <L>
		return newNode;
	}
	else if (tk->tokenID == GT_tk) { // predict <RO> -> GT_tk <G>
		newNode->t1 = tk; // process GT_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = G(); // process <G>
		return newNode;
	}
	else if (tk->tokenID == EQ_tk) { // predict <RO> -> EQ_tk <E>
		newNode->t1 = tk; // process EQ_tk (stored in node)
		tk = scanner();
		
		newNode->child1 = E(); // process <E>
		return newNode;
	}
	else parserError("RO");
	
}

/*
<L> -> LT_tk | empty  
*/
static struct node * L() {
	
	struct node * newNode = getNode("L");
	
	if (tk->tokenID == LT_tk) { // predict <L> -> LT_tk
		newNode->t1 = tk; // process LT_tk (stored in node)
		tk = scanner(); 

		return newNode;
	}
	
	else // predict <L> -> empty
		return NULL;
}

/*
<G> -> GT_tk | empty  
*/
static struct node * G() {
	
	struct node * newNode = getNode("G");
	
	if (tk->tokenID == GT_tk) { // predict <G> -> GT_tk
		newNode->t1 = tk; // process GT_tk (stored in node)
		tk = scanner();

		return newNode;
	}
	
	else // predict <G> -> empty
		return NULL;
}

/*
<E> -> EQ_tk | empty  
*/
static struct node * E() {
	
	struct node * newNode = getNode("E");
	
	if (tk->tokenID == EQ_tk) { // predict <E> -> EQ_tk
		newNode->t1 = tk; // process EQ_tk (stored in node)
		tk = scanner();

		return newNode;
	}
	
	else // predict <E> -> empty
		return NULL;
}

static void parserError(char * function) {

	if (tk->tokenID == ERR_tk) { // error occured in scanner, error message is handled in scanner
		exit(0);
		return;
	}
	else { // error occured in parser, error message is handled in parser
		printf("Parser Error (line %d): Syntax error in %s\n", tk->linenumber, function);
		exit(0);
		return;
	}
		
}