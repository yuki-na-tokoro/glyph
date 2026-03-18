CFLAGS=-Og -g -fsanitize=address,undefined -Wall -Wextra -pedantic
CPPFLAGS=-MMD -MF $*.d

all: server client

server: src/server/main.c
client: src/client/main.c

-include *.d

%.o: %.d
%.d: ;

clean:
	@$(RM) -v server client *.o *.d
	
.PHONY: all clean server client
.PRECIOUS: *.o *.d
