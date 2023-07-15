CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LDFLAGS = -lm

OBJS_E = encode.o trie.o word.o io.o
OBJS_D = decode.o trie.o word.o io.o

debug: CFLAGS+= -g -gdwarf-4
debug: encode decode

all: encode decode
	
encode: $(OBJS_E)
	$(CC) $(LDFLAGS) -o encode $(OBJS_E)
		
decode: $(OBJS_D)
	$(CC) $(LDFLAGS) -o decode $(OBJS_D)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f encode decode $(OBJS_E) $(OBJS_D)

format:
	clang-format -i -style=file *.[ch]
