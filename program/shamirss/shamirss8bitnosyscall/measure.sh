#! /bin/bash

# Choose measure mode
echo "Choose measure mode (1:time or 2:system call)"
echo -n "Please input 1 or 2:"
read mmode
echo ""
if [ ${mmode} -ne 1 -a ${mmode} -ne 2 ]; then
	echo "Error: invalid mode, please retype parameter (split or combine)."
	exit 1
fi

if [ ${mmode} -eq 1 ]; then
	mmode="time"
elif [ ${mmode} -eq 2 ]; then
	mmode="strace -c"
fi

# Choose secret sharing mode
echo -n "Choose mode (split or combine):"
read ssmode
echo ""

if [ "${ssmode}" != "split" -a "${ssmode}" != "combine" ]; then
	echo "Error: invalid mode, please retype parameter (split or combine)."
	exit 1
fi

# In case of split mode
if [ "${ssmode}" = "split" ]; then
	# Choose test file
	echo -n "Test file size (large or small):"
	read size
	echo ""

	if [ "${size}" = "large" ]; then
		testFile="../test_large.txt"
	elif [ "${size}" = "small" ]; then
		testFile="../test_small.txt"
	else
		echo "Error: invalid size, please retype parameter (large or small)."
		exit 1
	fi

	# Input parameter
	echo -n "Input the number of share:"
	read n
	echo ""
	if [ ${n} -ge 256 ]; then
		echo "Error: invalid parameter, please retype parameter (n < 256)."
		exit 1
	fi

	echo -n "Input the number of degree of polynomial:"
	read k
	echo ""
	if [ ${k} -gt ${n} ]; then
		echo "Error: invalid parameter, please retype parameter (n >= k)."
		exit 1
	fi

	# Clean files
	echo "---initializing---"
	make clean
	echo ""

	eval ${mmode} ./shamirss8bitnosyscall --mode=${ssmode} -n${n} -k${k} ${testFile}

elif [ "${ssmode}" = "combine" ]; then
	# Input parameter
	echo -n "Input the number of share:"
	read num
	echo ""
	if [ ${num} -ge 256 ]; then
		echo "Error: invalid parameter, please retype parameter (n < 256)."
		exit 1
	fi

	# Set share file name
	echo "Use 1 to ${num} share"
	files=()
	for var in `seq ${num} -1 1`
	do
		files+=("${var}.share")
	done

	eval ${mmode} ./shamirss8bitnosyscall --mode=${ssmode} ${files[@]}

fi

