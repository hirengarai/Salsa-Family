#include <stdio.h>
#include <time.h>

int main()
{
    double sum = 0;
    double add = 1;

    // Start measuring time
    clock_t start = clock();

    int iterations = 1000 * 1000 * 1000;
    for (int i = 0; i < iterations; i++)
    {
        sum += add;
        add /= 2.0;
    }

    // Stop measuring time and calculate the elapsed time
    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    printf("Result: %.20f\n", sum);

    printf("Time measured: %.3f seconds.\n", elapsed);

    return 0;
}