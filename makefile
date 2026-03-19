CFLAGS=-Og -g -fsanitize=address,undefined -Wall -Wextra -pedantic
CPPFLAGS=-MMD -MF $*.d

all: server client

-include *.d

client: src/client/client.c src/logging.c src/client/commands.c
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

server: src/server/server.c src/server/commands.c src/server/networking.c src/server/client.c src/logging.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@$(RM) -v server client *.o *.d
	
.PHONY: all clean
.PRECIOUS: *.o *.d
