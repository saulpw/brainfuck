
test: brainfuck
	./brainfuck -t hello.golden hello.b
	./brainfuck -t squares.golden squares.b
	./brainfuck -t boundsleft.golden boundsleft.b
	./brainfuck -t boundsright.golden boundsright.b
	./brainfuck -t obscure-problems.golden obscure-problems.b
	./brainfuck -x -t atoi.golden atoi.b
	./brainfuck -x -t crtest.golden crtest.b
	./brainfuck -x -t numwarp.golden numwarp.b
#	./brainfuck -x -t rot13.golden rot13.b
#	./brainfuck -t unmatched.golden unmatched.b

brainfuck: main.c
	cc -ggdb -o brainfuck main.c

clean:
	rm brainfuck
