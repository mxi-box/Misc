/* Calculate e with some parallel bignum magic and memory bandwidth saving techniques */
/* NOTE: index 0 is the highest word (Big Endian) */

/* TODO: clean up the whole parallelism / intensity mess */

/* How the parallelization works:
 * Because:
 * 1. the divisor is decreasing from terms to 1
 * 2. each cell in the fixed-point number array will only be touched once per iteration
 * 3. calculation in each cell depends on the previous cell
 * 
 * So:
 * 1. the calculation for next iteration could start before the last one completes
 * 2. it can be implemented as a pipeline
 * 
 * What is "intensity":
 * it's a term I borrowed from GPU mining program, it denotes how many iterations to be
 * done in a single pass. The higher this value is, the more it decreases memory bandwidth usage.
 * But this increases usage of L1/L2 cache, be careful not to set it too high. 256 is
 * generally enough for most usage.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include <omp.h>
#include <sched.h>

#define true	1
#define false	0

#define WORD_SIZE (64)

typedef uint64_t word_t;
// support for 128bit integer division is essential to its performance
#if __SIZEOF_INT128__ != 16
	#error "No 128-bit integer support"
#endif
typedef __uint128_t dword_t; // necessary
typedef uint8_t byte;

// approximate log2(n!)
double log2fractorial(word_t n)
{
	return log2(2 * M_PI)/2 + log2(n) * (n + 0.5) - n / log(2);
}

volatile word_t ctr = 0;
volatile word_t secs = 0;
word_t terms = 5;

// display progress based on ctr/terms
void display(union sigval sigval)
{
	// silence the compiler
	(void)sigval;

	static word_t last = 0;
	if(last > terms)
		last = 0;

	secs += 1;
	fprintf(stderr, ">%7.3f%% (%" PRIu64 "/%" PRIu64 ") @ %zu op/s (%zu op/s avg.)\n",
		(float)ctr * 100 / terms,
		ctr,
		terms,
		ctr - last,
		ctr / secs
	);
	last = ctr;
}

size_t to_decimal_precision(size_t n, size_t word_size)
{
	const size_t digits = floor(log(2)/log(10) * n * word_size);
	return digits;
}

static inline word_t intpow10(byte n)
{
	word_t r = 1;
	assert(n <= 19);
	for(word_t i = 0; i < n; i++)
		r *= 10;
	return r;
}

// convert bin to decimal in base 10^19
#define GROUP_SIZE (19)
const word_t pow10_19 = 10000000000000000000ULL;

static inline word_t lfixmul(word_t *frac, word_t mul, word_t carry_in)
{
	dword_t wide = (dword_t)(*frac) * mul + carry_in;
	(*frac) = wide; // lower 64 bits
	wide >>= WORD_SIZE; // carry
	return (word_t)wide;
}

void print_fraction(word_t *frac, size_t n, size_t digits, word_t intensity)
{
	word_t carrys[intensity];

	// new progress
	ctr = 0;
	terms = digits/GROUP_SIZE;
	// in groups of digits
	for(size_t i = 0; i < digits/(GROUP_SIZE * intensity); i++)
	{
		for(size_t j = 0; j < intensity; j++)
			carrys[j] = 0;
		for(ssize_t j = n - 1; j >= 0; j--)
		{
			for(size_t k = 0; k < intensity; k++)
				carrys[k] = lfixmul(&frac[j], pow10_19, carrys[k]);
		}
		for(size_t j = 0; j < intensity; j++)
			printf("%019" PRIu64, carrys[j]);
		
		ctr += intensity;
	}

	size_t rest = digits % (GROUP_SIZE * intensity);
	size_t rest_intensity = rest / GROUP_SIZE;

	// use max possible intensity in 19-digit group
	for(size_t j = 0; j < rest_intensity; j++)
		carrys[j] = 0;
	for(ssize_t j = n - 1; j >= 0; j--)
	{
		for(size_t k = 0; k < rest_intensity; k++)
			carrys[k] = lfixmul(&frac[j], pow10_19, carrys[k]);
	}
	for(size_t j = 0; j < rest_intensity; j++)
		printf("%019" PRIu64, carrys[j]);
	ctr += rest_intensity;

	rest %= GROUP_SIZE;
	char fmtspec[32];

	// the rest in one group
	word_t carry = 0;
	word_t pow10 = intpow10(rest);
	for(ssize_t j = n - 1; j >= 0; j--)
	{
		carry = lfixmul(&frac[j], pow10, carry);
	}
	sprintf(fmtspec, "%%0%zu" PRIu64, rest);
	printf(fmtspec, carry);
}

static inline uint64_t udiv128by64to64(uint64_t u1, uint64_t u0, uint64_t v,
                                     uint64_t *r) {
#if defined(__x86_64__)
  uint64_t  result;
  __asm__("divq %[v]"
          : "=a"(result), "=d"(*r)
          : [ v ] "r"(v), "a"(u0), "d"(u1));
  return result;
#else
  #error "No 128-bit integer support"
#endif
}

/* Algorithm by Steve Wozniak in 1980
 * https://archive.org/details/byte-magazine-1981-06/page/n393/mode/1up
 * 
 * Parallelized by me
 */

