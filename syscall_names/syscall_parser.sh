#!/bin/sh

# Copyright (C) 2019 HLRS, University of Stuttgart
# <https://www.hlrs.de/>, <https://www.uni-stuttgart.de/>
#
# This file is part of iotrace.
#
# iotrace is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# iotrace is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with iotrace.  If not, see <https://www.gnu.org/licenses/>.
#
# The following people contributed to the project (in alphabetic order
# by surname):
#
# - Nicolas Gross <https://github.com/nicolasgross>


# Converts syscall_64.tbl to a C array. Generates a .c and .h file.


if [ "$#" -ne 3 ]; then
	echo "Illegal number of parameters"
	echo "arg1: input (e.g. ./syscall_64.tbl)"
	echo "arg2: output .c file (e.g. ../src/syscall_names.c)"
	echo "arg3: output .h file (e.g. ../src/syscall_names.h)"
	exit
fi

in="$1"
out_c="$2"
out_h="$3"
tbl=`cat $in | grep -o '^[^#]*'`

# .c file:

echo "#include \"syscall_names.h\"\n\n" > "$out_c"
echo "char const *const syscall_names[] = {" >> "$out_c"

lastnum=-1
printf "%s" "$tbl" |
while read nr abi name entry; do
	tmp=$(($nr - 1))
	while [ "$lastnum" -lt "$tmp" ]; do
		lastnum=$(($lastnum + 1))
		`echo "\tUNDEFINED_SYSCALL,\t\t// nr: $lastnum" >> "$out_c"`
	done
	`echo "\t\"$name\",\t\t// nr: $nr, abi: $abi" >> "$out_c"`
	lastnum=$nr
done

echo "};\n" >> "$out_c"

# .h file:

echo "#ifndef IOTRACE_SYSCALL_NAMES_H" > "$out_h"
echo "#define IOTRACE_SYSCALL_NAMES_H\n" >> "$out_h"
echo "#define UNDEFINED_SYSCALL \"UNDEFINED_SYSCALL\"" >> "$out_h"
name_count=$((`wc -l < "$out_c"` - 6))
echo "#define NAMES_LENGTH $name_count\n\n" >> "$out_h"
echo "extern const char *const syscall_names[NAMES_LENGTH];\n\n" >> "$out_h"
echo "#endif\n" >> "$out_h"

