#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define LOOPS 1000000

static inline void
sum_branchy(int *input, int len, int64_t *sumneg, int64_t *sumpos)
{
	int64_t neg = 0;
	int64_t pos = 0;
	
	for (int i = 0; i < len; i++)
	{
		if (input[i] < 0)
			neg += input[i];
		else
			pos += input[i];
	}

	*sumneg = neg;
	*sumpos = pos;
}

static inline void
sum_branchless(int *input, int len, int64_t *sumneg, int64_t *sumpos)
{
	int64_t sums[2] = { 0, 0};
	
	for (int i = 0; i < len; i++)
	{
		sums[(input[i] > 0)] += input[i];
	}

	*sumneg = sums[0];
	*sumpos = sums[1];
}

#define NANOSEC_PER_SEC 1000000000

// Returns difference in nanoseconds
int64_t
get_clock_diff(struct timespec *t1, struct timespec *t2)
{
	int64_t nanosec = (t1->tv_sec - t2->tv_sec) * NANOSEC_PER_SEC;
	nanosec += (t1->tv_nsec - t2->tv_nsec);

	return nanosec;
}

static void
populate_array(int *input, int len)
{
	int number = 0 - (len / 2);

	/* Make half the numbers negative and half positive */
	for (int i = 0; i < len; i++)
		input[i] = number++;
}

int main(int argc, char **argv)
{
	struct timespec start,end;
	int64_t branchy_time, branchless_time;
	int *numbers;
	int nnumbers;
	int64_t sum1, sum2;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <array size>\n", argv[0]);
		return -1;
	}

	nnumbers = atoi(argv[1]);
	
	if (nnumbers < 0)
	{
		fprintf(stderr, "Usage: %s <array size>\n", argv[0]);
		return -1;
	}


	numbers = (int *) malloc(nnumbers * sizeof(int));
	if (numbers == NULL)
	{
		fprintf(stderr, "unable to allocate %zu bytes\n", nnumbers * sizeof(int));
		return -2;
	}

	populate_array(numbers, nnumbers);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_branchy(numbers, nnumbers, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	branchy_time = get_clock_diff(&end, &start) / LOOPS;
	printf("branchy: done in %ld nanoseconds\n", branchy_time);
	printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_branchless(numbers, nnumbers, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	branchless_time = get_clock_diff(&end, &start) / LOOPS;
	printf("branchless: done in %ld nanoseconds (%g times faster than branchy)\n", v2_time, (double) branchy_time / branchless_time);
	printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);

	return 0;
}