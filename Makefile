OBJS = geohexv3.o tmain.o
CFLAGS += -Wall -Werror -pedantic-errors -std=c99 -O3

tmain: $(OBJS)
	$(CC) -o tmain $(OBJS)

clean:
	rm -rf *.o tmain
