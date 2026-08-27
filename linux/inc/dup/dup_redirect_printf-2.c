#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    fflush(stdout);
    setvbuf(stdout,NULL,_IONBF,0);
    printf("test stdout\n");
    freopen("foo-1.txt","w",stdout);
    //do NOt use 'stdout = fopen("foo-1.txt","w");'
    //use freopen instead
    printf("test file\n");
    freopen("/dev/tty", "w", stdout);
    printf("test tty\n");
}
