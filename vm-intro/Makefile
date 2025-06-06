build: memory_user.c
	clang -o memory_user.o -pthread memory_user.c

clean:
	rm -f ./memory_user.o

run:
	./memory_user.o $(SIZE) $(DURATION)
