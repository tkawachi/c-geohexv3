OBJS = geohexv3.o tmain.o
CFLAGS += -Wall -Werror -pedantic-errors -std=c99
all: $(OBJS)
	$(CC) -o tmain $(OBJS)
