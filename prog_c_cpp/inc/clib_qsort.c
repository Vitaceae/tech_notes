#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARR_SIZE(x)                     (sizeof(x) / sizeof(x[0]))

int cmp_val(const void *a, const void *b)
{
    int c = *(int *)a;
    int d = *(int *)b;

    if (c < d) {
        return -1;
    } else if (c == d) {
        return 0;
    } else {
        return 1;
    }
}

int cmp_str(const void *a, const void *b)
{
    return(strcmp(*(const char **)a, *(const char **)b));
}

void print_int_arr(int *arr, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print_str_arr(char **arr, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");
}

void main(void)
{
    int a[10] = {0}, i;
    char *b[] = {"gdfa", "brt", "resv", "luhvwoaih", "agfew"};

    srand((unsigned) time(NULL));
    for (i = 0; i < ARR_SIZE(a); i++) {
        a[i] = rand();
    }

    printf("unsorted array: ");
    print_int_arr(a, ARR_SIZE(a));
    qsort((void *)a, ARR_SIZE(a), sizeof(a[0]), cmp_val);
    printf("sorted array: ");
    print_int_arr(a, ARR_SIZE(a));

    printf("unsorted str: ");
    print_str_arr(b, ARR_SIZE(b));
    qsort((void *)b, ARR_SIZE(b), sizeof(b[0]), cmp_str);
    printf("sorted str: ");
    print_str_arr(b, ARR_SIZE(b));
}
