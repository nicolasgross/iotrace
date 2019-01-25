#!/bin/sh


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

echo "#define UNDEFINED \"UNDEFINED\"\n" > "$out_c"
echo "const char *const syscall_names[] = {" >> "$out_c"

lastnum=-1
printf "%s" "$tbl" |
while read nr abi name entry; do
	tmp=$(($nr - 1))
	while [ "$lastnum" -lt "$tmp" ]; do
		lastnum=$(($lastnum + 1))
		`echo "\tUNDEFINED,\t\t// nr: $lastnum" >> "$out_c"`
	done
	`echo "\t\"$name\",\t\t// nr: $nr, abi: $abi" >> "$out_c"`
	lastnum=$nr
done

echo "};\n" >> "$out_c"

# .h file:

echo "#ifndef IOTRACE_SYSCALL_NAMES_H" > "$out_h"
echo "#define IOTRACE_SYSCALL_NAMES_H\n" >> "$out_h"
name_count=$((`wc -l < "$out_c"` - 5))
echo "#define NAMES_LENGTH $name_count\n" >> "$out_h"
echo "extern const char *const syscall_names[NAMES_LENGTH];\n" >> "$out_h"
echo "#endif\n" >> "$out_h"

