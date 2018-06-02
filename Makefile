all: main.c scanner.c parser.c statSemantics.c node.c
	gcc -o comp main.c scanner.c parser.c statSemantics.c node.c

  clean: 
	  $(RM) comp tmp.sp18 bottom.asm top.asm
