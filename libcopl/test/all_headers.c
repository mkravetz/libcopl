/*
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Mike kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp
*/

#include <stdio.h>

#include <libcopl/adm.h>
#include <libcopl/asym.h>
#include <libcopl/comp.h>
#include <libcopl/comp/deflate.h>
#include <libcopl/comp/gzip.h>
#include <libcopl/comp/zlib.h>
#include <libcopl/cop_err.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>
#include <libcopl/crypto.h>
#include <libcopl/crypto/aes.h>
#include <libcopl/crypto/aes_hmac.h>
#include <libcopl/crypto/des.h>
#include <libcopl/crypto/hash_ex.h>
#include <libcopl/crypto/hash.h>
#include <libcopl/crypto/hmac.h>
#include <libcopl/crypto/imf.h>
#include <libcopl/crypto/ecc.h>
#include <libcopl/crypto/kasumi.h>
#include <libcopl/crypto/rc4.h>
#include <libcopl/crypto/tdes_hmac.h>
#include <libcopl/icswx.h>
#include <libcopl/icswx_util.h>
#include <libcopl/regx.h>
#include <libcopl/rnd.h>
#include <libcopl/waitm.h>

/*
 * The primary purpose of this 'test' is to include all the libcopl
 * header files.  The complier options check for all warnings and
 * and will error out if there are any (-Wall -Werror).
 */
int main(int argc, char *argv[])
{
	printf("=== %s: 0/1 failures ===\n", argv[0]);

	return 0;
}
