#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define LOOPS 100

#ifdef BIG
#define ARRAY_TYPE int64_t
#else
#define ARRAY_TYPE int
#endif

static int64_t __attribute__((noinline))
sum_array(ARRAY_TYPE *input_array, ARRAY_TYPE *positions, int len)
{
	int64_t sum = 0;
	
	for (int i = 0; i < len; i++)
	{
		sum += input_array[positions[i]];
	}

	return sum;
}

static int64_t __attribute__((noinline))
sum_array_random(ARRAY_TYPE *input_array, ARRAY_TYPE *positions, int len)
{
	int64_t sum = 0;
	
	for (int i = 0; i < len; i++)
	{
		sum += input_array[positions[i]];
	}

	return sum;
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

void shuffle(ARRAY_TYPE *array, int len)
{
	if (len > 1) 
	{
		for (int i = 0; i < len - 1; i++) 
		{
			int j = i + rand() / (RAND_MAX / (len - i) + 1);
			ARRAY_TYPE t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}

#ifdef DEBUG
	for (int i = 0; i < len; i++)
		printf("%ld\n", (int64_t) array[i]);
#endif

}

static void
populate_arrays(ARRAY_TYPE *input, ARRAY_TYPE *positions, int len)
{
	for (int i = 0; i < len; i++)
		positions[i] = input[i] = i;
}


int main(int argc, char **argv)
{
	struct timespec start,end;
	int64_t inorder_time, randomorder_time;
	volatile int64_t sum1;
	volatile int64_t sum2;
	ARRAY_TYPE *numbers;
	ARRAY_TYPE *positions;
	int nnumbers;

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


	numbers = (ARRAY_TYPE *) malloc(nnumbers * sizeof(ARRAY_TYPE));
	if (numbers == NULL)
	{
		fprintf(stderr, "unable to allocate %zu bytes\n", nnumbers * sizeof(int));
		return -2;
	}

	positions = (ARRAY_TYPE *) malloc(nnumbers * sizeof(ARRAY_TYPE));
	if (positions == NULL)
	{
		fprintf(stderr, "unable to allocate %zu bytes\n", nnumbers * sizeof(int));
		return -2;
	}

	populate_arrays(numbers, positions, nnumbers);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum1 = sum_array(numbers, positions, nnumbers);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	inorder_time = get_clock_diff(&end, &start) / LOOPS;

	shuffle(positions, nnumbers);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (int i = 0; i < LOOPS; i++)
	{
		sum2 = sum_array_random(numbers, positions, nnumbers);
	}

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
	randomorder_time = get_clock_diff(&end, &start) / LOOPS;
	printf("time: linear in %ld nanoseconds (sum1=%ld) random in %ld nanoseconds (sum2=%ld) (%g times slower than in order)\n", inorder_time, sum1, randomorder_time, sum2, (double) randomorder_time / inorder_time);	

	return 0;
}
