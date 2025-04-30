#!/bin/bash
gcc -O0 branchpred1.c -o branchpred1 
for n in $(seq 1000 1000 1000000)
do
	echo -n "$n " && perf stat ./branchpred1 $n 2>&1 | grep branch-misses;
done
