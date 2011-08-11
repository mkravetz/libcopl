/*
 *
 * Licensed Materials - Property of IBM
 * Restricted Materials of IBM
 * Copyright 2010
 * Chris J Arges
 * Everton Constantino
 * Tom Gall
 * IBM Corp, All Rights Reserved
 *                    
 * US Government Users Restricted Rights - Use Duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM
 * Corp
 *
 **/

#include <byteswap.h>

#include "libcopmanage.h"
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcop.h"
#include "config.h"
#include "libcopl/cop_util.h"
#include "libcopl/comp/gzip.h"
#include "libcopl/comp/zlib.h"
#include "libcopl/comp/deflate.h"
#include "libcopl/cop_util.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];
extern cop_session_t *cop_create_session();

#define CHECK_INPUT(s) (s && s->src_dde.data && s->tgt_dde.data && s->src_dde.bytes>=0 && s->tgt_dde.bytes>=0)
#define CHECK_INPUT2(s) (s && s->src_dde.data && s->src_dde.bytes>=0 )

#define SET_RESUME(crb) ((crb)->flags |= COP_RESUME )
#define UNSET_RESUME(crb) ((crb)->flags &= (0xFF ^ COP_RESUME) )

#define SET_DDE(crb) ((crb)->flags |= COP_DDE_PRESENT);
#define UNSET_DDE(crb) ((crb)->flags &= (0xFF ^ COP_DDE_PRESENT))

int cop_get_comp_type(cop_session_t * session)
{
	return (((struct cop_crb *) session->ptrPVT)->flags);
}

cop_session_t *cop_create_empty_comp_session()
{
	cop_session_t *session = malloc(sizeof(cop_session_t));
	session->state = COP_SESSION_INIT;
	session->ptrPVT = NULL;
	session->data = malloc( sizeof( cop_comp_session_t ));
	cop_comp_session_t *t = (cop_comp_session_t *)session->data;
	t->root = NULL;
	t->last = NULL;
	t->dde_list_size = 0;

	return session;
}

void cop_free_comp_session( cop_session_t *s )
{
	if( s->data ){
		cop_comp_session_t *cdc_session = (cop_comp_session_t *) s->data;
		cop_comp_dde_list_t *t = cdc_session->root;

		while( t != NULL ){
			cop_comp_dde_list_t *freeme = t;
			
			cop_free( cop_cc_pool(), t->target_buffer );
			t = t->next;

			free( freeme );
		}
	}

	cop_free_session(s);
}

