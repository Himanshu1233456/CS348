CS348 <-> Assignment-4 ( PARSER )

Submitted By - Himanshu [180101031]

--- Coded & Tested on Windows OS ---

-> For compiling lex, "flex" compiler is needed
-> For compiling yacc, "win_bison" compiler is needed
-> For compiling makefile, "make" compiler is needed

1. To compile the Parser, run the command [ make ] in the directory containing makefile, yacc, lex and cpp file.
2. A file named "parser.exe" will be created on successful compiling with make command.  
3. To run the parser, use the command [ parser <filename> ] which will run the parser on the given input file as an argument. For eg. To run the parser on "input.txt", use the command [ parser input.txt or parser "input.txt" ].
4. There is no need to run make command again for parsing different files or the same file with some changes. The above command is sufficient.
5. On successful compiling and running, the output will be the errors which the input file conatains.
6. To delete all the extra files created and the parser, use the command [ make clean ].