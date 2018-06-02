// Emma Daues
// CS 4280
// P4

// "token.h"

#ifndef TOKEN_H
#define TOKEN_H

// enumeration to store token identifications
typedef enum {IDENT_tk, NUM_tk, EQ_tk, GT_tk, LT_tk, COL_tk, PLUS_tk, MINUS_tk, MULT_tk, DIV_tk, PCENT_tk, DOT_tk, LPAREN_tk, RPAREN_tk, COMM_tk, LBRACE_tk, RBRACE_tk, SEMCOL_tk, LBRACK_tk, RBRACK_tk, EOF_tk, ERR_tk, START_tk, STOP_tk, ITER_tk, VOID_tk, VAR_tk, RETURN_tk, READ_tk, PRINT_tk,
PROG_tk, IFF_tk, THEN_tk, LET_tk} ID;

/*
	Identifier: Start with lowercase letter, followed by any sequence of letters or digits
	Keywords: start stop iter void var return read print program iff then let (have unique token IDs)
	Number: Any sequence of digits
*/
	
typedef struct token {
	
	int linenumber;
	// stores which line the token is found on
	
	ID tokenID;
	// stores the ID enumeration of the token
	
	char *instance;
	// stores instance of the token as a string
	
	
} token;

#endif