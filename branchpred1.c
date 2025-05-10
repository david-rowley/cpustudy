#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define LOOPS 100

static inline void
sum_v1(int *input, int len, int64_t *sumneg, int64_t *sumpos)
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
sum_v2(int *input, int len, int64_t *sumneg, int64_t *sumpos)
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

#define NINPUTS 10000000

static void
populate_input(int *input, int len)
{
	for (int i = 0; i < len; i++)
		input[i] = rand() + rand();
	
	// for (int i = 0; i < 10; i++)
		// printf("%d\n", input[i]);
}

int main(int argc, char **argv)
{
	struct timespec start,end;
	int64_t v1_time, v2_time;
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

	populate_input(numbers, nnumbers);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_v1(numbers, nnumbers, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	v1_time = get_clock_diff(&end, &start) / LOOPS;
	//printf("v1: done in %ld nanoseconds\n", v1_time);
//	printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_v2(numbers, nnumbers, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	v2_time = get_clock_diff(&end, &start) / LOOPS;
//	printf("v2: done in %ld nanoseconds (%g times faster than v2)\n", v2_time, (double) v1_time / v2_time);	
//printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);
	printf("v1_time %ld v2_time %ld\n", v1_time, v2_time);

	return 0;
}
