AM_CFLAGS = -Wall --pedantic-error --std=c99
AM_CPPFLAGS = -D_ANSI_SOURCE

lib_LTLIBRARIES = libgeohex3.la
libgeohex3_la_SOURCES = geohex3.c geohex3.h

bin_PROGRAMS = geohex3
geohex3_SOURCES = main.c
geohex3_LDADD = libgeohex3.la
