LEX_FILE = gpplexer.l
TARGET = myscanner
I_FILE = input.txt
LISP_FILE = main.lisp

flex:
	flex gpplexer.l
	gcc lex.yy.c -o $(TARGET)
	./$(TARGET) <$(I_FILE)

lisp:
	clisp $(LISP_FILE) $(I_FILE)


