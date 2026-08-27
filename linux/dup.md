# Linux dup

![dup illustrate](inc/dup-illustrate-1.png)

![dup illustrate](inc/dup-illustrate-2.png)

----
## printf not direceted to console

root cause:  
1. stdout has been redirected
2. output buffer not flushed
    1. fflush(stdout)
    2. disable buffer
       setvbuf(stdout,NULL,_IONBF,0);

[dup_redirect_printf-1.c](inc/dup/dup_redirect_printf-1.c)  
[dup_redirect_printf-2.c](inc/dup/dup_redirect_printf-2.c)  

----
# Restore stdout to Terminal

```c
#include <unistd.h>

int saved_stdout;

saved_stdout = dup(1);
dup2(my_temporary_stdout_fd, 1);

//do something

dup2(saved_stdout, 1);
close(saved_stdout);
```