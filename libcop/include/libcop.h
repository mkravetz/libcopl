/*  

Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009
   Chris J Arges
   Everton Constantino
   Tom Gall
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/
#ifndef _LIB_POWERPC_COP_H
#define _LIB_POWERPC_COP_H

#include <stdint.h>
#include <string.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/asym.h>
#include <libcopl/crypto.h>
#include <libcopl/rnd.h>

#ifdef VERBOSE_ON
#include <stdio.h>
#endif

// Index
// 1) Low Level structures
// 2) debug foo
// 3) cop_dispatch
// 4) Coprocessor Management
// 5) ADM APIs
// 6) RND APIs
// 7) Comp/Decomp APIs
// 8) Regular Expressions
// 9) BigNumber
// 10) Asymetric Cryptography
// 11) Symmetric Cryptography

/******************************************************************************
 * 1) Low Level structures
 *
 * Low level structures now reside in libcopl
 */
enum cop_completion_method {
	COMPLETION_STORE = 0,
	ASYNC_INTERRUPT
};

/******************************************************************************
 * 2) Debugging Functions
 *
 * Define extra-verbose debugging functions.
 */

#include "cop_debug.h"

/******************************************************************************
 * 3) cop_dispatch
 *
 * Define icswx interface/
 */

int cop_dispatch(struct cop_crb *crb);
int cop_dispatch_and_wait(struct cop_crb *crb);

typedef unsigned short errorn;
#if 0
enum error_codes {
	EOK = 0,
	EINVALID_ALIGNMENT,
	EOPERAND_OVERLAP,
	EDATA_LENGTH,
	ETRANSLATION = 5,
	EPROTECTION,
	EUNKNOWN,
	EBN_INVALID_DST_SIZE = 300,
	EBN_INVALID_POINTER_TO_FREE,
	EBN_TOO_BIG,
	EAC_INVALID_ARGUMENT_SIZE = 400,
	EAC_INVALID_ARGUMENT,
	ECD_INVALID_ARGUMENT = 500
};
#endif

/******************************************************************************
 * 4) Coprocessor Management
 */

/* check for existence of coprocessor on hardware
 */
int cop_exists(int cop);
int cop_get_status(int cop_number);

/* global variable that indicates DD chip revision number
 * DD1 = 1, DD2 = 2 */
extern int gDD_version;
int cop_get_DD_version();

/* session state */
typedef enum {
	COP_SESSION_INVALID,
	COP_SESSION_INIT,
	COP_SESSION_INIT_COMPLETE,
	COP_SESSION_INFLIGHT,	// icswx issued
	COP_SESSION_INFLIGHT_NO_RETURN,	// icswx issues, as a 1st or n-1
	COP_SESSION_COMPLETE,
} cop_state_t;

/*
 * struct cop_session - holds data for a coprocessor session
 * @ state - current state of coprocessor session
 * @ crb - pointer to crb of session
 * @ data - point to coprocessor specific session data
 */
typedef struct cop_session {
	cop_state_t state;
	struct cop_crb *ptrPVT;
	void *data;
} cop_session_t;

cop_session_t *cop_create_session();
cop_state_t cop_get_session_state(cop_session_t * session);
void cop_set_session_state(cop_session_t * session, cop_state_t newstate);
size_t cop_get_tgt_size(cop_session_t * session);
size_t cop_get_src_size(cop_session_t * session);
size_t cop_get_target_output_size(cop_session_t * session);
size_t cop_get_src_processed(cop_session_t * session);
cop_session_t *cop_commit_session(cop_session_t * session);
cop_session_t *cop_commit_session_async(cop_session_t * session);
int cop_session_iscomplete(cop_session_t * session);
void cop_free_session(cop_session_t * session);

/******************************************************************************
 * 5) ADM APIs
 */
void *cop_memcpy(void *dest, void *src, size_t n);
void *cop_memmove(void *dest, void *src, size_t n);
void *cop_memset(void *s, int c, size_t n);

/******************************************************************************
 * 6) RND APIs
 */

int cop_rand();
long cop_random();
cop_session_t *cop_create_random_session(int mode);
long cop_get_rnd(cop_session_t * session);

/******************************************************************************
 * 7) Comp/Decomp APIs
 */

typedef struct cop_comp_dde_list_s {
	size_t size;
	void *target_buffer;
	struct cop_comp_dde_list_s *next, *prev;
} cop_comp_dde_list_t;

typedef struct cop_comp_session_s {
	cop_comp_dde_list_t *root, *last;
	size_t dde_list_size;
} cop_comp_session_t;

int cop_get_comp_type(cop_session_t * session);

cop_session_t *cop_create_empty_comp_session();

size_t cop_comp_get_dde_list_size(cop_session_t * session);

size_t cop_comp_copy_dde_list_with_offset(cop_session_t * session, void *buf,
	size_t size, size_t offset);

void cop_comp_copy_dde_list(cop_session_t * session, void *buf, size_t size);

cop_session_t *cop_gunzip_add_dde(cop_session_t * session, size_t tgt_size);

void cop_free_comp_session(cop_session_t * s);

