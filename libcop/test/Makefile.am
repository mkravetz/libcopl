AUTOMAKE_OPTIONS=foreign 
CLEANFILES = *~ core

INCLUDES= -I../include -I../lib

check_PROGRAMS_default = \
mmutest001 \
mmutest002 \
mmutest003 \
mmutest004 \
mmutest005 \
mmutest006 \
mmutest007 \
mmutest008 \
admtest001 \
admtest002 \
admtest003 \
admtest004 \
admtest005 \
asymtest001 \
asymtest004 \
asymtest005 \
bntest001 \
err001 \
forktest001 \
forktest002 \
hashtest002 \
hashtest003 \
regxtest004 \
regxtest012 \
rndtest001 \
rndtest002 \
rndtest003 \
rndtest004 \
threadtest001 \
threadtest002 \
threadtest003 \
threadtest004 \
threadtest005 \
threadtest006 \
comptest003 \
comptest004 \
comptest007 \
comptest008 \
comptest010 \
comptest015 \
comptest016 \
comptest017 \
comptest018 \
comptest019

if PERF_ON
check_PROGRAMS_perf = \
perf_aes_sync \
perf_aes_async \
perf_aes_sync_noPapi \
perf_aes_async_noPapi \
perf_des_sync \
perf_des_async \
perf_des_sync_noPapi \
perf_des_async_noPapi \
perf_arc4_sync \
perf_arc4_async \
perf_arc4_sync_noPapi \
perf_arc4_async_noPapi \
perf_kasumi_sync \
perf_kasumi_sync_noPapi \
perf_md5_sync \
perf_md5_async \
perf_md5_sync_noPapi \
perf_md5_async_noPapi \
perf_sha_sync \
perf_sha_async \
perf_sha_sync_noPapi \
perf_sha_async_noPapi \
perf_asym_sync \
perf_asym_async \
perf_asym_sync_noPapi \
perf_asym_async_noPapi \
perf_combo_sync \
perf_combo_async \
perf_combo_sync_noPapi \
perf_combo_async_noPapi
else
check_PROGRAMS_perf =
endif

if OPENSSL_ON
check_PROGRAMS_openssl = \
asymtest002 \
asymtest003 \
asymtest006 \
ciphertest001 \
hashtest001 \
hashtest004 \
hmactest001
else
check_PROGRAMS_openssl =
endif

check_PROGRAMS= \
${check_PROGRAMS_default} \
${check_PROGRAMS_perf} \
${check_PROGRAMS_openssl} 

LIBADD=../lib/libcop.la -lpthread

admtest001_LDADD= ../lib/libcop.la -lpthread
admtest001_SOURCES=admtest001.c
admtest002_LDADD=../lib/libcop.la -lpthread
admtest002_SOURCES=admtest002.c
admtest003_LDADD=../lib/libcop.la -lpthread
admtest003_SOURCES=admtest003.c
admtest004_LDADD=../lib/libcop.la -lpthread
admtest004_SOURCES=admtest004.c
admtest005_LDADD=../lib/libcop.la -lpthread
admtest005_SOURCES=admtest005.c
asymtest001_LDADD=../lib/libcop.la -lpthread
asymtest001_SOURCES=asymtest001.c
asymtest004_LDADD=../lib/libcop.la -lpthread
asymtest004_SOURCES=asymtest004.c
asymtest005_LDADD=../lib/libcop.la -lpthread
asymtest005_SOURCES=asymtest005.c
bntest001_LDADD=../lib/libcop.la -lpthread
bntest001_SOURCES=bntest001.c
comptest003_LDADD=../lib/libcop.la -lpthread
comptest003_SOURCES=comptest003.c
comptest004_LDADD=../lib/libcop.la -lpthread
comptest004_SOURCES=comptest004.c
comptest007_LDADD=../lib/libcop.la -lpthread
comptest007_SOURCES=comptest007.c
comptest008_LDADD=../lib/libcop.la -lpthread
comptest008_SOURCES=comptest008.c
comptest010_LDADD=../lib/libcop.la -lpthread
comptest010_SOURCES=comptest010.c
comptest015_LDADD=../lib/libcop.la -lpthread -lm
comptest015_SOURCES=comptest015.c
comptest016_LDADD=../lib/libcop.la -lpthread -lm
comptest016_SOURCES=comptest016.c
comptest017_LDADD=../lib/libcop.la -lpthread -lm
comptest017_SOURCES=comptest017.c
comptest018_LDADD=../lib/libcop.la -lpthread -lm 
comptest018_SOURCES=comptest018.c
comptest019_LDADD=../lib/libcop.la -lpthread -lm
comptest019_SOURCES=comptest019.c
err001_LDADD=../lib/libcop.la -lpthread
err001_SOURCES=err001.c
forktest001_LDADD=../lib/libcop.la -lpthread
forktest001_SOURCES=forktest001.c
forktest002_LDADD=../lib/libcop.la -lpthread
forktest002_SOURCES=forktest002.c
hashtest002_LDADD=../lib/libcop.la -lpthread
hashtest002_SOURCES=hashtest002.c
hashtest003_LDADD=../lib/libcop.la -lpthread
hashtest003_SOURCES=hashtest003.c         
mmutest002_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest002_SOURCES=mmutest002.c
mmutest003_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest003_SOURCES=mmutest003.c
mmutest004_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest004_SOURCES=mmutest004.c
mmutest005_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest005_SOURCES=mmutest005.c
mmutest006_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest006_SOURCES=mmutest006.c
mmutest007_LDADD=../lib/libcopmalloc.la ../lib/libcopmanage.la -lpthread
mmutest007_SOURCES=mmutest007.c
mmutest008_LDADD=../lib/libcop.la -lpthread
mmutest008_SOURCES=mmutest008.c
mmutest001_LDADD=../lib/libcop.la -lpthread
mmutest001_SOURCES=mmutest001.c
regxtest004_LDADD=../lib/libcop.la -lpthread
regxtest004_SOURCES=regxtest004.c
regxtest012_LDADD=../lib/libcop.la -lpthread
regxtest012_SOURCES=regxtest012.c
rndtest001_LDADD=../lib/libcop.la -lpthread
rndtest001_SOURCES=rndtest001.c
rndtest002_LDADD=../lib/libcop.la -lpthread
rndtest002_SOURCES=rndtest002.c
rndtest003_LDADD=../lib/libcop.la -lpthread
rndtest003_SOURCES=rndtest003.c
rndtest004_LDADD=../lib/libcop.la -lpthread
rndtest004_SOURCES=rndtest004.c
threadtest001_LDADD= ../lib/libcop.la -lpthread
threadtest001_SOURCES=threadtest001.c
threadtest002_LDADD=../lib/libcop.la -lpthread
threadtest002_SOURCES=threadtest002.c
threadtest003_LDADD=../lib/libcop.la -lpthread
threadtest003_SOURCES=threadtest003.c
threadtest004_LDADD=../lib/libcop.la -lpthread
threadtest004_SOURCES=threadtest004.c
threadtest005_LDADD=../lib/libcop.la -lpthread
threadtest005_SOURCES=threadtest005.c
threadtest006_LDADD=../lib/libcop.la -lpthread
threadtest006_SOURCES=threadtest006.c