// Long Fixed-Point Division with remainder
static inline word_t lfixdiv(word_t * restrict efrac, size_t current, word_t divisor, word_t remainder)
{
	if(divisor <= 1)
		return 0;

	word_t tmp_partial_dividend;
	efrac[current] = udiv128by64to64(remainder, efrac[current], divisor, &tmp_partial_dividend);
	return tmp_partial_dividend;
}

 static inline void efrac_calc(word_t * restrict efrac, size_t start, size_t end, word_t divisor, word_t * restrict remainders, word_t intensity)
 {
	if(divisor <= 1)
		return;
	for(size_t i = start; i < end; i++)
	{
		for(word_t j = 0; j < intensity; j++)
		{
			remainders[j] = lfixdiv(efrac, i, divisor - j, remainders[j]);
		}
	}
 }

static inline uint64_t computeM_u32(uint32_t d) {
  return UINT64_C(0xFFFFFFFFFFFFFFFF) / d + 1;
}
static inline uint32_t lfixdiv_2mul(uint32_t * restrict efrac, size_t current, word_t divisor, word_t remainder)
{
	uint64_t tmp_partial_dividend = (remainder << 32) | efrac[current];
	efrac[current] = ((__uint128_t)divisor * tmp_partial_dividend) >> 64;
	return tmp_partial_dividend - efrac[current] * divisor;
}

static inline void efrac_calc_2mul(uint32_t * restrict efrac, size_t start, size_t end, uint64_t divisor[], uint32_t * restrict remainders, word_t intensity)
{
	if(intensity == 0) return;

	size_t i;
	for(i = start; i < end-1; i+=2)
	{
		remainders[0] = lfixdiv_2mul(efrac, i, divisor[0], remainders[0]);
		for(word_t j = 1; j < intensity; j++)
		{
			remainders[j-1] = lfixdiv_2mul(efrac, i+1, divisor[j-1], remainders[j-1]);
			remainders[j+0] = lfixdiv_2mul(efrac, i+0, divisor[j+0], remainders[j+0]);
		}
		remainders[intensity-1] = lfixdiv_2mul(efrac, i+1, divisor[intensity-1], remainders[intensity-1]);
	}
	if (i != end) {
		for(word_t j = 0; j < intensity; j++)
		{
			remainders[j] = lfixdiv_2mul(efrac, i, divisor[j], remainders[j]);
		}
	}
}

