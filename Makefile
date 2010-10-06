
test: brainfuck
	./brainfuck hello.b
	./brainfuck squares.b
#	./brainfuck -v tests.b
#	./brainfuck errors.b || true

brainfuck: main.c
	cc -ggdb -o brainfuck main.c

clean:
	rm brainfuck
