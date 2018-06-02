// Emma Daues
// CS 4280
// P4

// "scanner.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "scanner.h"
#include "filept.h"

// FA Table, a 2D array 
static int FATable[22][24] = { 
{   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,   21, 22,   23}, // column index
{   1,   -2,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,   21, -1, 1020}, // state1
{1000,    2,    2,    2, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, -1, 1000}, // state2
{1001, 1001, 1001,    3, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, -1, 1001}, // state3
{1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, -1, 1002}, // state4
{1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, -1, 1003}, // state5
{1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, 1004, -1, 1004}, // state6
{1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, 1005, -1, 1005}, // state7
{1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, 1006, -1, 1006}, // state8
{1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, 1007, -1, 1007}, // state9
{1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, 1008, -1, 1008}, // state10
{1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, 1009, -1, 1009}, // state11
{1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, 1010, -1, 1010}, // state12
{1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, 1011, -1, 1011}, // state13
{1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, 1012, -1, 1012}, // state14
{1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, 1013, -1, 1013}, // state15
{1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, 1014, -1, 1014}, // state16
{1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, 1015, -1, 1015}, // state17
{1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, 1016, -1, 1016}, // state18
{1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, 1017, -1, 1017}, // state19
{1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, 1018, -1, 1018}, // state20
{1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, 1019, -1, 1019} // state21
};


/* Column index:
	0: White Space
	1: A-Z
	2: a-z
	3: 0-9
	4:  =
	5:  >
	6:  <
	7:  :
	8:  +
	9:  -
	10: *
	11: /
	12: %
	13: .
	14: (
	15: )
	16: ,
	17: }
	18: {
	19: ;
	20: [
	21: ]
	22: Character not in alphabet
	23: EOF
*/

/* Final States for different tokens:
	1000: IDENT_tk or Keyword (checked for specifics later)
	1001: NUM_tk
	1002: EQ_tk
	1003: GT_tk
	1004: LT_tk
	1005: COL_tk
	1006: PLUS_tk
	1007: MINUS_tk
	1008: MULT_tk
	1009: DIV_tk
	1010: PCENT_tk
	1011: DOT_tk
	1012: LPAREN_tk
	1013: RPAREN_tk
	1014: COMM_tk
	1015: LBRACE_tk
	1016: RBRACE_tk
	1017: SEMCOL_tk
	1018: LBRACK_tk
	1019: RBRACK_tk
	1020: EOF_tk
	
*/

/* Errors detected:
	-1: symbol not in lexical alphabet
	-2: no tokens start wth a capital letter
	-3: unidentified final state
*/


/* Function Prototypes */
static void scannerError(int);
static int getColumn(char);
static void append(char*, char, int*);
static ID keywordLookup(char*);
static ID finalstateLookup(int);

static int nlFlag = 0;

char filter(FILE* pt) {
	char c = fgetc(pt);
	
	// check for comments
	if (c == '&') {
		do { // skip over comment section
			c = fgetc(pt);
			
			// check for new lines within comments
			if (c == '\n') {
				lineNum++;
			}
			
		} while (c != '&');
		
		c = fgetc(pt); // get next character after comments
	}
	
	// check for newlines
	if (nlFlag == 1) {
		lineNum++;
		nlFlag = 0;
	}

	if (c == '\r') {
		
		c = fgetc(pt); // skip \r if present
	}
	if (c == '\n') {
			c = ' '; // separate tokens from sepaprate lines
		
		nlFlag = 1; // set newline flag so next 
					// token is on next line
	}
		
	return c;
}

struct token *scanner() {
	
	// new token
	struct token *newToken = (token *)malloc(sizeof(token));
	newToken->instance = (char *)malloc(9);
	int stringIndex = 0;
	newToken->instance[0] = '\0';
	
	// initialize state (s1)
	int state = 1;
	
	// declare variable for next state
	int nextState = 0;
	
	while (nextState < 1000) { // while state is not final

		nextState = FATable[state][getColumn(nextChar)];
		
		if (nextState < 0) { // if error is encountered
		
			scannerError(nextState);
			
			newToken->tokenID = ERR_tk;
			return newToken;			
		}
		
		if (nextState >= 1000) { // if nextState is final
			
			// append '\0' to token instance
			append(newToken->instance, '\0', &stringIndex);
		
			if (nextState == 1000) { // token is either keyword or identifier
				
				// cross-reference instance with list of reserved words
				// return KW_tk if found, return IDENT_tk if not
				newToken->tokenID = keywordLookup(newToken->instance);
				newToken->linenumber = lineNum;
				return newToken;
				
			}
			else {
				newToken->tokenID = finalstateLookup(nextState);
				newToken->linenumber = lineNum;
				return newToken;
			
			}
		}
		
		else { // state is not final

			// append nextChar to instance 
			append(newToken->instance, nextChar, &stringIndex);
			stringIndex++;
			
			// get next state
			state = nextState;
			
			// get next character
			nextChar = filter(filept);
			
		}
	}
	
}

