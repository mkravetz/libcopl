#!/usr/bin/python

#
# Written by Chris J Arges <arges@linux.vnet.ibm.com>
#

# UNDER CONSTRUCTION

import sys
import csv
import os
import math
import string

symfiles = ['IMF','ECC', 'ASYM']
for i in symfiles:
	vars()[i] = []

symfile_desc = {'IMF':'Integer Modular Asymmetric Cryptographic Functions.',
		'ASYM':'General Asymmetric Cryptographic definitions.',
		'ECC':'Eliptical Curve Cryptographic Functions.'}

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
\n"

#include <libcopl/asym.h>\n"

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

reader = csv.reader(open("rc_encode.csv", "rb"), delimiter=';', quoting=csv.QUOTE_ALL)

rc_encode = {}

def round_qw(x):
	i = x
	while (i % 16) != 0:
		i = i + 1
	return i

def print_param(strings, name, size, desc):
	strings.append("\tuint8_t " + name)
	if size > 1:
		strings.append("[" + str(size) + "];\n")

	#comment = "/**< @brief " + string.capitalize(desc) + " */"
	#strings.append("; " + comment + "\n")

def print_struct(strings, name, desc, result_sz, op_names, op_types, cpb):

	# print header
	strings.append("/**\n * @brief " + desc + "\n")
	strings.append(" **/\n")
	strings.append("struct cop_asym_" + name + result_sz + cpb + " {\n")
	strings.append("\tuint8_t _rsv[16];\n"),

	# print parameters in structure
	byte_no = 0
	pad_no = 0
	for i in range(len(op_names)):

		op_sz = rc_encode[rc][int(op_types[i])-1]
		param_sz = int(math.ceil(float(op_sz)/8))
		pad_sz = round_qw(param_sz) - param_sz

		if (int(op_sz)%8) or (param_sz < 8):
			strings.append("\tuint8_t _pad" + str(pad_no) + "[" + str(pad_sz) + "];\n")
			pad_no += 1
			byte_no += pad_sz
		print_param(strings, op_names[i], str(param_sz), "...")
		byte_no += param_sz

	pad_bytes = (byte_no % 16)

	if (pad_bytes):
		strings.append("\tuint8_t _pad" + str(pad_no) + "[" + str(16 - pad_bytes) + "];\n")

	strings.append("};\n\n")



def print_result(strings, name, desc, result_sz, op_names, op_types, cpb):

	# print header
	strings.append("/**\n * @brief " + desc + "\n")
	strings.append(" **/\n")
	strings.append("struct cop_asym_" + name + result_sz + cpb + " {\n")

	# print parameters in structure
	byte_no = 0
	pad_no = 0
	for i in range(len(op_names)):

		op_sz = rc_encode[rc][int(op_types[i])-1]
		param_sz = int(math.ceil(float(op_sz)/8))
		pad_sz = round_qw(param_sz) - param_sz

		if (int(op_sz)%8) or (param_sz < 8):
			strings.append("\tuint8_t _pad" + str(pad_no) + "[" + str(pad_sz) + "];\n")
			pad_no += 1
			byte_no += pad_sz
		print_param(strings, op_names[i], str(param_sz), "...")
		byte_no += param_sz

	pad_bytes = (byte_no % 16)

	if (pad_bytes):
		strings.append("\tuint8_t _pad" + str(pad_no) + "[" + str(16 - pad_bytes) + "];\n")

	strings.append("};\n\n")



################################################################################
# Read in RC Encode Table
for row in reader:
	if row[0] == "RC":
		row = reader.next();
	
	if row[0] == "###":
		break;

	rc_encode[row[0]] = row[1:8]
	

# Read in structures and create
operand_types = []

for row in reader:
	strings = []

	if row[0] == "Type":
		row = reader.next();
	
	if row[0] == "###":
		break;

	# Gather data
	name = row[0].lower()
	desc = row[1]
	op_names = row[2].split(',')
	op_types = row[3].split(',')
	result_type = row[5]
	rcs = row[4].split(',')
	if rcs == ['']:
		break;

	for rc in rcs:
		# Get result size
		result_sz = rc_encode[rc][int(result_type)]
		print_struct(strings, name, "CPB for " + result_sz + " bit " + 
			desc, result_sz, op_names, op_types, "_cpb")

	for rc in rcs:
		result_sz = rc_encode[rc][int(result_type)]
		strings.append("/**\n * @brief " + result_sz + " bit " + desc + "\n")
		strings.append(" **/\n")
		strings.append("struct cop_asym_" + name + result_sz + "_cxb {\n")
        	strings.append("\tstruct cop_crb crb;\n")
	        strings.append("\tuint8_t _pad[48];\n")
		strings.append("\tstruct cop_csb csb;\n")
		strings.append("\tstruct cop_asym_" + name + result_sz + "_cpb cpb;\n")
		strings.append("};\n\n")


	if name[:2] == "ec":
		vars()['ECC'] += strings
	elif name[:2] == "im":
		vars()['IMF'] += strings



# READ IN RESULT STRUCTURES
for row in reader:
	strings = []

	if row[0] == "Type":
		row = reader.next();
	
	if row[0] == "###":
		break;

	# Gather data
	name = row[0].lower()
	desc = row[1]
	op_names = row[2].split(',')
	op_types = row[3].split(',')
	result_type = row[5]
	rcs = row[4].split(',')
	if rcs == ['']:
		break;

	for rc in rcs:
		# Get result size
		result_sz = rc_encode[rc][int(result_type)]
		print_result(strings, name, "CPB for " + result_sz + " bit " + 
			desc, result_sz, op_names, op_types, "")

	if name[:2] == "ec":
		vars()['ECC'] += strings
	elif name[:2] == "im":
		vars()['IMF'] += strings


def print_enum(strings, name, fc, type, desc, equation):
	strings.append("/**\n * @brief " + desc + ". \n")
	strings.append(" * Performs the operation <b>" + equation + "</b>. " +
		"Uses cop_asym_" + type.lower() + "*_cpb structures. */\n");
	strings.append("COP_ASYM_" + name + "=" + fc + ",\n");

	
for row in reader:
	strings = []

	if row[0] == "Function":
		row = reader.next();
	
	if row[0] == "###":
		break;

	name = row[0].strip()
	fc = row[1]
	type = row[2]
	desc = row[3]
	equation = row[4]

	print_enum(strings, name, fc, type, desc, equation)

	vars()['ASYM'] += strings



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

