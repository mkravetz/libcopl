#!/usr/bin/python

#
# Written by Chris J Arges <arges@linux.vnet.ibm.com>
#

import sys
import csv
import os
import string

def round_qw(x):
	return ((x+0xff) & ~0xff)

symfiles = ['AES','DES','KASUMI','RC4','HASH','HMAC','AES_HMAC','TDES_HMAC']
for i in symfiles:
	vars()[i] = []

symfile_desc = {'AES':'Advanced Encryption Standard interfaces.',
		'DES':'Data Encryption Standard interfaces.',
		'KASUMI':'Kasumi F8/F9 interfaces.',
		'RC4':'Alleged Rivest Cipher 4 interfaces.',
		'HASH':'Hashing (SHA1/SHA256/SHA512/MD5) interfaces.',
		'HMAC':'SHA Hashing Message Authentication Code interfaces.',
		'AES_HMAC':'AES and Hashing Message Authentication Code combination mode interfaces.',
		'TDES_HMAC':'Triple DES and Hashing Message Authentication Code combination mode interfaces.'}

def mk_cpb_struct(name):
	return "struct cop_sym_" + name + "_cpb"	

def print_header(file, name):
	print >>file,\
"/*\n\
   Licensed Materials - Property of IBM\n\
   Restricted Materials of IBM\n\
\n\
   Copyright 2008,2009, 2010\n\
   Chris J Arges\n\
   IBM Corp, All Rights Reserved\n\
\n\
   US Government Users Restricted Rights - Use Duplication or\n\
   disclosure restricted by GSA ADP Schedule Contract with IBM\n\
   Corp\n\
\n\
*/\n\
\n\
#ifndef _INCLUDE_LIBCOPL_CRYPTO_" + name.upper() + "_H\n\
#define _INCLUDE_LIBCOPL_CRYPTO_" + name.upper() + "_H\n\
\n\
#include <libcopl/crypto.h>\n"

def print_file_doxy(file, f):
	print >>file,\
"/**\n\
 * @ingroup CRYPTO\n\
 * @defgroup " + f + " " + symfile_desc[f] + "\n\
 * @{\n\
 */\n"


def print_footer(file, f):
	print >>file,\
"/** @} " + f + " */\n\
#endif\t/* _INCLUDE_LIBCOPL_CRYPTO_" + f.upper() + "_H */\n"

################################################################################
# CPB STRUCTURES
#

def check_size(size, fn_name):
	if size < 1:
		print "check your structure definition for " + fn_name + " ."
		sys.exit()

def print_param(strings, name, size, desc):
	check_size(size, name)

	if (name[:3]== "mbl" or name[:4] == "blen") and size == 8:
		strings.append("\tuint64_t " + name ,)
	elif (name[:3]== "mbl") and size == 16:
		strings.append("\tuint64_t " + name + "[2]")
	else:
		strings.append("\tuint8_t " + name)
		if size > 1:
			strings.append("[" + str(size) + "]")

	comment = "/**< @brief " + string.capitalize(desc) + " */"
	strings.append("; " + comment + "\n")


def print_rsv(strings, num, size):
	check_size(size, "_rsv")
	strings.append("\tuint8_t _rsv" + str(num) + "[" + str(size) + "];\n")
	#strings.append(" /**< Unused, required for alignment. */\n")