// PUNT
cop_session_t *cop_reset_comp_session(cop_session_t * session, int alg,
				      int mode, int nmode,
				      size_t transaction_size, size_t src_size,
				      void *ptrSrc, size_t tgt_size,
				      void *ptrTgt)
{
	struct cop_crb *cmd_crb;

	if (!session->ptrPVT) {
		mapped_memory_pool_t cc_pool = cop_cc_pool();

		cmd_crb = cop_next_crb(cc_pool, COP_DECOMP);
		session->ptrPVT = cmd_crb;
	} else
		cmd_crb = session->ptrPVT;

	cmd_crb->seq_num = 0;
	cmd_crb->src_dde.bytes = src_size;
	cop_set_sdptr(cmd_crb, ptrSrc);
	cmd_crb->tgt_dde.bytes = tgt_size;
	cop_set_tdptr(cmd_crb, ptrTgt);

	cop_crb_set_ccw_fc(cmd_crb, &cop_ct_info_array[COP_DECOMP].ctx, mode);
	cmd_crb->flags |= alg | COP_DDE_PRESENT;

	if (nmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(cmd_crb, ASYNC_INTERRUPT);

	cmd_crb->ch = COP_CHAINING_ONLY;
	((struct cop_csb *) cop_get_csbaddr(cmd_crb))->cw.ch = COP_CHAINING_ONLY;

	if (!CHECK_INPUT2(session->ptrPVT)) {
		mapped_memory_pool_t cc_pool = cop_cc_pool();
		cop_give_back_crb(cc_pool, session->ptrPVT, COP_DECOMP);
		session->ptrPVT = NULL;
		cop_set_session_state(session, COP_SESSION_INVALID);
		return NULL;
	}
	cop_set_session_state(session, COP_SESSION_INIT);
	return session;
}

cop_session_t *cop_prep_next_decompression(cop_session_t * session, int last,
					   size_t src_size, void *ptrSrc,
					   size_t tgt_size, void *ptrTgt)
{
	struct cop_crb *crb = session->ptrPVT;

	if (last)
		cop_decomp_setup_ml(crb, ptrSrc, src_size, ptrTgt, tgt_size,
				      COP_CHAINING_LAST);
	else
		cop_decomp_setup_ml(crb, ptrSrc, src_size, ptrTgt, tgt_size,
				      COP_CHAINING_MIDDLE);

    cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
    return session;
}

// mode sync or async, or async w/notification via interrupt
// sizes and pointers pretty obvious
cop_session_t *cop_create_gzip_session(int notificationmode, size_t src_size,
				       void *ptrSrc, size_t tgt_size,
				       void *ptrTgt)
{
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	/*
	 * set control block pointers
	 */
	crb = cop_next_crb(cc_pool, COP_DECOMP);

	if (cop_gzip_setup(crb, &cop_ct_info_array[COP_DECOMP].ctx,
					ptrSrc, src_size, ptrTgt, tgt_size)) {
		cop_give_back_crb(cc_pool, session->ptrPVT, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	session->ptrPVT = crb;
	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

cop_session_t *cop_create_gunzip_session_with_dde_list(int notificationmode, int multiflag,
														size_t src_size, void *ptrSrc,
														size_t tgt_size )
{
	void *tgt_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );

	cop_session_t *session = cop_create_gunzip_session( notificationmode, multiflag, src_size, ptrSrc, tgt_size, tgt_buffer );

	if( !session ){
		cop_free(cop_cc_pool(), tgt_buffer );
		return NULL;
	}
	
	session->data = malloc( sizeof( cop_comp_session_t ) );
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cdc_session->root = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );
	cdc_session->last = cdc_session->root;
	
	cdc_session->root->size = tgt_size;
	cdc_session->root->target_buffer = tgt_buffer;
	cdc_session->root->prev = NULL;
	cdc_session->root->next = NULL;

	cdc_session->dde_list_size = tgt_size;

	return session;
}

cop_session_t *cop_create_gunzip_session(int notificationmode, int multiflag,
					 size_t src_size, void *ptrSrc,
					 size_t tgt_size, void *ptrTgt)
{
	int rc;
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	/*
	 * set control block pointers
	 */
	crb = cop_next_crb(cc_pool, COP_DECOMP);
	
	if (multiflag)
		rc =  cop_gunzip_setup_first(crb,
					&cop_ct_info_array[COP_DECOMP].ctx,
					ptrSrc, src_size, ptrTgt, tgt_size);
	else
		rc =  cop_gunzip_setup_only(crb,
					&cop_ct_info_array[COP_DECOMP].ctx,
					ptrSrc, src_size, ptrTgt, tgt_size);
	if (rc) {
		cop_give_back_crb(cc_pool, crb, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	session->ptrPVT = crb;
	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

cop_session_t *cop_gunzip_add_src_buffer(cop_session_t * session,
					 size_t src_size, void *ptrSrc)
{
	return session;
}

size_t cop_comp_get_dde_list_size( cop_session_t *session )
{
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	return cdc_session->dde_list_size;
}

size_t cop_comp_copy_dde_list_with_offset( cop_session_t *session, void *buf, size_t size, size_t offset )
{
	size_t all_size = cop_comp_get_dde_list_size( session );
	void *tmp = malloc( all_size );

	cop_comp_copy_dde_list( session, tmp, all_size );

	memcpy( buf, tmp + offset, size );

	free(tmp);

	return size;
}

void cop_comp_copy_dde_list( cop_session_t *session, void *buf, size_t size ){
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cop_comp_dde_list_t *t = cdc_session->root;
	size_t offset = 0;

	while( t != NULL ){
		size_t s = (offset + t->size) > size ? (size - offset) : t->size;
		memcpy( buf + offset, t->target_buffer, s );

		offset += s;

		t = t->next;
	}
}

cop_session_t *cop_gunzip_add_dde( cop_session_t *session, size_t tgt_size )
{
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cop_comp_dde_list_t *t = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );

	t->target_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );
	t->size = tgt_size;
	t->next = NULL;
	t->prev = cdc_session->last;

	if( cdc_session->last ){
		cdc_session->last->next = t;
	}

	cdc_session->last = t;
	cdc_session->dde_list_size += tgt_size;

	memset( t->target_buffer, 0x0, t->size );
	
	return cop_gunzip_add_tgt_buffer( session, t->size, t->target_buffer );
}

cop_session_t *cop_gunzip_add_tgt_buffer(cop_session_t * session,
					 size_t tgt_size, void *ptrTgt)
{
	struct cop_crb *crb = session->ptrPVT;

	(void)cop_gunzip_addtgt_rsm_setup(crb, ptrTgt, tgt_size);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);

	return session;
}

// mode sync or async, or async w/notification via interrupt
// transaction_size (how big of chunks to send things off as to icswx)  IE 64K at a time, 128K at a time etc
// sizes and pointers pretty obvious
cop_session_t *cop_create_zlib_compression_session(int notificationmode, 
						   size_t src_size,
						   void *ptrSrc,
						   size_t tgt_size,
						   void *ptrTgt)
{
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_DECOMP);
	session->ptrPVT = crb;

	if (cop_zlib_comp_setup(crb, &cop_ct_info_array[COP_DECOMP].ctx,
				ptrSrc, src_size, ptrTgt, tgt_size)) {
		cop_give_back_crb(cc_pool, session->ptrPVT, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

cop_session_t *cop_zlib_decompression_add_dde( cop_session_t *session, size_t tgt_size )
{
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cop_comp_dde_list_t *t = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );

	t->target_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );
	t->size = tgt_size;
	t->next = NULL;
	t->prev = cdc_session->last;

	if( cdc_session->last ){
		cdc_session->last->next = t;
	}

	cdc_session->last = t;
	cdc_session->dde_list_size += tgt_size;

	memset( t->target_buffer, 0x0, t->size );
	
	return cop_zlib_decompression_add_tgt_buffer( session, t->size, t->target_buffer );
}

cop_session_t *cop_create_zlib_decompression_session_with_dde_list(int notificationmode, int multiflag,
														size_t src_size, void *ptrSrc,
														size_t tgt_size )
{
	void *tgt_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );

	cop_session_t *session = cop_create_zlib_decompression_session( notificationmode, multiflag, src_size, ptrSrc, tgt_size, tgt_buffer );

	if( !session ){
		cop_free(cop_cc_pool(), tgt_buffer );
		return NULL;
	}
	
	session->data = malloc( sizeof( cop_comp_session_t ) );
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cdc_session->root = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );
	cdc_session->last = cdc_session->root;
	
	cdc_session->root->size = tgt_size;
	cdc_session->root->target_buffer = tgt_buffer;
	cdc_session->root->prev = NULL;
	cdc_session->root->next = NULL;

	cdc_session->dde_list_size = tgt_size;

	return session;
}

