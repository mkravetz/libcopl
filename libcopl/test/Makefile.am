AUTOMAKE_OPTIONS=foreign
CLEANFILES = *~ core

INCLUDES= -I../include -I../lib

AM_CFLAGS = -Werror -Wall

check_PROGRAMS = aes des adm_memcpy hash all_headers all_headers_cpp gzip

SUBDIRS=perf

LIBADD=../lib/libcopl.la -lpthread

aes_SOURCES=aes.c
aes_LDADD=-lcrypto ../lib/libcopl.la
des_SOURCES=des.c
des_LDADD=-lcrypto ../lib/libcopl.la
adm_memcpy_SOURCES=adm_memcpy.c
adm_memcpy_LDADD=../lib/libcopl.la
hash_SOURCES=hash.c
hash_LDADD=../lib/libcopl.la
gzip_SOURCES=gzip.c
gzip_LDADD=../lib/libcopl.la

all_headers_SOURCES=all_headers.c
all_headers_CFLAGS=${AM_CFLAGS} -Wpacked -Wpadded
#all_headers_CFLAGS=-Wall -Wpacked -Wpadded
all_headers_LDADD=../lib/libcopl.la

all_headers_cpp_SOURCES=all_headers.cpp
all_headers_cpp_LDADD=../lib/libcopl.la

