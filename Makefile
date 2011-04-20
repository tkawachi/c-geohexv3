OBJS = geohexv3.o tmain.o
CFLAGS += -Wall -Werror -pedantic-errors -std=c99 -O3
LDFLAGS += -lm

tmain: $(OBJS)
	$(CC) $(LDFLAGS) -o tmain $(OBJS)

clean:
	rm -rf *.o tmain
