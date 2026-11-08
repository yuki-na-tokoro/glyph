AM_CFLAGS = -Wall -Wextra -pedantic

noinst_LIBRARIES    = common/libcommon.a
common_libcommon_a_SOURCES = common/logging.c common/logging.h \
						common/hashtypes.c common/hashtypes.h \
						common/protocol.h

bin_PROGRAMS = glyph-client glyph-server

glyph_client_SOURCES = client/client.c client/client.h \
						client/commands.c client/commands.h \
						client/screen.c client/screen.h

glyph_client_CPPFLAGS = -I$(srcdir)/common $(NCURSES_CFLAGS)
glyph_client_LDADD = common/libcommon.a $(NCURSES_LIBS)

glyph_server_SOURCES = server/server.c \
						server/client.c server/client.h \
						server/networking.c server/networking.h \
						server/commands.c server/commands.h \
						server/data.c server/data.h

glyph_server_CPPFLAGS = -I$(srcdir)/common
glyph_server_LDADD = common/libcommon.a
