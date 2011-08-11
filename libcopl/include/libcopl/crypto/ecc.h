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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_ECC_H
#define _INCLUDE_LIBCOPL_CRYPTO_ECC_H

/**
 * @ingroup CRYPTO
 * @defgroup ECC Eliptical Curve Cryptographic Functions.
 * @{
 */

/**
 * @brief CPB for 192 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd192_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[24];
	uint8_t Yp[24];
	uint8_t Xq[24];
	uint8_t Yq[24];
	uint8_t N[24];
	uint8_t _pad0[8];
};

/**
 * @brief CPB for 256 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd256_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[32];
	uint8_t Yp[32];
	uint8_t Xq[32];
	uint8_t Yq[32];
	uint8_t N[32];
};

/**
 * @brief CPB for 521 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd521_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[14];
	uint8_t Xp[66];
	uint8_t _pad1[14];
	uint8_t Yp[66];
	uint8_t _pad2[14];
	uint8_t Xq[66];
	uint8_t _pad3[14];
	uint8_t Yq[66];
	uint8_t _pad4[14];
	uint8_t N[66];
};

/**
 * @brief 192 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpadd192_cpb cpb;
};

/**
 * @brief 256 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpadd256_cpb cpb;
};

/**
 * @brief 521 bit ECC GF(p) Point Functions
 **/
struct cop_asym_eccpadd521_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpadd521_cpb cpb;
};

/**
 * @brief CPB for 192 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl192_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[24];
	uint8_t Yp[24];
	uint8_t N[24];
	uint8_t _pad0[8];
};

/**
 * @brief CPB for 256 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl256_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[32];
	uint8_t Yp[32];
	uint8_t N[32];
};

/**
 * @brief CPB for 521 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl521_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[14];
	uint8_t Xp[66];
	uint8_t _pad1[14];
	uint8_t Yp[66];
	uint8_t _pad2[14];
	uint8_t N[66];
};

/**
 * @brief 192 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpdbl192_cpb cpb;
};

/**
 * @brief 256 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpdbl256_cpb cpb;
};

/**
 * @brief 521 bit ECC GF(p) Point Doubling Function
 **/
struct cop_asym_eccpdbl521_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpdbl521_cpb cpb;
};

/**
 * @brief CPB for 192 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul192_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[24];
	uint8_t Yp[24];
	uint8_t N[24];
	uint8_t k[24];
};

/**
 * @brief CPB for 256 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul256_cpb {
	uint8_t _rsv[16];
	uint8_t Xp[32];
	uint8_t Yp[32];
	uint8_t N[32];
	uint8_t k[32];
};

/**
 * @brief CPB for 521 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul521_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[14];
	uint8_t Xp[66];
	uint8_t _pad1[14];
	uint8_t Yp[66];
	uint8_t _pad2[14];
	uint8_t N[66];
	uint8_t _pad3[14];
	uint8_t k[66];
};

/**
 * @brief 192 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul192_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpmul192_cpb cpb;
};

/**
 * @brief 256 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul256_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpmul256_cpb cpb;
};

/**
 * @brief 521 bit ECC GF(p) Point Multiplication Function
 **/
struct cop_asym_eccpmul521_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccpmul521_cpb cpb;
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame163_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[11];
	uint8_t A[21];
	uint8_t _pad1[11];
	uint8_t B[21];
	uint8_t _pad2[11];
	uint8_t N[21];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame233_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[2];
	uint8_t A[30];
	uint8_t _pad1[2];
	uint8_t B[30];
	uint8_t _pad2[2];
	uint8_t N[30];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame283_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t A[36];
	uint8_t _pad1[12];
	uint8_t B[36];
	uint8_t _pad2[12];
	uint8_t N[36];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame409_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t A[52];
	uint8_t _pad1[12];
	uint8_t B[52];
	uint8_t _pad2[12];
	uint8_t N[52];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame571_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[8];
	uint8_t A[72];
	uint8_t _pad1[8];
	uint8_t B[72];
	uint8_t _pad2[8];
	uint8_t N[72];
};

/**
 * @brief 163 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame163_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ame163_cpb cpb;
};

/**
 * @brief 233 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame233_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ame233_cpb cpb;
};

/**
 * @brief 283 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame283_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ame283_cpb cpb;
};

/**
 * @brief 409 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame409_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ame409_cpb cpb;
};

/**
 * @brief 571 bit ECC GF(2^m) Modular Functions
 **/