cop_session_t *cop_reset_comp_session(cop_session_t * session, int alg,
	int mode, int nmode,
	size_t transaction_size, size_t src_size,
	void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_prep_next_decompression(cop_session_t * session, int last,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_create_gzip_session(int notificationmode,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_create_gunzip_session_with_dde_list(int notificationmode,
	int multiflag, size_t src_size, void *ptrSrc, size_t tgt_size);

cop_session_t *cop_create_gunzip_session(int notificationmode, int multiflag,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_gunzip_add_src_buffer(cop_session_t * session,
	size_t src_size, void *ptrSrc);

cop_session_t *cop_gunzip_add_tgt_buffer(cop_session_t * session,
	size_t tgt_size, void *ptrTgt);

cop_session_t *cop_create_zlib_compression_session(int notificationmode,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_create_zlib_decompression_session(int notificationmode,
	int multiflag, size_t src_size, void *ptrSrc, size_t tgt_size,
	void *ptrTgt);

cop_session_t *cop_zlib_decompressiion_add_src_buffer(cop_session_t * session,
	size_t src_size, void *ptrSrc);

cop_session_t *cop_zlib_decompression_add_tgt_buffer(cop_session_t * session,
	size_t tgt_size, void *ptrTgt);

cop_session_t *cop_zlib_decompression_fix_exception(cop_session_t * session);

cop_session_t *cop_create_deflate_session(int notificationmode,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_create_inflate_session(int notificationmode, int multiflag,
	size_t src_size, void *ptrSrc, size_t tgt_size, void *ptrTgt);

cop_session_t *cop_inflate_add_src_buffer(cop_session_t * session,
	size_t src_size, void *ptrSrc);

cop_session_t *cop_inflate_add_tgt_buffer(cop_session_t * session,
	size_t tgt_size, void *ptrTgt);

cop_session_t *cop_psr2_gzip_compression(cop_session_t * session, char *tgt,
	int *size);

/******************************************************************************
 * 8) Regular Expressions
 */

#include "libcopl/regx.h"

#define REGX_MAX_SEARCHES_PER_SESSION 4

/**
 * @brief holds data for a single regx search, 
 *
 * To gather patterns from a search, one needs to track the pcid, 
 * and search command number.
 */
typedef struct cop_regx_search {
	uint8_t results_total; /**< @brief result entries from a search */
	struct cop_regx_results *results; /** <@ total number of results */
	struct cop_regx_sc cmd[2]; /** <@ command(s) for a particular search */
	int status; /** <@ status of a search */
} cop_regx_search_t;

/*
 * @brief holds any stateful information about a particular search
 */
typedef struct cop_regx_session {
	/** @brief search command with corresponding results */
	cop_regx_search_t search[REGX_MAX_SEARCHES_PER_SESSION];
	int search_count; /**< @brief the amount of searches used */
	int results_count; /**< @brief the total number of results for all searches */
	void * um_shared; /**< @brief regx shared object from UM (should be type regex_shared_t )*/
} cop_regx_session_t;

cop_session_t *cop_regx_create_session();

void cop_regx_free_session(cop_session_t * session);
void cop_regx_reset_session(cop_session_t * session);

int cop_regx_pcs_load(cop_session_t * session, char *pcs_file);
void cop_regx_search(cop_session_t * session, uint32_t ctx, const void *source,
	size_t source_size);
int cop_regx_search_ex(cop_session_t * session, uint32_t ctx_id1,
	uint32_t ctx_id2, const void *source, size_t source_size, int start1,
	int stop1, int start2, int stop2);
int cop_regx_get_results(cop_session_t * session, int **offset_array,
	int **pattern_array, int **scommand_array);

void cop_regx_init();
void cop_regx_close();

/******************************************************************************
 * 9) BigNumber API
 *
 * BigNumber is a data type for use with the asymmetric crypto API. It defines
 * a pointer to data and the size of data.
 */
#if 0
typedef struct {
	size_t size;
	uint8_t *number;
} BigNumber;
#endif

BigNumber *cop_bn_new(uint8_t * number, size_t size);
BigNumber *cop_bn_new_int(int number);
BigNumber *cop_bn_new_sz(size_t size);
void cop_bn_copy(BigNumber * dest, const BigNumber * src);
int cop_bn_is_zero(BigNumber * n);
void cop_bn_print(BigNumber * n);
errorn cop_bn_free(BigNumber * bn);

/******************************************************************************
 * 10) Asymmetric Cryptography
 *
 * Defines an API for modular integer functions as well as point functions 
 * using the Crypto coprocessor.
 */

BigNumber *cop_madd(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_msub(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_mred(const BigNumber * a, const BigNumber * n);
BigNumber *cop_mex(const BigNumber * a, const BigNumber * e,
	const BigNumber * n);
BigNumber *cop_mmul(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_mgmul(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_minv(const BigNumber * a, const BigNumber * n);
BigNumber *cop_mexc(const BigNumber * a, const BigNumber * dp,
	const BigNumber * dq, const BigNumber * np,
	const BigNumber * nq, const BigNumber * u);
BigNumber *cop_m2add(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_m2red(const BigNumber * a, const BigNumber * n);
BigNumber *cop_m2ex(const BigNumber * a, const BigNumber * e,
	const BigNumber * n);
BigNumber *cop_m2mul(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_mg2mul(const BigNumber * a, const BigNumber * b,
	const BigNumber * n);
BigNumber *cop_m2mi(const BigNumber * a, const BigNumber * n);

BigNumber *cop_madd_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_msub_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_mred_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * n);
BigNumber *cop_mex_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * e, const BigNumber * n);
BigNumber *cop_mmul_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_mgmul_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_minv_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * n);
BigNumber *cop_mexc_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * dp, const BigNumber * dq,
	const BigNumber * np, const BigNumber * nq, const BigNumber * u);
BigNumber *cop_m2add_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_m2red_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * n);
BigNumber *cop_m2ex_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * e, const BigNumber * n);
BigNumber *cop_m2mul_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_mg2mul_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * b, const BigNumber * n);
BigNumber *cop_m2mi_async(cop_session_t * session, const BigNumber * a,
	const BigNumber * n);

/******************************************************************************
 * 11) Symmetric Cryptography
 *
 * Defines an API for symmetric encryption and hashing functions.
 */

/* hash functions */
int cop_md5(const void *source, size_t length, void *digest,
	uint64_t mbl, uint8_t flag);

int cop_sha1(const void *source, size_t length, void *digest,
	uint64_t mbl, uint8_t flag);

int cop_sha256(const void *source, size_t length, void *digest,
	uint64_t mbl, uint8_t flag);

int cop_sha512(const void *source, size_t length, void *digest,
	uint64_t mbl, uint8_t flag);

/* sha hmac functions */
int cop_sha1_hmac(const void *source, size_t length, void *digest,
	uint64_t mbl, void *key, uint8_t flag);

int cop_sha256_hmac(const void *source, size_t length, void *digest,
	uint64_t mbl, void *key, uint8_t flag);

int cop_sha512_hmac(const void *source, size_t length, void *digest,
	uint64_t mbl, void *key, uint8_t flag);

/* arc4 functions */
int cop_rc4_40(const void *key, size_t length, const void *indata,
	void *outdata, void *state, uint8_t * x_val, uint8_t * y_val,
	uint8_t flag);

int cop_rc4_64(const void *key, size_t length, const void *indata,
	void *outdata, void *state, uint8_t * x_val, uint8_t * y_val,
	uint8_t flag);

int cop_rc4_128(const void *key, size_t length, const void *indata,
	void *outdata, void *state, uint8_t * x_val, uint8_t * y_val,
	uint8_t flag);

/* des functions */
int cop_des_ecb(const void *input, size_t input_sz, void *output,
	void *key_schedule, uint8_t enc, uint8_t pad_rules, uint8_t pad_byte);

int cop_tdes_ecb(const void *input, size_t input_sz, void *output,
	void *key_schedule, uint8_t enc, uint8_t pad_rules, uint8_t pad_byte);

int cop_des_cbc(const void *input, size_t input_sz, void *output,
	void *key_schedule, void *initial_value, uint8_t enc,
	uint8_t pad_rules, uint8_t pad_bytes);

int cop_tdes_cbc(const void *input, size_t input_sz, void *output,
	void *key_schedule, void *initial_value, uint8_t enc,
	uint8_t pad_rules, uint8_t pad_bytes);

/* aes functions */
int cop_aes_ecb_128(const void *input, size_t input_sz, void *output,
	void *key_schedule, uint8_t enc, uint8_t pad_rules, uint8_t pad_byte);

int cop_aes_ecb_192(const void *input, size_t input_sz, void *output,
	void *key_schedule, uint8_t enc, uint8_t pad_rules, uint8_t pad_byte);

int cop_aes_ecb_256(const void *input, size_t input_sz, void *output,
	void *key_schedule, uint8_t enc, uint8_t pad_rules, uint8_t pad_byte);

int cop_aes_cbc_128(const void *input, size_t input_sz, void *output,
	void *key_schedule, void *initial_value, uint8_t enc,
	uint8_t pad_rules, uint8_t pad_byte);

int cop_aes_cbc_192(const void *input, size_t input_sz, void *output,
	void *key_schedule, void *initial_value, uint8_t enc,
	uint8_t pad_rules, uint8_t pad_byte);

int cop_aes_cbc_256(const void *input, size_t input_sz, void *output,
	void *key_schedule, void *initial_value, uint8_t enc,
	uint8_t pad_rules, uint8_t pad_byte);

/* kasumi functions */
int cop_kasumi_f8(const void *source, size_t length,
	void *dest,
	void *iv, void *key, void *bc_in, void *ks_in,
	void *a, void *bc_out, void *ks_out, uint8_t flag);

int cop_kasumi_f9(const void *source, size_t length,
	void *dest,
	void *iv, void *pad, void *key, void *b_in,
	void *mac, void *b_out, uint8_t flag);

#endif				/* _POWERPC_COP_LIB_H */
