#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop.h>
#include <libcopl/cop_util.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/kasumi.h>

#include "./bma2libcopl.h"

typedef struct cop_sym_kasumi_f9_cxb perf_cxb;

#define SETUP_FUNC() \
	cop_kasumi_f9_setup(&cxb[Rq].crb, ctx, input, input_sz, COP_FLAG_ONESHOT);

#include "PMUCtrl.c"
#include "perfTest.c"

int main(int argc, char *argv[])
{
	return mt_testos(argc, argv);
}