struct cop_asym_eccm2ame571_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ame571_cpb cpb;
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri163_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[11];
	uint8_t A[21];
	uint8_t _pad1[11];
	uint8_t N[21];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri233_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[2];
	uint8_t A[30];
	uint8_t _pad1[2];
	uint8_t N[30];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri283_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t A[36];
	uint8_t _pad1[12];
	uint8_t N[36];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri409_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t A[52];
	uint8_t _pad1[12];
	uint8_t N[52];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri571_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[8];
	uint8_t A[72];
	uint8_t _pad1[8];
	uint8_t N[72];
};

/**
 * @brief 163 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri163_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ri163_cpb cpb;
};

/**
 * @brief 233 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri233_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ri233_cpb cpb;
};

/**
 * @brief 283 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri283_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ri283_cpb cpb;
};

/**
 * @brief 409 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri409_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ri409_cpb cpb;
};

/**
 * @brief 571 bit ECC GF(2^m) Modular Reduction and Inverse
 **/
struct cop_asym_eccm2ri571_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccm2ri571_cpb cpb;
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add163_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[11];
	uint8_t Xp[21];
	uint8_t _pad1[11];
	uint8_t Yp[21];
	uint8_t _pad2[11];
	uint8_t Xq[21];
	uint8_t _pad3[11];
	uint8_t Yq[21];
	uint8_t _pad4[11];
	uint8_t N[21];
	uint8_t _pad5[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add233_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[2];
	uint8_t Xp[30];
	uint8_t _pad1[2];
	uint8_t Yp[30];
	uint8_t _pad2[2];
	uint8_t Xq[30];
	uint8_t _pad3[2];
	uint8_t Yq[30];
	uint8_t _pad4[2];
	uint8_t N[30];
	uint8_t _pad5[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add283_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[36];
	uint8_t _pad1[12];
	uint8_t Yp[36];
	uint8_t _pad2[12];
	uint8_t Xq[36];
	uint8_t _pad3[12];
	uint8_t Yq[36];
	uint8_t _pad4[12];
	uint8_t N[36];
	uint8_t _pad5[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add409_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[52];
	uint8_t _pad1[12];
	uint8_t Yp[52];
	uint8_t _pad2[12];
	uint8_t Xq[52];
	uint8_t _pad3[12];
	uint8_t Yq[52];
	uint8_t _pad4[12];
	uint8_t N[52];
	uint8_t _pad5[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add571_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[8];
	uint8_t Xp[72];
	uint8_t _pad1[8];
	uint8_t Yp[72];
	uint8_t _pad2[8];
	uint8_t Xq[72];
	uint8_t _pad3[8];
	uint8_t Yq[72];
	uint8_t _pad4[8];
	uint8_t N[72];
	uint8_t _pad5[9];
	uint8_t a[7];
};

/**
 * @brief 163 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add163_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2add163_cpb cpb;
};

/**
 * @brief 233 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add233_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2add233_cpb cpb;
};

/**
 * @brief 283 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add283_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2add283_cpb cpb;
};

/**
 * @brief 409 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add409_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2add409_cpb cpb;
};

/**
 * @brief 571 bit ECC GF(2^m) Point Add
 **/
struct cop_asym_eccp2add571_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2add571_cpb cpb;
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl163_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[11];
	uint8_t Xp[21];
	uint8_t _pad1[11];
	uint8_t Yp[21];
	uint8_t _pad2[11];
	uint8_t N[21];
	uint8_t _pad3[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl233_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[2];
	uint8_t Xp[30];
	uint8_t _pad1[2];
	uint8_t Yp[30];
	uint8_t _pad2[2];
	uint8_t N[30];
	uint8_t _pad3[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl283_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[36];
	uint8_t _pad1[12];
	uint8_t Yp[36];
	uint8_t _pad2[12];
	uint8_t N[36];
	uint8_t _pad3[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl409_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[52];
	uint8_t _pad1[12];
	uint8_t Yp[52];
	uint8_t _pad2[12];
	uint8_t N[52];
	uint8_t _pad3[9];
	uint8_t a[7];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl571_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[8];
	uint8_t Xp[72];
	uint8_t _pad1[8];
	uint8_t Yp[72];
	uint8_t _pad2[8];
	uint8_t N[72];
	uint8_t _pad3[9];
	uint8_t a[7];
};

/**
 * @brief 163 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl163_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2dbl163_cpb cpb;
};

/**
 * @brief 233 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl233_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2dbl233_cpb cpb;
};

/**
 * @brief 283 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl283_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2dbl283_cpb cpb;
};

/**
 * @brief 409 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl409_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2dbl409_cpb cpb;
};

/**
 * @brief 571 bit ECC GF(2^m) Point Double
 **/
struct cop_asym_eccp2dbl571_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2dbl571_cpb cpb;
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul163_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[11];
	uint8_t Xp[21];
	uint8_t _pad1[11];
	uint8_t Yp[21];
	uint8_t _pad2[11];
	uint8_t N[21];
	uint8_t _pad3[9];
	uint8_t a[7];
	uint8_t _pad4[11];
	uint8_t k[21];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul233_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[2];
	uint8_t Xp[30];
	uint8_t _pad1[2];
	uint8_t Yp[30];
	uint8_t _pad2[2];
	uint8_t N[30];
	uint8_t _pad3[9];
	uint8_t a[7];
	uint8_t _pad4[2];
	uint8_t k[30];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul283_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[36];
	uint8_t _pad1[12];
	uint8_t Yp[36];
	uint8_t _pad2[12];
	uint8_t N[36];
	uint8_t _pad3[9];
	uint8_t a[7];
	uint8_t _pad4[12];
	uint8_t k[36];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul409_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[12];
	uint8_t Xp[52];
	uint8_t _pad1[12];
	uint8_t Yp[52];
	uint8_t _pad2[12];
	uint8_t N[52];
	uint8_t _pad3[9];
	uint8_t a[7];
	uint8_t _pad4[12];
	uint8_t k[52];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul571_cpb {
	uint8_t _rsv[16];
	uint8_t _pad0[8];
	uint8_t Xp[72];
	uint8_t _pad1[8];
	uint8_t Yp[72];
	uint8_t _pad2[8];
	uint8_t N[72];
	uint8_t _pad3[9];
	uint8_t a[7];
	uint8_t _pad4[8];
	uint8_t k[72];
};

/**
 * @brief 163 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul163_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2mul163_cpb cpb;
};

/**
 * @brief 233 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul233_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2mul233_cpb cpb;
};

/**
 * @brief 283 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul283_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2mul283_cpb cpb;
};

/**
 * @brief 409 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul409_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2mul409_cpb cpb;
};

/**
 * @brief 571 bit ECC GF(2^m) Point Multiply
 **/
struct cop_asym_eccp2mul571_cxb {
	struct cop_crb crb;
	uint8_t _pad[48];
	struct cop_csb csb;
	struct cop_asym_eccp2mul571_cpb cpb;
};

/**
 * @brief CPB for 192 bit ECC GF(p) Point Operation Result
 **/
struct cop_asym_eccpresult192 {
	uint8_t Xr[24];
	uint8_t Yr[24];
};

/**
 * @brief CPB for 256 bit ECC GF(p) Point Operation Result
 **/
struct cop_asym_eccpresult256 {
	uint8_t Xr[32];
	uint8_t Yr[32];
};

/**
 * @brief CPB for 521 bit ECC GF(p) Point Operation Result
 **/
struct cop_asym_eccpresult521 {
	uint8_t _pad0[14];
	uint8_t Xr[66];
	uint8_t _pad1[14];
	uint8_t Yr[66];
};

/**
 * @brief CPB for 163 bit ECC GF(2^m) Point Operation Result
 **/
struct cop_asym_eccp2result163 {
	uint8_t _pad0[11];
	uint8_t Xr[21];
	uint8_t _pad1[11];
	uint8_t Yr[21];
};

/**
 * @brief CPB for 233 bit ECC GF(2^m) Point Operation Result
 **/
struct cop_asym_eccp2result233 {
	uint8_t _pad0[2];
	uint8_t Xr[30];
	uint8_t _pad1[2];
	uint8_t Yr[30];
};

/**
 * @brief CPB for 283 bit ECC GF(2^m) Point Operation Result
 **/
struct cop_asym_eccp2result283 {
	uint8_t _pad0[12];
	uint8_t Xr[36];
	uint8_t _pad1[12];
	uint8_t Yr[36];
};

/**
 * @brief CPB for 409 bit ECC GF(2^m) Point Operation Result
 **/
struct cop_asym_eccp2result409 {
	uint8_t _pad0[12];
	uint8_t Xr[52];
	uint8_t _pad1[12];
	uint8_t Yr[52];
};

/**
 * @brief CPB for 571 bit ECC GF(2^m) Point Operation Result
 **/
struct cop_asym_eccp2result571 {
	uint8_t _pad0[8];
	uint8_t Xr[72];
	uint8_t _pad1[8];
	uint8_t Yr[72];
};

/** @} ECC */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_ECC_H */