cop_session_t *cop_create_zlib_decompression_session(int notificationmode, int multiflag,
						     size_t src_size,
						     void *ptrSrc,
						     size_t tgt_size,
						     void *ptrTgt)
{
	int rc;
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();	//malloc(sizeof (cop_session_t));
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_DECOMP);
	session->ptrPVT = crb;

	if (multiflag)
		rc = cop_zlib_decomp_setup_first(crb,
				&cop_ct_info_array[COP_DECOMP].ctx,
				ptrSrc, src_size, ptrTgt, tgt_size);
	else
		rc = cop_zlib_decomp_setup_only(crb,
				&cop_ct_info_array[COP_DECOMP].ctx,
				ptrSrc, src_size, ptrTgt, tgt_size);

	if (rc) {
		cop_give_back_crb(cc_pool, session->ptrPVT, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

cop_session_t *cop_zlib_decompression_add_src_buffer(cop_session_t * session,
						     size_t src_size,
						     void *ptrSrc)
{
	return session;
}

cop_session_t *cop_zlib_decompression_add_tgt_buffer(cop_session_t * session,
						     size_t tgt_size,
						     void *ptrTgt)
{
	struct cop_crb *crb = session->ptrPVT;

	(void)cop_zlib_decomp_addtgt_rsm_setup(crb, ptrTgt, tgt_size);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

// deflate == compress
cop_session_t *cop_create_deflate_session(int notificationmode, size_t src_size,
					  void *ptrSrc, size_t tgt_size,
					  void *ptrTgt)
{
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();	//malloc(sizeof (cop_session_t));
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_DECOMP);
	session->ptrPVT = crb;

	if (cop_deflate_setup(crb, &cop_ct_info_array[COP_DECOMP].ctx,
				ptrSrc, src_size, ptrTgt, tgt_size)) {
		cop_give_back_crb(cc_pool, session->ptrPVT, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

// inflate == decompress
cop_session_t *cop_create_inflate_session(int notificationmode, int multiflag,
					  size_t src_size, void *ptrSrc,
					  size_t tgt_size, void *ptrTgt)
{
	int rc;
	struct cop_crb *crb;
	cop_session_t *session = cop_create_session();
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_DECOMP);

	if (multiflag)
		rc = cop_inflate_setup_first(crb,
					&cop_ct_info_array[COP_DECOMP].ctx,
					ptrSrc, src_size, ptrTgt, tgt_size);
	else
		rc = cop_inflate_setup_only(crb,
					&cop_ct_info_array[COP_DECOMP].ctx,
					ptrSrc, src_size, ptrTgt, tgt_size);

	if (rc) {
		cop_give_back_crb(cc_pool, crb, COP_DECOMP);
		free(session);
		return NULL;
	}

	if (notificationmode == ASYNC_INTERRUPT)
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);

	session->ptrPVT = crb;
	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);
	return session;
}

cop_session_t *cop_inflate_add_src_buffer(cop_session_t * session,
					  size_t src_size, void *ptrSrc)
{
	return session;
}

cop_session_t *cop_create_inflate_session_with_dde_list(int notificationmode, int multiflag,
														size_t src_size, void *ptrSrc,
														size_t tgt_size )
{
	void *tgt_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );

	cop_session_t *session = cop_create_inflate_session( notificationmode, multiflag, src_size, ptrSrc, tgt_size, tgt_buffer );

	if( !session ){
		cop_free(cop_cc_pool(), tgt_buffer );
		return NULL;
	}
	
	session->data = malloc( sizeof( cop_comp_session_t ) );
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cdc_session->root = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );
	cdc_session->last = cdc_session->root;
	
	cdc_session->root->size = tgt_size;
	cdc_session->root->target_buffer = tgt_buffer;
	cdc_session->root->prev = NULL;
	cdc_session->root->next = NULL;

	cdc_session->dde_list_size = tgt_size;

	return session;
}

