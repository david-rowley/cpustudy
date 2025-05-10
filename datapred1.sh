#!/bin/bash
gcc -O2 datapred1.c -DNBIG -o datapred1
for n in 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432 67108864 134217728
do
	echo -n "$n " && perf record -e stalled-cycles-backend ./datapred1 $n 2>&1 && perf report | grep sum_array
done
