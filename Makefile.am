# Makefile.am for the SDL Rich Text Format loading library and viewer

lib_LTLIBRARIES = libSDL_rtf.la

libSDL_rtfincludedir = $(includedir)/SDL
libSDL_rtfinclude_HEADERS =	\
	SDL_rtf.h

libSDL_rtf_la_SOURCES =		\
	rtfactn.c		\
	rtfdecl.h		\
	rtfreadr.c		\
	rtftype.h		\
	SDL_rtf.c

EXTRA_DIST =			\
	CHANGES			\
	SDL_rtf.spec		\
	SDL_rtf.spec.in		\
	autogen.sh

libSDL_rtf_la_LDFLAGS = 		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libSDL_rtf_la_LIBADD = 

if HAVE_SDL_TTF
EXAMPLES = showrtf
else
EXAMPLES =
endif
noinst_PROGRAMS = $(EXAMPLES)

showrtf_SORUCES = showrtf.c
showrtf_LDADD = libSDL_rtf.la -lSDL_ttf

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz

