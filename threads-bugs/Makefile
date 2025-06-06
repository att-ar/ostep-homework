
ALL = vector-deadlock vector-global-order vector-try-wait vector-avoid-hold-and-wait vector-nolock
COMMON = vector-header.h main-common.c main-header.h common.h common_threads.h

all: $(ALL)

clean:
	rm -f $(ALL) *~

vector-deadlock: vector-deadlock.c $(COMMON)
	clang -o vector-deadlock vector-deadlock.c -Wall -pthread -O

vector-global-order: vector-global-order.c $(COMMON)
	clang -o vector-global-order vector-global-order.c -Wall -pthread -O

vector-try-wait: vector-try-wait.c $(COMMON)
	clang -o vector-try-wait vector-try-wait.c -Wall -pthread -O

vector-avoid-hold-and-wait: vector-avoid-hold-and-wait.c $(COMMON)
	clang -o vector-avoid-hold-and-wait vector-avoid-hold-and-wait.c -Wall -pthread -O

vector-nolock: vector-nolock.c $(COMMON)
	clang -o vector-nolock vector-nolock.c -Wall -pthread -O

