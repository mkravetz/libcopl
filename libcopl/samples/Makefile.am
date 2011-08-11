AUTOMAKE_OPTIONS=foreign 
CLEANFILES = *~ core

INCLUDES= -I../include -I../lib

bin_PROGRAMS = simple_adm simple_aes

LIBADD=../lib/libcopl.la -lpthread

simple_adm_SOURCES=simple_adm.c
simple_adm_LDADD=../lib/libcopl.la

simple_aes_SOURCES=simple_aes.c
simple_aes_LDADD=../lib/libcopl.la
