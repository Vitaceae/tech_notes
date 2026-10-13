---
title: "C/C++ Programming - C Sundries"
tags: [programming, c]
draft: false
date = "2026-10-12"
---

# C Sundries

關於 C 的各式雜談。

----
## File Processing 檔案處理

create directory

```c
mkdir(dir_name, 0777);
```

remove file

```c
remove(file_name);
```

set file permission  
+ umask(x) sets system umask value to (x & 0777)
    + real permission: (mode & ~umask)
    + example:  
      mode = 0666  
      default umask = 022  
      real permission = (0666 & ~022) = 0644, or rw-r--r--

```c
#include <sys/stat.h>

mode_t umask(mode_t mask);
```

----
## Use fgets() over scanf()

1. scanf() 使用者不需管理 buffer size (預設值 100 bytes?), 但可能導致 buffer overflow 或 stack overflow
2. scanf() 惡意使用者可能插入 function pointer 或 executable code
3. fgets() 相對強健(robust), \*nix 系統上的資料來源可以是 sockets, streams 或實際檔案
4. fgets() 需指定緩衝區長度, 可避免溢出式攻擊
5. fgets() I/O 問題可使用 ferror() 做為 error-handling

----
## Clear Input Buffer

```c
//platform-dependent
fseek(stdin, 0, SEEK_END);

//portable
char c = NULL;
while ((c = getchar()) != '\n' && c != EOF) {;/*do nothing*/}
```

----
## sscanf

```c
/* Prototype */
int sscanf(const char * str, const char * format, ...);

//string
sscanf("name:john age:40 tel:082-313530", "%s", res);

//string with length
sscanf("name:john age:40 tel:082-313530", "%8s", res);

//regex
//res=name
sscanf("name:john age:40 tel:082-313530", "%[^:]", res);
//res=name john
sscanf("name:john age:40 tel:082-313530", "%[^:]:%s", field, res);

//mixed types
//res=john res2=40 res3=082-313530
sscanf("name:john age:40 tel:082-313530", "name:%s age:%d tel:%s", res, &res2, res3);
sscanf("name:john age:40 tel:082-313530", "%*[^:]:%s %*[^:]:%d %*[^:]:%s", name, &age, tel);
//protocol=http site=ccckmit.wikidot.com path=cp/list/hello.txt
sscanf("http://ccckmit.wikidot.com/cp/list/hello.txt", "%[^:]:%*2[/]%[^/]/%[a-zA-Z0-9._/-]", protocol, site, path);
```

----
## System Call

```c
/* Prototype */
int system(const char *command);

void example(void)
{
    //echo the parent path of "/home/user/work", which is expected to be "/home/user/"
    char cmd[64] = {0};
    sprintf(cmd, "dirname %s | xargs mkdir -p", "/home/user/work";
    system(cmd);
}
```

----
## Function pointer

Function pointer as input parameter

```c
void (*foo[10])(void (*)());

/* equivalent */
typedef void (*fptr_param)();
typedef void (*fptr)(fptr_param);
fptr foo[10];
```

----
## Calculate Hash: MD5, Message Digest Algorithm 5


```bash
$ sudo apt install libssl-dev
```

使用 openssl 計算字串 MD5 值: [calc_md5_openssl.c](inc/calc_md5_openssl.c)

實作 MD5 函式手動計算字串 MD5 值: [calc_md5_manual.c](inc/calc_md5_manual.c)

```makefile
all:
	gcc foo.c -l crypto -o bar
```

ref: [Wiki: MD5](https://en.wikipedia.org/wiki/MD5)

----
## inline

1. should be put in the header file
    + the scope of static function is within the `translation unit`(.c and all the included .h)
2. use 'static inline' instead of 'inline'
    + Guarantee that GNU will optimize it

>   When an inline function is not static, then the compiler must assume that  
>   there may be calls from other source files; since a global symbol can be  
>   defined only once in any program, the function must not be defined in the  
>   other source files, so the calls therein cannot be integrated. Therefore,  
>   a non-static inline function is always compiled on its own in the usual  
>   fashion.

----
# sizeof

+ not a function
+ a unary operator? NO!(does not support chaining operation)
+ determined at compile time
    + sizeof(int): length of 'int' is 4
    + sizeof(1 == 2): length of 'bool' is 1
    + sizeof(a = 3): length of 'type of lvalue' is 'int', or 4
    + sizeof("abcd"): length of string is 5

```c
int f1() {return 0;};
double f2() {return 0.0;}
void f3(){};

//sizeof(f1()): int 
//sizeof(f2()): double
//sizeof(f3()): ERROR; sizeof(void) is invalid
//sizeof(f1): ERROR; sizeof(function pointer) is invalid
//sizeof *f2: sizeof(f2())
//sizeof(int*): on a 32-bit machine the address length is 4
```

----
# Undefined Behavior and Unspecified Behavior

UB, Undefined Behavior  
+ program behavior not defined in standards(ISO/IEC 9899...)
+ compiler optimizes the UB in low-level language  
    ```c
    //rvalue may be optimized to be 'x' if there's no overflow
    x * 2 / 2
    ```

Unspecified Behavior example  
    ```c
    void func(int *x) {return *x;}
    int n = 0;

    //depends on ABI calling conventions(void value)
    printf("%d %d %d\n", n++, n++, n++);
    printf("%d %d %d\n", func(&n), func(&n), func(&n));
    ```

----
## Bit Operation

```c
//query bit state: bitwise AND
if (flags & bit_mask) {...}

//turn bits on: bitwise OR
flags |= bit_mask;
flags |= (bit_mask_1 | bit_mask_2);

//turn bits off: bitwise AND with inverse bit
flags &= ~bit_mask;
flags &= ~(bit_mask_1 | bit_mask_2);

//toggle bit states: bitwise XOR
flags ^= bit_mask;
flags ^= (bit_mask_1 | bit_mask_2); //flip bit_mask_1 and bit_mask_2
```

----
## References

[Open Standards](http://open-std.org/)  
[Open Standards: C - Approved standards](http://www.open-std.org/jtc1/sc22/wg14/www/standards)  
