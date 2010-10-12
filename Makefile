
TESTS= $(wildcard tests/*.golden)

test: brainfuck $(TESTS:golden=output)

brainfuck: main.c
	cc -ggdb -o brainfuck main.c

tests/%.output: tests/%.golden tests/%.b
	./brainfuck -x -t tests/$*.golden tests/$*.b

clean:
	rm brainfuck
