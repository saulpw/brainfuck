
PASSTESTS= $(wildcard tests/*.golden)
FAILTESTS= $(wildcard tests/*.fail)

test: passes

passes: brainfuck $(PASSTESTS:golden=output)

fails: brainfuck $(FAILTESTS:fail=error)

brainfuck: main.c
	cc -ggdb -o brainfuck main.c

tests/%.output: tests/%.golden tests/%.b
	./brainfuck -t tests/$*.golden tests/$*.b

tests/%.error: tests/%.fail tests/%.b
	./brainfuck -t tests/$*.expected tests/$*.b

clean:
	rm -f brainfuck
