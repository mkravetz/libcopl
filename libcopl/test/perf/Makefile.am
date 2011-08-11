AUTOMAKE_OPTIONS=foreign
CLEANFILES = *~ core

INCLUDES= -I../../include -I../../lib

AM_CFLAGS = -Werror -Wall

check_PROGRAMS = \
	aes_mt \
	des_mt \
	tdes_mt \
	sha1_mt \
	sha256_mt \
	sha512_mt \
	hmacsha1_mt \
	hmacsha256_mt \
	hmacsha512_mt \
	md5_mt \
	arc4_mt \
	kasumi_f9_mt \
	aeshmacsha1_mt \
	aeshmacsha256_mt \
	aeshmacsha512_mt \
	tdeshmacsha1_mt \
	tdeshmacsha256_mt \
	tdeshmacsha512_mt \
	gzip_decomp_perf

LIBADD=../../lib/libcopl.la -lpthread
LDADD=../../lib/libcopl.la -lpthread
LDFLAGS=-all-static

aes_mt_SOURCES=aes_mt.c
des_mt_SOURCES=des_mt.c
tdes_mt_SOURCES=tdes_mt.c
sha1_mt_SOURCES=sha1_mt.c
sha256_mt_SOURCES=sha256_mt.c
sha512_mt_SOURCES=sha512_mt.c
hmacsha1_mt_SOURCES=hmacsha1_mt.c
hmacsha256_mt_SOURCES=hmacsha256_mt.c
hmacsha512_mt_SOURCES=hmacsha512_mt.c
md5_mt_SOURCES=md5_mt.c
arc4_mt_SOURCES=arc4_mt.c
kasumi_f9_mt_SOURCES=kasumi_f9_mt.c
aeshmacsha1_mt_SOURCES=aeshmacsha1_mt.c
aeshmacsha256_mt_SOURCES=aeshmacsha256_mt.c
aeshmacsha512_mt_SOURCES=aeshmacsha512_mt.c
tdeshmacsha1_mt_SOURCES=tdeshmacsha1_mt.c
tdeshmacsha256_mt_SOURCES=tdeshmacsha256_mt.c
tdeshmacsha512_mt_SOURCES=tdeshmacsha512_mt.c

gzip_decomp_perf_SOURCES=gzip_decomp_perf.c


