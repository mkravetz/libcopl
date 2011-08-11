#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop.h>
#include <libcopl/cop_util.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/hash.h>

#include "./bma2libcopl.h"

typedef struct cop_sym_md5_cxb perf_cxb;

#define SETUP_FUNC() \
	cop_md5_setup(&cxb[Rq].crb, ctx, input, input_sz, COP_FLAG_ONESHOT);

#include "PMUCtrl.c"
#include "perfTest.c"

int main(int argc, char *argv[])
{
	return mt_testos(argc, argv);
}

