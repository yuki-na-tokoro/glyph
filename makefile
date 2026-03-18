CFLAGS=-Og -g -fsanitize=address,undefined -Wall -Wextra -pedantic
CPPFLAGS=-MMD -MF $*.d

all: server client

-include *.d

%.o: %.d
%.d: ;

clean:
	@$(RM) -v server client *.o *.d
	
.PHONY: all clean
.PRECIOUS: *.o *.d
