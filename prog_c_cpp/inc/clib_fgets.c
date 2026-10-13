#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void main(void)
{
    char buf[40] = {0};

    memset((void *)buf, 0, sizeof(buf));
    printf("input string:\n");
    //read string
    if (fgets(buf, 40, stdin)) {
        //overwrite '\n' with 0, if any
        buf[strcspn(buf, "\n")] = 0;
    }
    printf("read string: %s\n", buf);

    memset((void *)buf, 0, sizeof(buf));
    printf("input number:\n");
    //read numbers
    if (!fgets(buf, 40, stdin)) {
        return;
    }
    printf("read number: %d\n", atoi(buf));

    //error checking
    char *endptr;
    errno = 0;
    long b = strtol(buf, &endptr, 10);

    if (errno == ERANGE) {
        //value out of range
    } else if (endptr == buf) {
        //no char read
    } else if (*endptr && *endptr != '\n') {
        //invalid char at the end of input string
    } else {
        //success
    }
}
