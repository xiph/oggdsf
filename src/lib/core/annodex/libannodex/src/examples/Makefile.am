## Process this file with automake to produce Makefile.in

AM_CFLAGS = -Wall -pedantic -g

INCLUDES = -I$(top_srcdir)/include

ANNODEXDIR = ../libannodex
ANNODEX_LIBS = $(ANNODEXDIR)/libannodex.la $(OGGZ_LIBS) -lm

# Programs to build
noinst_PROGRAMS = print-title-file print-title-memory print-lots \
	write-clip-file

print_title_file_SOURCES = print-title-file.c
print_title_file_LDADD = $(ANNODEX_LIBS)

print_title_memory_SOURCES = print-title-memory.c
print_title_memory_LDADD = $(ANNODEX_LIBS)

print_lots_SOURCES = print-lots.c
print_lots_LDADD = $(ANNODEX_LIBS)

write_clip_file_SOURCES = write-clip-file.c
write_clip_file_LDADD = $(ANNODEX_LIBS)
