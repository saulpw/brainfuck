
test: brainfuck
	./brainfuck hello.bf

brainfuck: main.c
	cc -ggdb -o brainfuck main.c

clean:
	rm brainfuck