if PERF_ON
perf_aes_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_aes_sync_SOURCES=perf_aes_sync.c
perf_aes_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_aes_async_SOURCES=perf_aes_async.c
perf_aes_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto
perf_aes_sync_noPapi_SOURCES=perf_aes_sync_noPapi.c
perf_aes_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto
perf_aes_async_noPapi_SOURCES=perf_aes_async_noPapi.c
perf_des_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_des_sync_SOURCES=perf_des_sync.c
perf_des_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_des_async_SOURCES=perf_des_async.c
perf_des_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_des_sync_noPapi_SOURCES=perf_des_sync_noPapi.c
perf_des_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_des_async_noPapi_SOURCES=perf_des_async_noPapi.c
perf_arc4_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_arc4_sync_SOURCES=perf_arc4_sync.c
perf_arc4_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_arc4_async_SOURCES=perf_arc4_async.c
perf_arc4_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_arc4_sync_noPapi_SOURCES=perf_arc4_sync_noPapi.c
perf_arc4_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_arc4_async_noPapi_SOURCES=perf_arc4_async_noPapi.c
perf_md5_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_md5_sync_SOURCES=perf_md5_sync.c
perf_md5_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_md5_async_SOURCES=perf_md5_async.c
perf_md5_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_md5_sync_noPapi_SOURCES=perf_md5_sync_noPapi.c
perf_md5_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_md5_async_noPapi_SOURCES=perf_md5_async_noPapi.c
perf_kasumi_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_kasumi_sync_SOURCES=perf_kasumi_sync.c
perf_kasumi_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_kasumi_sync_noPapi_SOURCES=perf_kasumi_sync_noPapi.c
perf_sha_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_sha_sync_SOURCES=perf_sha_sync.c
perf_sha_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_sha_async_SOURCES=perf_sha_async.c
perf_sha_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_sha_sync_noPapi_SOURCES=perf_sha_sync_noPapi.c
perf_sha_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_sha_async_noPapi_SOURCES=perf_sha_async_noPapi.c
perf_asym_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_asym_sync_SOURCES=perf_asym_sync.c
perf_asym_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_asym_async_SOURCES=perf_asym_async.c
perf_asym_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_asym_sync_noPapi_SOURCES=perf_asym_sync_noPapi.c
perf_asym_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_asym_async_noPapi_SOURCES=perf_asym_async_noPapi.c
perf_combo_sync_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_combo_sync_SOURCES=perf_combo_sync.c
perf_combo_async_LDADD=../lib/libcop.la -lpthread -lcrypto -lpapi
perf_combo_async_SOURCES=perf_combo_async.c
perf_combo_sync_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_combo_sync_noPapi_SOURCES=perf_combo_sync_noPapi.c
perf_combo_async_noPapi_LDADD=../lib/libcop.la -lpthread -lcrypto 
perf_combo_async_noPapi_SOURCES=perf_combo_async_noPapi.c
endif

if OPENSSL_ON
asymtest002_LDADD=../lib/libcop.la -lcrypto -lpthread
asymtest002_SOURCES=asymtest002.c
asymtest003_LDADD=../lib/libcop.la -lcrypto -lpthread
asymtest003_SOURCES=asymtest003.c
asymtest006_LDADD=../lib/libcop.la -lcrypto -lpthread
asymtest006_SOURCES=asymtest006.c
ciphertest001_LDADD=../lib/libcop.la -lcrypto -lpthread
ciphertest001_SOURCES=ciphertest001.c
hashtest001_LDADD=../lib/libcop.la -lcrypto -lpthread
hashtest001_SOURCES=hashtest001.c
hashtest004_LDADD=../lib/libcop.la -lcrypto -lpthread
hashtest004_SOURCES=hashtest004.c         
hmactest001_LDADD=../lib/libcop.la -lcrypto -lpthread
hmactest001_SOURCES=hmactest001.c
endif

