/*
   Licensed Materials - Property of IBM
   Restricted Materials of IBM

   Copyright 2008,2009, 2010
   Chris J Arges
   IBM Corp, All Rights Reserved

   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _INCLUDE_LIBCOPL_CRYPTO_IMF_H
#define _INCLUDE_LIBCOPL_CRYPTO_IMF_H

/**
 * @ingroup CRYPTO
 * @defgroup IMF Integer Modular Asymmetric Cryptographic Functions.
 * @{
 */

/**
 * @brief CPB for 192 bit Integer Modular Functions
 **/
struct cop_asym_imf192_cpb {
	uint8_t _rsv[16];
	uint8_t A[24];
	uint8_t B[24];
	uint8_t N[24];
	uint8_t _pad0[8];
};

/**
 * @brief CPB for 256 bit Integer Modular Functions
 **/
struct cop_asym_imf256_cpb {
	uint8_t _rsv[16];
	uint8_t A[32];
	uint8_t B[32];
	uint8_t N[32];
};

/**
 * @brief CPB for 512 bit Integer Modular Functions
 **/
struct cop_asym_imf512_cpb {
	uint8_t _rsv[16];
	uint8_t A[64];
	uint8_t B[64];
	uint8_t N[64];
};

/**
 * @brief CPB for 1024 bit Integer Modular Functions
 **/
struct cop_asym_imf1024_cpb {
	uint8_t _rsv[16];
	uint8_t A[128];
	uint8_t B[128];
	uint8_t N[128];
};

/**
 * @brief CPB for 2048 bit Integer Modular Functions
 **/
struct cop_asym_imf2048_cpb {
	uint8_t _rsv[16];
	uint8_t A[256];
	uint8_t B[256];
	uint8_t N[256];
};

/**
 * @brief CPB for 4096 bit Integer Modular Functions
 **/
struct cop_asym_imf4096_cpb {
	uint8_t _rsv[16];
	uint8_t A[512];
	uint8_t B[512];
	uint8_t N[512];
};

/**
 * @brief 192 bit Integer Modular Functions
 **/
struct cop_asym_imf192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf192_cpb cpb;
};

/**
 * @brief 256 bit Integer Modular Functions
 **/
struct cop_asym_imf256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf256_cpb cpb;
};

/**
 * @brief 512 bit Integer Modular Functions
 **/
struct cop_asym_imf512_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf512_cpb cpb;
};

/**
 * @brief 1024 bit Integer Modular Functions
 **/
struct cop_asym_imf1024_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf1024_cpb cpb;
};

/**
 * @brief 2048 bit Integer Modular Functions
 **/
struct cop_asym_imf2048_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf2048_cpb cpb;
};

/**
 * @brief 4096 bit Integer Modular Functions
 **/
struct cop_asym_imf4096_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imf4096_cpb cpb;
};

/**
 * @brief CPB for 192 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri192_cpb {
	uint8_t _rsv[16];
	uint8_t A[24];
	uint8_t N[24];
};

/**
 * @brief CPB for 256 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri256_cpb {
	uint8_t _rsv[16];
	uint8_t A[32];
	uint8_t N[32];
};

/**
 * @brief CPB for 512 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri512_cpb {
	uint8_t _rsv[16];
	uint8_t A[64];
	uint8_t N[64];
};

/**
 * @brief CPB for 1024 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri1024_cpb {
	uint8_t _rsv[16];
	uint8_t A[128];
	uint8_t N[128];
};

/**
 * @brief CPB for 2048 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri2048_cpb {
	uint8_t _rsv[16];
	uint8_t A[256];
	uint8_t N[256];
};

/**
 * @brief CPB for 4096 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri4096_cpb {
	uint8_t _rsv[16];
	uint8_t A[512];
	uint8_t N[512];
};

/**
 * @brief 192 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri192_cpb cpb;
};

/**
 * @brief 256 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri256_cpb cpb;
};

/**
 * @brief 512 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri512_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri512_cpb cpb;
};

/**
 * @brief 1024 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri1024_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri1024_cpb cpb;
};

/**
 * @brief 2048 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri2048_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri2048_cpb cpb;
};

/**
 * @brief 4096 bit Integer Modular Reduction and Inverse
 **/
struct cop_asym_imri4096_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imri4096_cpb cpb;
};

/**
 * @brief CPB for 192 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt192_cpb {
	uint8_t _rsv[16];
	uint8_t A[24];
	uint8_t _pad0[16];
	uint8_t Dp[12];
	uint8_t Dq[12];
	uint8_t _pad1[16];
	uint8_t Np[12];
	uint8_t Nq[12];
	uint8_t _pad2[16];
	uint8_t U[12];
	uint8_t _pad3[12];
};

/**
 * @brief CPB for 256 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt256_cpb {
	uint8_t _rsv[16];
	uint8_t A[32];
	uint8_t _pad0[16];
	uint8_t Dp[16];
	uint8_t Dq[16];
	uint8_t _pad1[16];
	uint8_t Np[16];
	uint8_t Nq[16];
	uint8_t _pad2[16];
	uint8_t U[16];
};

/**
 * @brief CPB for 512 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt512_cpb {
	uint8_t _rsv[16];
	uint8_t A[64];
	uint8_t _pad0[16];
	uint8_t Dp[32];
	uint8_t Dq[32];
	uint8_t _pad1[16];
	uint8_t Np[32];
	uint8_t Nq[32];
	uint8_t _pad2[16];
	uint8_t U[32];
};

/**
 * @brief CPB for 1024 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt1024_cpb {
	uint8_t _rsv[16];
	uint8_t A[128];
	uint8_t _pad0[16];
	uint8_t Dp[64];
	uint8_t Dq[64];
	uint8_t _pad1[16];
	uint8_t Np[64];
	uint8_t Nq[64];
	uint8_t _pad2[16];
	uint8_t U[64];
};

/**
 * @brief CPB for 2048 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt2048_cpb {
	uint8_t _rsv[16];
	uint8_t A[256];
	uint8_t _pad0[16];
	uint8_t Dp[128];
	uint8_t Dq[128];
	uint8_t _pad1[16];
	uint8_t Np[128];
	uint8_t Nq[128];
	uint8_t _pad2[16];
	uint8_t U[128];
};

/**
 * @brief CPB for 4096 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt4096_cpb {
	uint8_t _rsv[16];
	uint8_t A[512];
	uint8_t _pad0[16];
	uint8_t Dp[256];
	uint8_t Dq[256];
	uint8_t _pad1[16];
	uint8_t Np[256];
	uint8_t Nq[256];
	uint8_t _pad2[16];
	uint8_t U[256];
};

/**
 * @brief 192 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt192_cpb cpb;
};

/**
 * @brief 256 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt256_cpb cpb;
};

/**
 * @brief 512 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt512_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt512_cpb cpb;
};

/**
 * @brief 1024 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt1024_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt1024_cpb cpb;
};

/**
 * @brief 2048 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt2048_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt2048_cpb cpb;
};

/**
 * @brief 4096 bit CRT Modular Exponentiation
 **/
struct cop_asym_imcrt4096_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_imcrt4096_cpb cpb;
};

/** @} IMF */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_IMF_H */
