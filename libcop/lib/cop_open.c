#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <sys/types.h>
#include "libcopmanage.h"
#include "libcopl/cop_sys.h"

extern int gDD_version;
extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);

unsigned int COP_NUMBER_COPS=0;
unsigned int COP_NUMBER_PBICS=0;

struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];
struct cop_pbic_info cop_pbic_info_array[COP_NUMBER_KNOWN_PBICS];
void cop_init_cops(void)
{
	int i, rc;
	struct cop_ctx ctx;

	COP_NUMBER_COPS=0;

	for(i = 0; i < COP_NUMBER_KNOWN_COPS; i++) {
		// FIXME: for now lets skip XMLX since nobody is really using it --cja
		if (i == COP_XML) {
			cop_ct_info_array[i].ctx.fd = -1;
			continue;
		}

		cop_ct_info_array[i].ptrDevName = cop_dev_node[i];
		cop_ct_info_array[i].mt_id = cop_mt_devpbicid[i];

		rc = cop_open_bind_local(&ctx, i);
		if (rc < 0) {
			fprintf(stderr, "cop_open_bind_local failure %s\n",
					cop_ct_info_array[i].ptrDevName);
			cop_ct_info_array[i].ctx.fd = -1;
			continue;
		}

		if ((cop_ct_info_array[i].unit_fd = cop_open_unit(&ctx) < 0)) {
			fprintf(stderr, "cop_open_unit failure %s\n",
					cop_ct_info_array[i].ptrDevName);
			cop_close(&ctx);
			cop_ct_info_array[i].ctx.fd = -1;
			continue;
		}

		/* successful coprocessor setup */
		cop_ct_info_array[i].ctx = ctx;  /* structure copy */
		COP_NUMBER_COPS++;
	}

	/*
	 * Just use fd (ctx struct) already associated with these units
	 */
	cop_pbic_info_array[COP_CC_PBIC].ctx =
		cop_ct_info_array[COP_ASYNC_DATA_MOVER].ctx;
	cop_pbic_info_array[COP_RX_PBIC].ctx =
		cop_ct_info_array[COP_REGX].ctx;
}