static void scannerError(int i) {
	
	if (i == -1) {
		printf("Scanner Error (line %d): Invalid character input\n", lineNum);
	}
	else if (i == -2) {
		printf("Scanner Error (line %d):No valid token starts with capital letter\n", lineNum);
	}
	else if (i >= 1000) {
		printf("Scanner Error line %d): Unknown final state occurred\n", lineNum);
	}
	
	return;
}

static int getColumn(char c) {
	
	if (c == ' ') // WHITESPACE
		return 0;
	else if ((c >= 'A' ) && (c <= 'Z')) // UPPERCASE
		return 1;
	else if ((c >= 'a' ) && (c <= 'z')) // LOWERCASE
		return 2;
	else if ((c >= '0' ) && (c <= '9')) // NUMBER
		return 3;
	else if (c == '=')
		return 4;
	else if (c == '>')
		return 5;	
	else if (c == '<')
		return 6;
	else if (c == ':')
		return 7;
	else if (c == '+')
		return 8;
	else if (c == '-')
		return 9;
	else if (c == '*')
		return 10;
	else if (c == '/')
		return 11;
	else if (c == '%')
		return 12;
	else if (c == '.')
		return 13;
	else if (c == '(')
		return 14;
	else if (c == ')')
		return 15;
	else if (c == ',')
		return 16;
	else if (c == '{')
		return 17;
	else if (c == '}')
		return 18;
	else if (c == ';')
		return 19;
	else if (c == '[')
		return 20;
	else if (c == ']') 
		return 21;
	else if (feof(filept)) // EOF
		return 23;
	else // NOT IN ALPHABET
		return 22;		
}

static void append(char* string, char c, int* i) {

	// make sure whitespaces don't get added to strings
	if (c == ' ') {
		
		(*i)--;
	}
	else {
		
		string[*i] = c;
	}
	
	return;
}

static ID keywordLookup(char* word) {

	/* keywords */
	if (strcmp(word, "start") == 0)
		return START_tk;
	else if (strcmp(word, "stop") == 0)
		return STOP_tk;
	else if (strcmp(word, "iter") == 0)
		return ITER_tk;
	else if (strcmp(word, "void") == 0)
		return VOID_tk;
	else if (strcmp(word, "var") == 0)
		return VAR_tk;
	else if (strcmp(word, "return") == 0)
		return RETURN_tk;
	else if (strcmp(word, "read") == 0)
		return READ_tk;
	else if (strcmp(word, "print") == 0)
		return PRINT_tk;
	else if (strcmp(word, "program") == 0)
		return PROG_tk;
	else if (strcmp(word, "iff") == 0)
		return IFF_tk;
	else if (strcmp(word, "then") == 0)
		return THEN_tk;
	else if (strcmp(word, "let") == 0)
		return LET_tk;
	
	/* identifiers */
	else {
		return IDENT_tk;
	}
}

static ID finalstateLookup(int state) {
	
	switch (state) {
		case 1001:
			return NUM_tk;
			break;
		case 1002: 
			return EQ_tk;
			break;
		case 1003: 
			return GT_tk;
			break;
		case 1004: 
			return LT_tk;
			break;
		case 1005: 
			return COL_tk;
			break;
		case 1006: 
			return PLUS_tk;
			break;
		case 1007: 
			return MINUS_tk;
			break;
		case 1008: 
			return MULT_tk;
			break;
		case 1009:
			return DIV_tk;
			break;
		case 1010: 
			return PCENT_tk;
			break;
		case 1011: 
			return DOT_tk;
			break;
		case 1012: 
			return LPAREN_tk;
			break;
		case 1013: 
			return RPAREN_tk;
			break;
		case 1014: 
			return COMM_tk;
			break;
		case 1015: 
			return LBRACE_tk;
			break;
		case 1016: 
			return RBRACE_tk;
			break;
		case 1017: 
			return SEMCOL_tk;
			break;
		case 1018: 
			return LBRACK_tk;
			break;
		case 1019: 
			return RBRACK_tk;
			break;
		case 1020: 
			return EOF_tk;
			break;
		default:
			scannerError(1000);
			return ERR_tk;
			break;
				
	}
}
