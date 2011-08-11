AUTOMAKE_OPTIONS=foreign
CLEANFILES = *~ core

INCLUDES= -I../include -I$(top_srcdir)/libcopl/include

AM_CFLAGS = -Werror -Wall

lib_LTLIBRARIES = libcopmanage.la libcopmalloc.la libcopcache.la libcop.la

# noinst_LIBRARIES = libcop.a

libcop_la_SOURCES = bignumber.c \
		cop/adm.c \
		cop/asym.c \
		cop/comp.c \
		cop/regx.c \
		cop/rnd.c \
		cop/sym.c \
		cop_session.c \
		cop_dispatch.c \
		deq.c

libcopcache_la_SOURCES = cop_cache_setup.c \
		cop_tls_cache.c

libcopmalloc_la_SOURCES = cop_pool.c \
		cop_malloc.c \
		cop_map_node.c \
		cop_map_tree.c \
		deq.c

libcopmanage_la_SOURCES = cop_open.c \
		cop_base.c

#This installs headers in /usr/include
libcop_la_includedir = ${prefix}/include
libcop_la_include_HEADERS = ../include/libcop.h ../include/cop_debug.h

libcopcache_la_includedir = ${prefix}/include
libcopcache_la_include_HEADERS = ../include/libcopcache.h

libcopmanage_la_includedir = ${prefix}/include
libcopmanage_la_include_HEADERS = ../include/libcopmanage.h

libcopmalloc_la_includedir = ${prefix}/include
libcopmalloc_la_include_HEADERS = ../include/libcopmalloc.h

libcop_la_LDFLAGS = -version-info 0:12:0 
libcopcache_la_LDFLAGS = -version-info 0:12:0 
libcopmanage_la_LDFLAGS = -version-info 0:12:0 
libcopmalloc_la_LDFLAGS = -version-info 0:12:0 

# Add special flags for configured variables
if REGX_SUPPORT
extra_libadd=-lregxum
endif

LIBCOPL_LA = ../../libcopl/lib/libcopl.la

libcop_la_LIBADD= -lpthread libcopmanage.la libcopmalloc.la libcopcache.la $(LIBCOPL_LA) ${extra_libadd}
libcopmanage_la_LIBADD=-lpthread $(LIBCOPL_LA)
libcopmalloc_la_LIBADD=-lpthread libcopmanage.la $(LIBCOPL_LA)
libcopcache_la_LIBADD=-lpthread $(LIBCOPL_LA)