#define SPIN_TIMES (10)
static inline void ecalc_2mul(uint32_t *efrac, size_t efrac_size, word_t terms, word_t intensity)
{
	size_t maxt = omp_get_max_threads();
	const size_t interthread_buffer = 4;
	uint32_t remainders[maxt*interthread_buffer][intensity];
	uint64_t M[maxt*interthread_buffer][intensity];
	uint32_t sync[maxt];


	memset(remainders, 0, sizeof(remainders));

	if(maxt == 1 || efrac_size < maxt)
	{
		fprintf(stderr, "calculating e with 1 thread\n");
		word_t divisor;
		for(divisor = terms; divisor > intensity; divisor -= intensity)
		{
			for(size_t i = 0; i < intensity; i++)
			{
				remainders[0][i] = 1;
				M[0][i] = computeM_u32(divisor - i);
			}
			efrac_calc_2mul(efrac, 0, efrac_size, M[0], remainders[0], intensity);
			__atomic_fetch_add(&ctr, intensity, __ATOMIC_RELAXED);
		}

		for(size_t i = 0; i < divisor-1; i++)
		{
			remainders[0][i] = 1;
			M[0][i] = computeM_u32(divisor - i);
		}
		efrac_calc_2mul(efrac, 0, efrac_size, M[0], remainders[0], divisor-1);
		__atomic_fetch_add(&ctr, divisor-1, __ATOMIC_RELAXED);

	} else {
		fprintf(stderr, "calculating e with %zd threads, intensity = %zd\n", maxt, intensity);
		memset(sync, 0, sizeof(sync));

		size_t chunk_size = efrac_size / maxt;
		#pragma omp parallel default(shared)
		{
			size_t t = omp_get_thread_num();
			size_t start = t * chunk_size;
			size_t end = (t + 1) * chunk_size;
			uint64_t divisor;
			uint64_t buf_idx = 0;
			if(t == 0) {
				int64_t buffer_size = maxt*interthread_buffer;
				for(divisor = terms; divisor > intensity; divisor -= intensity)
				{
					int64_t csync = sync[t];
					#pragma omp flush(sync)
					for(size_t i = SPIN_TIMES; sync[maxt-1] <= csync - buffer_size; i--) {
						if(i == 0) sched_yield(), i = SPIN_TIMES;
						#pragma omp flush(sync)
					}
					for(size_t i = 0; i < intensity; i++)
					{
						remainders[buf_idx][i] = 1;
						M[buf_idx][i] = computeM_u32(divisor - i);
					}
					efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], intensity);
					buf_idx = buf_idx + 1 == maxt*interthread_buffer ? 0 : buf_idx + 1;
					sync[t]++;
					//ctr += intensity;
				}
				int64_t csync = sync[t];
				#pragma omp flush(sync)
				for(size_t i = SPIN_TIMES; sync[maxt-1] <= csync - buffer_size; i--) {
					if(i == 0) sched_yield(), i = SPIN_TIMES;
					#pragma omp flush(sync)
				}
				for(size_t i = 0; i < divisor-1; i++)
				{
					remainders[buf_idx][i] = 1;
					M[buf_idx][i] = computeM_u32(divisor - i);
				}
				efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], divisor-1);
				sync[t]++;
				//ctr += divisor-1;
			} else if(t == maxt - 1) {

				for(divisor = terms; divisor > intensity; divisor -= intensity)
				{
					uint32_t csync = sync[t];
					#pragma omp flush(sync)
					for(size_t i = SPIN_TIMES; sync[t-1] == csync; i--) {
						if(i == 0) sched_yield(), i = SPIN_TIMES;
						#pragma omp flush(sync)
					}
					efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], intensity);
					buf_idx = buf_idx + 1 == maxt*interthread_buffer ? 0 : buf_idx + 1;
					sync[t]++;
					#pragma omp atomic
					ctr += intensity;
				}
				uint32_t csync = sync[t];
				#pragma omp flush(sync)
				for(size_t i = SPIN_TIMES; sync[t-1] == csync; i--) {
					if(i == 0) sched_yield(), i = SPIN_TIMES;
					#pragma omp flush(sync)
				}
				efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], divisor-1);
				sync[t]++;
				#pragma omp atomic
				ctr += divisor-1;

			} else {
				
				for(divisor = terms; divisor > intensity; divisor -= intensity)
				{
					uint32_t csync = sync[t];
					#pragma omp flush(sync)
					for(size_t i = SPIN_TIMES; sync[t-1] == csync; i--) {
						if(i == 0) sched_yield(), i = SPIN_TIMES;
						#pragma omp flush(sync)
					}
					efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], intensity);
					buf_idx = buf_idx + 1 == maxt*interthread_buffer ? 0 : buf_idx + 1;
					sync[t]++;
				}
				uint32_t csync = sync[t];
				#pragma omp flush(sync)
				for(size_t i = SPIN_TIMES; sync[t-1] == csync; i--) {
					if(i == 0) sched_yield(), i = SPIN_TIMES;
					#pragma omp flush(sync)
				}
				efrac_calc_2mul(efrac, start, end, M[buf_idx], remainders[buf_idx], divisor-1);
				sync[t]++;
				
			}
		}
	}
}

 static inline void ecalc_parallel(size_t efrac_size, word_t * restrict efrac, const word_t intensity, word_t remainders[][intensity], word_t * restrict divisors_pipeline)
 {
	for(size_t i = 0; i < intensity; i++)
	{
		remainders[0][i] = 1;
	}

	#pragma omp parallel shared(remainders, efrac, efrac_size, divisors_pipeline, intensity)
	{
		int t = omp_get_thread_num();
		size_t chunk_size = efrac_size / omp_get_num_threads();
		size_t start = t * chunk_size;
		size_t end = (t + 1) * chunk_size;
		if(t == omp_get_num_threads() - 1)
			end = efrac_size;
		efrac_calc(efrac, start, end, divisors_pipeline[t], remainders[t], intensity);
	}

	#pragma omp barrier
	for(int i = omp_get_max_threads() - 1; i > 0; i--)
	{
		memcpy(remainders[i], remainders[i - 1], sizeof(remainders[0]));
	}
 }