def print_struct(strings, name, fn_desc, cpb_size, total, pm_dir, pm_names, pm_descs,
	pm_sizes, pm_offsets, spbc_offset, cpb_nohead_size):

	# print header
	strings.append("/**\n * @brief " + fn_desc + ", " + str(cpb_size) + " bytes long." + "\n")
	strings.append(" **/\n")
	strings.append( mk_cpb_struct(name) + " {\n\tstruct cop_sym_cpb_hdr hdr;")
 	strings.append("/**< @brief Common symmetric-crypto header. */\n")

	# print parameters in structure
	byte_no = 0
	rsv_no = 0
	for i in range(0, total):
		if byte_no == pm_offsets[i]:
			print_param(strings, pm_names[i], pm_sizes[i], pm_descs[i])
			byte_no += pm_sizes[i]
		else:
			pad_bytes = (pm_offsets[i] - byte_no)
			print_rsv(strings, rsv_no, pad_bytes)
			rsv_no += 1
			byte_no += pad_bytes
			print_param(strings, pm_names[i], pm_sizes[i], pm_descs[i])
			byte_no += pm_sizes[i]

	# add spbc
	if byte_no == spbc_offset:
		byte_no += 4
	else:
		pad_bytes = (spbc_offset - byte_no)
		print_rsv(strings, rsv_no, pad_bytes)
		rsv_no += 1
		byte_no += pad_bytes
		byte_no += 4
	
	strings.append("\tuint32_t spbc; /**< @brief Source Pointer Byte Count */\n")

	# end padding
	if byte_no != cpb_nohead_size:	
		pad_bytes = (cpb_nohead_size - byte_no)
		print_rsv(strings, rsv_no, pad_bytes)

	strings.append("};\n\n")

def print_cxb(strings, name, cpb_size):
	# print header
	strings.append("/**\n * @brief " + name + " CRB/CCB/CSB/CPB helper structure." + "\n")
	strings.append(" **/\n")

	# print structure 
	strings.append("struct cop_sym_" + name + "_cxb {\n")
	strings.append("\tstruct cop_crb crb; /**< @brief Coprocessor request block. */\n")

	total = cpb_size + 16 + 64
	pad_sz = (round_qw(total) - total) & 127

	strings.append("\tuint8_t _pad[" + str(pad_sz) + "]; /**<  @brief Padding to ensure alignment. */\n")

	strings.append("\tstruct cop_csb csb; /**< @brief Coprocessor status block. */\n")
	strings.append("\tstruct cop_sym_" + name + "_cpb cpb; /**< @brief Coprocessor Parameter block. */\n")
	strings.append("};\n\n")

################################################################################
# Write structures 

reader = csv.reader(open("sym_structs_def.csv", "rb"), delimiter=';', quoting=csv.QUOTE_ALL)

for row in reader:
	# skip comments
	while row[0][:1] == "#":
		row = reader.next()

	# read in everything line by line
	strings = []
	fn_name = row[0].lower()
	pm_names = []
	pm_sizes = []
	pm_offsets = []
	pm_descs = []
	pm_dir = []

	total = int(row[1])
	spbc_offset = int(row[3])
	cpb_size = int(row[2])
	cpb_nohead_size = int(row[2]) - 16
	fn_desc = row[5]
	fn_group = row[4]

	for i in range(0, total):
		row = reader.next()
		pm_names += [row[0].lower()]
		pm_sizes += [int(row[3])]
		pm_offsets += [int(row[2])]
		pm_dir += [row[1]]
		pm_descs += [row[5]]


	# print structures
	print_struct(strings, fn_name, fn_desc, cpb_size, total, pm_dir, pm_names, pm_descs,
		pm_sizes, pm_offsets, spbc_offset, cpb_nohead_size)

	print_cxb(strings, fn_name, cpb_size)

	vars()[fn_group] += strings
	

################################################################################
# write setup functions out

reader = csv.reader(open("sym_setup_def.csv", "rb"), delimiter=';', quoting=csv.QUOTE_ALL)

