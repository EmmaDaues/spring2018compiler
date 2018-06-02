# spring2018compiler
Final project for CS4280
Storage=Local

// Emma Daues
// CS 4280
// P4

// "README.txt"


====SCOPING====

This project uses the LOCAL OPTION for static semantics scoping

Program processes variables in the outside <vars> as global and all other variables as local

====STORAGE====

This project uses the LOCAL OPTION for strage allocation. Variables outside of a block are scoped in the entire program; variables in a block are scoped in that block

====FILES====

Makefile    - makefile for project
README.txt  - readme file for project
VirtMach    - used to execute the target .asm files

token.h          - contains data about token struct
filept.h         - contains data about shared file pointer
targetFilept.h         - contains data about shared file pointer
node.h           - contains data about node structure for parse tree & header file for "node.c"
scanner.h        - header file for "scanner.c"
parser.h         - header file for "parser.c"
statSemantics.h  - header file for "statSemantics.c"

main.c           - contains main function and opens file for reading
node.c		     - contains function to initialize new node
scanner.c        - uses FSA to create and return tokens
parser.c         - uses tokens from scanner to build parse tree
statSemantics.c  - traverses through parse tree and checks for proper variable declaration & generates code


====TO RUN====

- type "make clean" to clean up extra files
- type "make" to compile files into executable "comp"
- type "./comp [filename]" to run project for specific file "[filename].sp18"
- type "./comp" to run project with stdin

- program generates "file.asm" if using file input, or "out.asm" if using stdin
- run file by typing "./VirtMach [filename]", where [filename] is either "file.asm" or "out.asm"

====LL(1) GRAMMAR====

<program> -> PROG_tk <vars> <block>                                                     (k = 0) - stores 0 tokens in nodes
<block>   -> START_tk <vars> <stats> STOP_tk                                            (k = 0) - stores 0 tokens in nodes
<vars>    -> VAR_tk IDENT_tk EQ_tk NUM_tk <mvars> | empty                               (k = 1) - stores 3 tokens in nodes
<mvars>   -> DOT_tk | COL_tk IDENT_tk <mvars>                                           (k = 1) - stores 1 tokens in nodes
<expr>    -> <M> <X>                                                                    (k = 0) - stores 0 tokens in nodes
<X>       -> PLUS_tk <expr> | MINUS_tk <expr> | DIV_tk <expr> | MULT_tk <expr> | empty  (k = 1) - stores 1 tokens in nodes
<M>       -> PCENT_tk <M> | <R>                                                         (k = 1) - stores 1 tokens in nodes
<R>       -> LPAREN_tk <expr> RPAREN_tk | IDENT_tk | NUM_tk                             (k = 1) - stores 1 tokens in nodes
<stats>   -> <stat> <mStat>                                                             (k = 0) - stores 0 tokens in nodes
<mStat>   -> <stat> <mStat> | empty                                                     (k = 1) - stores 0 tokens in nodes
<stat>    -> <in> | <out> | <block> | <iff> | <loop> | <assign>                         (k = 1) - stores 0 tokens in nodes
<in>      -> READ_tk IDENT_tk DOT_tk                                                    (k = 0) - stores 1 tokens in nodes
<out>     -> PRINT_tk <expr> DOT_tk                                                     (k = 0) - stores 0 tokens in nodes
<iff>     -> IFF_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat>                       (k = 0) - stores 0 tokens in nodes
<loop>    -> ITER_tk LPAREN_tk <expr> <RO> <expr> RPAREN_tk <stat>                      (k = 0) - stores 0 tokens in nodes
<assign>  -> LET_tk IDENT_tk EQ_tk <expr> DOT_tk                                        (k = 0) - stores 2 tokens in nodes
<RO>      -> LT_tk <L> | GT_tk <G> | EQ_tk <E>                                          (k = 1) - stores 1 tokens in nodes
<L>       -> LT_tk | empty                                                              (k = 1) - stores 1 tokens in nodes
<G>       -> GT_tk | empty                                                              (k = 1) - stores 1 tokens in nodes
<E>       -> EQ_tk | empty                                                              (k = 1) - stores 1 tokens in nodes

>>> TOKENS THAT ARE STORED IN NODES (IDs, numbers, operators):
    - IDENT_tk
	- NUM_tk
	- EQ_tk
	- GT_tk
	- LT_tk
	- PLUS_tk
	- MINUS_tk
	- MULT_tk
	- DIV_tk
	- PCENT_tk
	
====PROBLEMS====

Nested <expr>s do not always work due to the nature of how the <X> production is set up
