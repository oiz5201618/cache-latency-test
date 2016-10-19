#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>

#define SAMPLE_NUM 100000

uint64_t overhead_count();
uint64_t Confidence_interval_67(uint64_t rd_overhead[]);

int main(int argc, char const *argv[]){

    int data_size = atoi(argv[0]);
    int data_length = data_size / sizeof(double);

    double data_array[data_length];

    uint64_t rdtsc_overhead = overhead_count();

    printf("%" PRIu64 "\n",rdtsc_overhead);

    return 0;
}

uint64_t overhead_count(){

    uint32_t upper, lower, temp1, temp2;
    uint64_t overhead[SAMPLE_NUM];

    int i;

    for(i = 0; i < SAMPLE_NUM; i++) {

        asm volatile(
            "mfence\n\t"        // memory fence
            "rdtsc\n\t"         // get cpu cycle count
            "mov %%edx, %2\n\t"
            "mov %%eax, %3\n\t"
            "mfence\n\t"        // memory fence
            "mfence\n\t"
            "rdtsc\n\t"
            "sub %2, %%edx\n\t" // substract cycle count
            "sbb %3, %%eax"     // substract cycle count
            : "=a" (lower) // a for eax
            , "=d" (upper) // d for edx
            , "=r" (temp1)
            , "=r" (temp2)
        );

        overhead[i] = ((uint64_t)upper << 32) | lower;

    }

    return Confidence_interval_67(overhead);
}

uint64_t Confidence_interval_67(uint64_t overhead[]) {
    double mean = 0;
    double SD = 0; // Standard Deviation
    double Low_End, Up_End;
    double final_array[SAMPLE_NUM];
    double result = 0;
    int final_array_length = 0;
    int i;
    //compute mean
    for(i = 0; i < SAMPLE_NUM; i++) {
        mean += overhead[i];
    }
    mean = mean / SAMPLE_NUM;

    //compute Standard Deviation
    for(i = 0; i < SAMPLE_NUM; i++) {
		SD += (overhead[i] - mean) * (overhead[i] - mean);
    }
    SD = SD / SAMPLE_NUM;
    SD = sqrt(SD);

    //find Lower-Endpoint & Upper-Endpoint
    Low_End = mean - 3 * SD;
    Up_End = mean + 3 * SD;

    //delete points out of range
    for(i = 0; i < SAMPLE_NUM; i++) {
        if(overhead[i] >= Low_End && overhead[i] <= Up_End) {
            final_array[final_array_length] = overhead[i];
            final_array_length++;
        }
    }

    //compute result
    for(i = 0; i < final_array_length; i++) {
        result += final_array[i];
    }

    result = result / final_array_length;

    return result;

}
