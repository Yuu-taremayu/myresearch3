#! /bin/bash

# execute split mode by using all parameter (1 <= n < 256, 1 <= k <= n)
# and measure each elapsed time

# file check
# if file exists, exit
fileName="auto_split_result.txt"
if [ -e ${fileName} ]; then
	echo "Result file already exists."
	exit 1
fi
touch ${fileName}

maxParam=255
# The number of share "n" loop
for i in `seq ${maxParam}`
do
	out=""
	# The number of degree of polynomial "k" loop
	for j in `seq ${i}`
	do
		# initializing
		make clean > /dev/null

		# execute command and calculating elapsed time
		result=$((time ./shamirss8bitnosyscall --mode=split -n${i} -k${j} ../test_small.txt) 2>&1 |\
			awk -F' ' '{print $2}' | head -n2 | tail -n1)
		minute=$(echo ${result} | cut -d'm' -f1)
		second=$(echo ${result} | cut -d'm' -f2 | cut -d's' -f1)
		second=$(echo "${minute}*60 + ${second}" | bc)

		# output as 3D data
		out="${i} ${j} ${second}"
		echo ${out} >> ${fileName}
	done
done
