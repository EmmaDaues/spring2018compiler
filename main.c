// Emma Daues
// CS 4280
// P4

// "main.c"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "filept.h"
#include "node.h"
#include "statSemantics.h"
#include "targetFilept.h"

FILE *filept;
FILE *targetFilept;
FILE *bottomFilept;

int main(int argc, char *argv[]) {
	
	// file variables
	char prefix[50];
	char *inputfilename;
	
	FILE * finalFilept;
	
	if (argc == 2) { // file name provided; read from file
	    
		finalFilept = fopen("file.asm", "w");
		
		strcpy(prefix, argv[1]);
		inputfilename = strcat(prefix, ".sp18");
		
		if (filept = fopen(inputfilename, "r")) {  // check if file exists
		
			printf("Reading from file.\n");
			
			// check to see if file is empty
			fseek (filept, 0, SEEK_END);
			int size = ftell(filept);

			if (size == 0) {
				printf("Error: File is empty.\n");
				return 1;
			}
			fseek(filept, 0, SEEK_SET);
			
		targetFilept = fopen("top.asm", "w");
	
		} else {
			
			printf("Error: File not found.\n");
			return 1;
		}
		
	} else if (argc == 1)  { // file name provided; redirect to file
							 // 				*OR*
							 // file name not provided; read from standard in
	
		printf("Reading from stdin or redirected file.\n");
		printf("***********************************\n");
		printf("If reading from stdin, enter tokens,\nthen hit [ENTER] and [CRTL+D]\n");
		printf("***********************************\n");
		
		finalFilept = fopen("out.asm", "w");
		
		/* write stdin to file */
		const int BUFSIZE =  10000;
			
		char buffer[BUFSIZE];
		
		FILE *writestdin = fopen("tmp.sp18", "w");
		
		if (writestdin == 0)
			printf("Error: Something went wrong while reading stdin\n");
		
		
		while (fgets(buffer, BUFSIZE, stdin) != NULL) {
			fputs(buffer, writestdin);

		}
		fclose(writestdin);
		
		/* read stdin from temp file */
		if (filept = fopen("tmp.sp18", "r")) {  // check if file exists
			
			// check to see if file is empty
			fseek (filept, 0, SEEK_END);
			int size = ftell(filept);

			if (size == 0) {
				printf("Error: Stream is empty.\n");
				return 1;
			}
			fseek(filept, 0, SEEK_SET);
		
			targetFilept = fopen("top.asm", "w");
		}
			
	} else {
		printf("Invalid arguments given\n");
		return 1;
	}

	struct node* parseTree;
	
	// create parse tree
	parseTree = parser();
	
	bottomFilept = fopen("bottom.asm", "w");
	
	// run tree through static semantics (variable checker)
	int j = -1;
	statSemantics(&parseTree, &j);		
	printf("Static Semantics OK\n");
	
	fprintf(targetFilept, "STOP\n");

	if (specialVariableFlag == 1) { // special variable needs to have storage directive
		fprintf(bottomFilept, "T100 0\n");
	}
	fclose(filept);
	fclose(targetFilept);
	fclose(bottomFilept);
	
	// merge 2 files together
	
	targetFilept = fopen("top.asm", "r");
	bottomFilept = fopen("bottom.asm", "r");
	char ch; 
	
	while((ch = fgetc(targetFilept)) != EOF) {
		
		fputc(ch, finalFilept);
	}
 
	while((ch = fgetc(bottomFilept)) != EOF) {
		
		fputc(ch, finalFilept);
	}
	
	fclose(targetFilept);
	fclose(bottomFilept);
	fclose(finalFilept);
	
	return 0;
}

