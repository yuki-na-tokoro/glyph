CFLAGS=-Og -g -fsanitize=address,undefined -Wall -Wextra -pedantic

all: server client

client: src/client/client.c src/client/commands.c src/client/screen.c
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

server: src/server/server.c src/server/commands.c src/server/networking.c src/server/client.c src/logging.c src/hashtypes.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@$(RM) -v server client *.o *.d
	
.PHONY: all clean
