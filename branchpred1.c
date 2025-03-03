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
sum_v1(int *input, int len, int64_t *sumzero_to_ten, int64_t *sumother)
{
	int64_t zero_to_ten = 0;
	int64_t other = 0;
	
	for (int i = 0; i < len; i++)
	{
		if (input[i] >= 0 && input[i] <= 10)
			zero_to_ten += input[i];
		else
			other += input[i];
	}

	*sumzero_to_ten = zero_to_ten;
	*sumother = other;
}

static inline void
sum_v2(int *input, int len, int64_t *sumzero_to_ten, int64_t *sumother)
{
	int64_t zero_to_ten = 0;
	int64_t other = 0;
	
	for (int i = 0; i < len; i++)
	{
		if (input[i] >= 0 & input[i] <= 10)
			zero_to_ten += input[i];
		else
			other += input[i];
	}

	*sumzero_to_ten = zero_to_ten;
	*sumother = other;
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

#define NINPUTS 10000

static void
populate_input(int *input, int len)
{
	for (int i = 0; i < len; i++)
		input[i] = rand() + rand();
	
	// for (int i = 0; i < 10; i++)
		// printf("%d\n", input[i]);
}

int main()
{
	struct timespec start,end;
	int64_t v1_time, v2_time;
	int *input = (int *) malloc(NINPUTS * sizeof(int));
	int64_t sum1, sum2;
	
	if (input == NULL)
	{
		fprintf(stderr, "malloc failure\n");
		return -1;
	}

	populate_input(input, NINPUTS);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_v1(input, NINPUTS, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	v1_time = get_clock_diff(&end, &start);
	printf("v1: done in %ld nanoseconds\n", v1_time);
	printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum_v2(input, NINPUTS, &sum1, &sum2);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	v2_time = get_clock_diff(&end, &start);
	printf("v2: done in %ld nanoseconds (%g times faster than v2)\n", v2_time, (double) v1_time / v2_time);	
	printf("sum1 = %" PRId64 ", sum2 = %" PRId64 "\n", sum1, sum2);

	return 0;
}