cop_session_t *cop_inflate_add_tgt_buffer(cop_session_t * session,
					  size_t tgt_size, void *ptrTgt)
{
	struct cop_crb *crb = session->ptrPVT;
	
	(void)cop_inflate_addtgt_rsm_setup(crb, ptrTgt, tgt_size);

	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);

	return session;
}

cop_session_t *cop_inflate_add_dde( cop_session_t *session, size_t tgt_size )
{
	cop_comp_session_t *cdc_session = (cop_comp_session_t *) session->data;
	cop_comp_dde_list_t *t = (cop_comp_dde_list_t *) malloc( sizeof( cop_comp_dde_list_t ) );

	t->target_buffer = cop_malloc( cop_cc_pool(), tgt_size, 64 );
	t->size = tgt_size;
	t->next = NULL;
	t->prev = cdc_session->last;

	if( cdc_session->last ){
		cdc_session->last->next = t;
	}

	cdc_session->last = t;
	cdc_session->dde_list_size += tgt_size;

	memset( t->target_buffer, 0x0, t->size );
	
	return cop_inflate_add_tgt_buffer( session, t->size, t->target_buffer );
}

void swap_endian(char *buffer, int start, int end)
{
	int i = start;
	int j = end;

	while (i < j) {
		char c = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = c;
		i++;
		j--;
	}
}

cop_session_t *cop_psr2_gzip_compression(cop_session_t * session, char *tgt,
					 int *size)
{
	size_t sz = 0;

	verbose_print("Using Errata 292 workaround\n");

	session = cop_commit_session(session);

	if (cop_session_iscomplete(session)) {
		sz = cop_get_target_output_size(session);
	}

	verbose_print("size=%d\n", sz);

//      sz=bswap_32(sz);

	swap_endian(tgt, sz - 8, sz - 5);
	swap_endian(tgt, sz - 4, sz - 1);

	// fix up session too
//      cop_fixup_target_output_size(session,sz);

	*size = sz;

	return session;
}
