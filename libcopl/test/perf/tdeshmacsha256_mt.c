#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop.h>
#include <libcopl/cop_util.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/tdes_hmac.h>

#include "./bma2libcopl.h"

typedef struct cop_sym_tdes_sha256_hmac_cxb perf_cxb;

#define SETUP_FUNC() \
	cop_tdes_sha256_hmac_eta_setup(&cxb[Rq].crb, ctx, input, input_sz, \
	output, output_sz, 1, COP_RPAD_NONE, 0x00);

#include "PMUCtrl.c"
#include "perfTest.c"

int main(int argc, char *argv[])
{
	return mt_testos(argc, argv);
}