static inline void ecalc(word_t *efrac, size_t efrac_size, word_t terms, word_t intensity)
{
	int maxt = omp_get_max_threads();
	word_t divisors_pipeline[maxt];
	word_t remainders[maxt][intensity];

	// initialize the pipeline
	for(int i = 0; i < maxt; i++)
		divisors_pipeline[i] = 0;

	memset(remainders, 0, sizeof(remainders));

	if(efrac_size < (size_t)maxt)
	{
		fprintf(stderr, "calculating e with 1 thread\n");
		for(word_t divisor = terms; divisor >= intensity; divisor -= intensity)
		{
			for(size_t i = 0; i < intensity; i++)
			{
				remainders[0][i] = 1;
			}
			efrac_calc(efrac, 0, efrac_size, divisor, remainders[0], intensity);
			ctr += intensity;
		}

		word_t remaining_divisor = terms % intensity;

		for(size_t i = 0; i < intensity; i++)
		{
			remainders[0][i] = 1;
		}
		efrac_calc(efrac, 0, efrac_size, remaining_divisor, remainders[0], remaining_divisor);
		ctr += remaining_divisor;

	}
	else
	{
		fprintf(stderr, "calculating e with %d threads, intensity = %zd\n", maxt, intensity);
		// divisor = 1 is impossible as we don't really store the integer part
		for(word_t divisor = terms; divisor >= intensity; divisor -= intensity)
		{
			for(size_t i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = divisor;

			ecalc_parallel(efrac_size, efrac, intensity, remainders, divisors_pipeline);
			ctr += intensity;
		}

		// finish the pipeline
		for(int i = 0; i < maxt; i++)
		{
			for(int i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = 0;
			ecalc_parallel(efrac_size, efrac, intensity, remainders, divisors_pipeline);
		}

		word_t remaining_intensity = terms % intensity;
		for(size_t i = maxt - 1; i > 0; i--)
			divisors_pipeline[i] = divisors_pipeline[i - 1];
		divisors_pipeline[0] = remaining_intensity;

		ecalc_parallel(efrac_size, efrac, remaining_intensity, remainders, divisors_pipeline);
		ctr += remaining_intensity;
	
		fprintf(stderr, "Finalizing...\n");
		// finish the pipeline for remaining divisors
		for(int i = 0; i < maxt; i++)
		{
			for(int i = maxt - 1; i > 0; i--)
				divisors_pipeline[i] = divisors_pipeline[i - 1];
			divisors_pipeline[0] = 0;
			ecalc_parallel(efrac_size, efrac, remaining_intensity, remainders, divisors_pipeline);
		}

	}
}

int main(int argc, char **argv)
{
	word_t intensity = 1;
	if(argc >= 2)
	{
		sscanf(argv[1], "%" SCNu64, &terms);
	}

	if(argc == 3)
	{
		sscanf(argv[2], "%" SCNu64, &intensity);
	}

	if(terms == 0)
	{
		fprintf(stderr, "Invalid term count\n");
		return 1;
	}

	if(intensity == 0 || intensity > terms)
	{
		fprintf(stderr, "Invalid intensity\n");
		return 1;
	}

	double precision = log2fractorial(terms);
	fprintf(stderr, "estimated required precision: log2(%" PRIu64 "!) ~= %lfbits\n", terms, precision);
	
	size_t efrac_size = ceil(precision / WORD_SIZE);
	uint32_t *efrac = calloc(efrac_size, sizeof(word_t));
	fprintf(stderr, "allocated %zd %dbit words (%zu bit)\n", efrac_size, WORD_SIZE, efrac_size * WORD_SIZE);

	// how many decimal digits do we have?
	size_t digits = to_decimal_precision(efrac_size, WORD_SIZE);
	fprintf(stderr, "will print %zu digits\n", digits);
	// set-up timer for progress display
	timer_t timer;
	struct sigevent ev =
	{
		.sigev_notify = SIGEV_THREAD,
		.sigev_notify_function = display,
		.sigev_notify_attributes = NULL
	};
	timer_create(CLOCK_MONOTONIC, &ev, &timer);

	struct itimerspec period =
	{
		.it_value.tv_sec=1,
		.it_interval.tv_sec=1,
		.it_interval.tv_nsec=000000000L
	};

	ctr = 0;
	timer_settime(timer, TIMER_ABSTIME, &period, NULL);
	// calculate e
	//ecalc(efrac, efrac_size, terms, intensity);
	ecalc_2mul(efrac, efrac_size*2, terms, intensity);

	// swap higher and lower bits
	for(size_t i = 0; i < efrac_size*2; i+=2)
	{
		uint32_t z = efrac[i];
		efrac[i] = efrac[i+1];
		efrac[i+1] = z;
	}

	putc('\n', stderr);

	// Print the result
	secs = 0;
	ctr = 0;

	fprintf(stderr, "Printing...\n");
	printf("e = 2.");
	print_fraction(efrac, efrac_size, digits, intensity);
	putchar('\n');

	timer_delete(timer);
	free(efrac);
	return 0;
}