for row in reader:
	# skip comments
	while row[0][:1] == "#":
		row = reader.next()

	strings = []
	name = row[0].lower()
	fn = row[1]
	group = row[2]
	mode = row[3]
	enc = row[4]
	flag = row[5]
	pad = row[6]
	ks = row[7]
	ds = row[8]
	source = row[9]
	target = row[10]
	struct = row[11]
	desc = row[12]

	#######################################################################
	# print doxygen header

	strings.append("\
/**\n\
 * @brief Setup required coprocessor request block fields for " + name + ".\n\
 *\n\
 * Setup function for " + name + ".\n\
 * @a crb must be setup before and have a valid CSB pointer. The source\n\
 * @a src data must be mapped for that particular coprocessor before.\n\
 *\n\
 * @pre @a crb must be valid, and the csb field must be a valid and \n\
 * 	mapped block, directly after the csb must follow enough\n\
 *	mapped memory to accommodate sizeof(" + struct + ").\n\
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given\n\
 *	this field will not be set, which is useful for reusing CRBs.\n\
 * @param[in] crb a coprocessor request block\n\
 * @param[in] ctx cop_ctx structure from open/bound copro interface\n\
 * @param[in] src pointer to source buffer\n\
 * @param[in] src_sz size in bytes of source data\n")

	if target != "0":
 		strings.append(" * @param[in] tgt pointer to target buffer\n")
		strings.append(" * @param[in] tgt_sz size of target buffer in bytes\n")

	if enc == "?":
		strings.append(" * @param[in] enc 1 = encryption, 0 = decryption\n")
	
	if flag == "?":
		strings.append(" * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}\n")

	if pad == "?":
		strings.append(" * @param[in] pad {COP_RPAD_NONE, COP_RPAD_ZERO, COP_RPAD_INC, COP_RPAD_USE}\n")
		strings.append(" * @param[in] rpad byte used per padding option in @a pad\n")

	if ks == "?":
		strings.append(" * @param[in] ks key size",)
		if fn == "AES" or fn == "AES_HMAC" or fn == "TDES_HMAC":
			strings.append(" {KS_AES_128, KS_AES_192, KS_AES_256}\n")

	strings.append("\
 *\n\
 * Returns -1 on failure, 0 on success.\n\
 */\n")

	#######################################################################
	# print function header

	strings.append("static inline int cop_" + name + 
		"_setup(struct cop_crb *crb, struct cop_ctx *ctx,\n\tconst void *src, size_t src_sz",)

	if target != "0":
		strings.append(", void *tgt, size_t tgt_sz",)

	if enc == "?":
		strings.append(", uint8_t ende",)

	if flag == "?":
		strings.append(", uint8_t flag",)

	if pad == "?":
		strings.append(", uint8_t pad, uint8_t rpad",)

	if ks == "?":
		strings.append(", uint8_t ks",)

	if ds == "?":
		strings.append(", uint8_t ds",)

	strings.append(")\n")
	strings.append("{\n")

	#######################################################################
	#print function body

	strings.append("\tstruct cop_sym_cpb_hdr *hdr = (struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);\n")
	strings.append("\thdr->mode = " + mode + ";\n")

	if enc == "?":
		strings.append("hdr->fdm.ende = ende;\n",)

	if flag == "?":
		strings.append("hdr->fdm.i = IS_I(flag);\n",)
		strings.append("hdr->fdm.c = IS_C(flag);\n",)

	if pad == "?":
		strings.append("hdr->fdm.pr = pad;\n",)

	if ks == "?":
		strings.append("hdr->ks = ks;\n",)
	elif ks != "0":
		strings.append("hdr->ks = " + ks + ";\n",)

	if ds == "?":
		strings.append("hdr->ds = ds;\n",)
	elif ds != "0":
		strings.append("hdr->ds = " + ds + ";\n",)

	if pad == "?":
		strings.append("hdr->rpad = rpad;\n",)

	strings.append("return cop_sym_setup(crb, ctx, src, src_sz,",)
	
	if target != "0":
		strings.append("tgt, tgt_sz,",)
	else:
		strings.append("NULL, 0,",)
		
	strings.append("FN_" + fn + ");\n")
	strings.append("}\n")

	vars()[group] += strings

################################################################################
# Write all files out

for f in symfiles:
	file = open( f.lower() + ".h", 'w')

	print_header(file, f)

	print_file_doxy(file,f)

	for i in vars()[f]:
		print >>file,  i,

	print_footer(file, f)

	file.close()
	os.system('indent -linux -nlp -ci8 ' + f.lower() + ".h")
	os.system('rm *.h~')


