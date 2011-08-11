AUTOMAKE_OPTIONS=foreign
CLEANFILES = *~ core

INCLUDES= -I../include -I$(top_srcdir)

#AM_CFLAGS = -Werror -Wall -Wpacked -Wpadded
AM_CFLAGS = -Werror -Wall

lib_LTLIBRARIES = libcopl.la

# noinst_LIBRARIES = libcop.a

libcopl_la_SOURCES = cop_sys.c cop_err.c cop_util.c cop_asym.c

#This installs headers in /usr/include
libcopl_la_includedir = ${prefix}/include
libcopl_la_include_HEADERS = ../include/libcopl.h

libcopl2_la_includedir = ${prefix}/include/libcopl
libcopl2_la_include_HEADERS = ../include/libcopl/cop.h \
			../include/libcopl/icswx.h ../include/libcopl/waitm.h \
			../include/libcopl/regx.h ../include/libcopl/crypto.h \
			../include/libcopl/comp.h ../include/libcopl/cop_sys.h \
			../include/libcopl/rnd.h ../include/libcopl/cop_err.h \
			../include/libcopl/cop_util.h \
			../include/libcopl/icswx_util.h \
			../include/libcopl/asym.h \
			../include/libcopl/adm.h

libcopl3_la_includedir = ${prefix}/include/libcopl/crypto
libcopl3_la_include_HEADERS = ../include/libcopl/crypto/aes.h \
				../include/libcopl/crypto/aes_hmac.h \
				../include/libcopl/crypto/tdes_hmac.h \
				../include/libcopl/crypto/des.h \
				../include/libcopl/crypto/rc4.h \
				../include/libcopl/crypto/hmac.h \
				../include/libcopl/crypto/kasumi.h \
				../include/libcopl/crypto/hash.h \
				../include/libcopl/crypto/hash_ex.h \
				../include/libcopl/crypto/imf.h \
				../include/libcopl/crypto/ecc.h
libcopl4_la_includedir = ${prefix}/include/libcopl/comp
libcopl4_la_include_HEADERS = ../include/libcopl/comp/deflate.h \
				../include/libcopl/comp/gzip.h \
				../include/libcopl/comp/zlib.h

libcopl_la_LDFLAGS = -version-info 0:12:0 
libcopl_la_CFLAGS = -D_GNU_SOURCE ${AM_CFLAGS}


libcopl_la_LIBADD = -lpthread
