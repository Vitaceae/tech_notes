---
title: "C/C++ Programming - C Traps and Pitfalls"
tags: [programming, c, cpp]
draft: false
date = "2026-10-13"
---

# C Traps and Pitfalls

各種 C 語言陷阱與誤用

----
## Pointers

### Function Pointer 就是記憶體中的位置?

C 依據變數型態位移記憶體位址  
例如 int 指標遞增 n 時位移量為 4n bytes, 再使用運算子 \* 取值  
若非平面記憶體模型 (flat memory model)，即記憶體非連續表格時，假設不一定成立。  

### null pointer 不一定為 0

以下架構 Null pointer 不為 0

+ Prime 50 series(PL/I)
    + 使用 segment 07777, offset 0
+ Honeywell-Bull mainframes
    + 使用 bit pattern 06000
+ CDC Cyber 180 Series
    + 使用 0xB00000000000
+ old CDC 1's complement 機種
    + 使用 all-one-bits word 表示所有種類資料, 包含無效位址
+ Symbolics Lisp
    + tagged architecture
    + 不存在 numeric pointers, 使用 <NIL, 0> (不存在的 <object, offset> handle)

### pointer type mismatch errors

+ word-addressed Prime machines
    + byte pointer 大於 word pointer
+ Data General 的 Eclipse MV series C compiler
    + 用 byte pointers 表示 char \* 和 void \*
    + 用 word pointers 表示其餘
+ 16-bit Nova 演進至 32-bit MV line 時 word/byte pointers 包含 offset, indirection, ring protection bits
+ CDC Cyber 180 Series
    + 48-bit pointers 含有 ring, segment, and offset
+ old HP 3000 series
    + byte/word addresses 的 addressing scheme 不同
    + char \* 和 void \* 表示法不同
+ 8086-family processors 使用 16-bit data pointers 和 32-bit function pointers  
+ 部分 64-bit Cray machines 中 word 的低 48 位表示 int \*, 高 16 位表示 char \*

### 指標都能轉型成 void * 或 char *?

+ refer to `N1256 6.3.2.3`
+ `*` 只保證做為物件指標(object/data pointer); 將函式指標轉型成 `void *` 不保證 portable
    + 某些平台函數地址長度可能大於物件指標
+ 不同型態的 function pointer 保證可互相轉型, 如 `int (*)()` 和 `void (*)()`
+ 考量可攜性, 若一定要交換存放函數/物件指標, 建議使用 union

### 指標必定可轉型成 int, long, intptr_t 或 uintptr_t?

+ refer to `N1256 6.3.2.3`
+ 即便非函式指標也不保證正確; 轉型後數值未必能直接讀取的情形:
    + 指標長度大於數值, 如 64 位元指標
    + AMD64 指令集下, Linux/Mac OS/Solaris/FreeBSD 等採用 LP64 模型裡的 int
    + MS Windows X64 採用 LLP64 模型裡的 int 和 long
+ 可攜轉型方式: 轉型成 void \* (函式指標可能失敗) 再轉型成 intptr_t 或 uintptr_t.
    + ```c
      //函式指標可能失敗
      int val;
      int *p = &val;
      //轉型為 intptr_t
      intptr_t i = (intptr_t)(void *)p;
      //轉型回 int *
      int *pi = (int *)(void *)i;
      ```
    + 不保證提供 intptr_t
    + 部分平台上指標不以數字表示
    + ref: N1256 7.18.1.4

### 指標 p1 == p2 轉型成 int 後其值也相等?

不保證; 建議轉型回原始指標型態再比較  
例如 [tagged-pointers](https://source.android.com/docs/security/test/tagged-pointers?hl=zh-tw)  
ARM 的 TBI, Top-byte Ignore 功能忽略指標的頂端位元組  

### 物件指標轉型成不同物件型態必定可存取?

+ `Type-based aliasing rules`: 轉型成功不保證可存取
    + 例如 short 指標不能指向 int 物件
+ 編譯器最佳化時調整運算順序, 使不同型態的存取指向不同物件, 使結果不如預期  
  ```c
    int i = 0;
    short *s = (short *)&i;
    i = 0;
    *s = 1; //i may eventually still be 0
  ```
    + ref: N1256 6.5
    + [Linus Torvalds 的抱怨](https://lwn.net/Articles/316126/)  
      ```c
        unsigned long a;
        a = 5;                      //gcc 後執行這行指令, 使下一行失效
        *(unsigned short *)&a = 4;  //gcc 先執行這行指令
      ```
    + gcc 可添加 `-fno-strict-aliasing` 避免上述行為

### printf("%p", p) 必定印出函數位置?

+ printf 系列的 %p 只能列印指向 void 的指標, 故須先轉型成 void \*
    + 函式指標不一定能轉型
    + 建議使用  
      ```c
      printf("%p", (void*)p);
      ```
      或轉型成 intptr_t
+ 指標列印結果視實作而定, 不一定是十六進位
+ ref: N1256 7.19.6.1

### NULL 型態是空指標?

+ NULL 是`空指標常數`, 但不保證是`空指標`
    + 空指標常數保證可轉型成正確的空指標  
      ```c
      printf("%p", NULL);         //不符合標準(NULL 非空指標)
      printf("%p", (void *)NULL); //符合標準(NULL 轉型成指標時必為空指標)
      ```
    + 空指標常數代表計算結果為零的整數常數表達式: 0, 0ULL
    + 空指標常數**也可能**是常數表達式的轉型如 (void *)(0U)
    + 使用 NULL 時(尤其是 C++)建議先轉型成適當型態如 (int \*)NULL
    + C99 規定**空指標與空指標常數比較結果相等**, 也可**指派空指標常數給一個指標** (編譯器會在編譯期適當轉換)
        + 使用 NULL 替代 0 不會增加可攜性, 多出於寫作風格與除錯目的
        + C99 中**常數零一定是合法的空指標常數**, **一定可以轉型成空指標**, **一定可以和其他指標比較**
+ C++03 中空指標常數只能是計算結果為零的整數常數表達式, overloading 時可能出現錯誤
    + 編譯器不確定是整數 0 或 null pointer
    + C++0x 引入 `nullptr` 以解決這個問題
    + ref: [More C++ Idioms/nullptr](http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/nullptr)

### 執行期的數值零必定可轉型為空指標?

```c
int x = 0;
int *p = (int *)x;  //not guaranteed to be a NULL pointer
```

+ 並非所有平台上的空指標都是零
+ [comp.lang.c FAQ list · Question 5.18](https://c-faq.com/null/runtime0.html): 若允許執行期轉換空指標, 需要檢查常數值是否為零

### 全域變數 int *p; 初始值為何?

C99 保證初始化成空指標 (N1256 6.7.8)

### 相同型態的指標相減代表距離?

一般指向同一陣列的元素指標或該陣列末元素 +1 的位置才能相減比較  
若陣列太大差值 ptrdiff_t 可能溢出, 需檢查 `PTRDIFF_MAX`, `PTRDIFF_MIN`  

### 陣列常自動轉型成指標, 將 int x[] 理解成 int * const 會產生問題?

```c
//file_1.c
int x[10];

//file_2.c
extern int * const x;   //C99 不要求編譯器報錯; 部份實作於此 scope 中存取 *x 時會計算 *(int *)(x[0])
```

----
## Array 陣列

### 陣列索引使用負值存取結構內容 .x[-1]?

```c
struct {
    int heap[1024]; // 依結構內對齊方式, 成員中間可能存在 gap
    int stack[1];   // 存取陣列時先運算指標再 dereference, 只允許在陣列範圍及範圍+1間移動
                    // 符合標準的編譯器可檢查陣列範圍
};

.stack[-1]  // 不合法; 存取陣列時先運算指標再 dereference, 只允許在陣列範圍及範圍+1間移動
            // 符合標準的編譯器可檢查陣列範圍
```

### 陣列 x 型態 int[5][5], 可以 x[1][10] 存取?

即便陣列連續, 也不保證成功; 指標運算後解參照, 等價於 \*( \*(a + 1) + 10 ) 違反指標移動範圍限制  
標準允許實作檢查指標是否超過範圍或用不同方法來表示陣列 (N1256 Annex J.2 Undeﬁned behavior)  

----
## 整數

### Trap representation, 陷阱表示法

+ 陷阱表示法: 並非所有位元組合都表示合理數字, 存取特定位元組合在特定平台上可能造成錯誤
    + 除非使用位元運算或違反標準規定(如溢位), 一般運算不可能被 trap
    + 標準允許實作決定以下狀況是否表示陷阱 (6.2.6.2 Integer types Footnotes 44, 45)
        + 有號整數的 sign/value 位元為特定組合
        + padding 為特定組合
    + 位元運算會忽略 padding bits, 所以**無號整數可放心使用位元運算**, 有號整數不建議使用位元運算
+ 整數表示法包含的位元類型 (N1256 6.2.6.2)
    + 無號整數: value, padding
    + 有號整數: value, padding, sign
+ sign/value 格式
    + 2's complement
    + 1's complement
    + sign and magnitude
+ 若實作提供 `uintN_t` 和 `intN_t` 時保證以下性質 (N1256 7.18.1.1)
    + uintN_t 和 intN_t 沒有 padding bits
    + intN_t 必為 2's complement
    + intN_t 不可能有陷阱表示法

### int 為 32-bit? short 為 16-bit? long long 為 64-bit?

+ 標準僅規定 int 需滿足特定數值範圍
    + 除寬度外可能還有 padding bits
    + int32_t 保證寬度為 32-bit
+ 即便 sizeof(int) 為 4 或確定佔 32 位元, 也**不保證 int 可以儲存此範圍上限的整數**
    + 位元組不一定是 8-bit
    + 可能有 padding bits
    + 格式可能不是 2's complement
    + 特定組合可能是陷阱表示法
    + `int32_t` 保證沒有以上問題, 但 **sizeof(int32_t) 不一定為 4**

### int 至少 32-bit?

+ int 只保證整數範圍**至少**為 -32767 到 32767, 使用 16-bit 已足夠
+ `int_least32_t` 和 `int_fast32_t` 保證範圍下限 32-bit, 但不保證是沒有陷阱表示法的 2's complement
+ 一般使用 `INT_MAX` 和 `INT_MIN` 確認範圍

### malloc(300 * 300 * sizeof(double)) 可能緩衝區溢位?

+ 300 \* 300 可能超過 INT_MAX
+ 300 \* 300 \* sizeof(double) 可能超過 SIZE_MAX 或 INT_MAX (依實作決定轉型成何者)
+ 建議使用型態 size_t 或範圍更大的無號整數, 且確保運算結果不超過 size_t 範圍  
  ```c
    if (SIZE_MAX / 300u / 300u < sizeof(double)) {
        p = NULL;
    } else {
        p = malloc(sizeof(double) * 300u * 300u) ;
    }
  ```

### 整數 i 是否能用 memset(&i, 0, sizeof(i)) 清零?

標準向實作妥協, **只有 0 可以這樣操作** (N1256 6.2.6.2)

### 相同整數型態 a 和 b 可否如此互換: a^=b; b^=a; a^=b; ?

不保證正確; a^b 可能產生陷阱表示法

### unsigned int x, 且 x 寬度為 32, (a << 32) 結果必為 0?

有些處理器結果為 x, 因 (x << i) 被解釋為 (a << (i % 32)) (N1256 6.5.7)

### 如何分辦 little-endianness 和 big-endianness

平台可能不使用以上兩者, 沒有簡單可攜式寫法可以判斷  
思考是否真有必要判斷; 網路傳輸可使用系統提供的轉換函數 (middle-endian, bi-endian)

### 80000U 必為 unsigned int?

+ 若 unsigned int 範圍只能表示到 65535, 會變成 unsigned long
+ 0x33FF 一定是有號整數, 因為可以 int 表示
+ 0xFFFFFF 則不一定是有號整數, 因為超出標準範圍
+ 若 int 表示上限為 2147483647, 2147483648 不會轉型成 unsigned int <!--tag: long?-->
+ 規則 (N1256 6.4.4.1, C++03 2.13.1, C++1x N3242 2.14.2)
    + signed: 十進位無後綴 u, U 
    + unsigned: unsigned-suffix u, U
    + both: octal/hexadecimal 無後綴 u,U
    + Lv2 起算: 後綴 l, L
    + Lv3 起算: 後綴 ll, LL
    + 選擇符合等級與數值範圍要求欄位中最左方者
    + C++03 未規範 Lv3, Lv4; C++1x 同 C99
        |        |   |Lv1         |    |Lv2          |         |Lv3                |Lv4                  |unspecified|
        |:-------|:-:|:----------:|:--:|:-----------:|:-------:|:-----------------:|:-------------------:|:---------:|
        |signed  |int|            |long|             |long long|                   |extended type(if any)|?          |
        |unsigned|   |unsigned int|    |unsigned long|         |unsigned long  long|extended type(if any)|?          |
        |both    |int|unsigned int|long|unsigned long|long long|unsigned long  long|extended type(if any)|?          |

----
## Execution Order 運算順序: 最佳化, 表達式

### x++; y++; 中 x 一定先被運算?

+ 編譯器可能為了最佳化改變運算順序, 讓編譯器判斷為不相關的運算交錯進行 (N1256 5.1.2.3)
+ C99 規範分號處有 sequential points(循序同步點), 副作用必須完結, 但屬於**抽象機器行為**,
    + 編譯器不必維持真實機器與抽象機器間的對應關係 (N1256 5.1.2.3) <!--tag: read-->

### int a; a = b / c; fprintf(stderr, "done\n"); 輸出訊息時 a 已更新?

+ 因 fprintf 未存取 a, 所以編譯器可能改變運算順序, 輸出後才運算 a (N1256 5.1.2.3)
+ 改寫 fprintf(stderr, "done: %d\n", a); 也未必循原始碼順序執行, a 值可能從暫存器直接傳給 fprintf (N1256 5.1.2.3)
    + 不同的最佳化設定可能導致不同的運算順序
    + 思考如何保證不受實作或最佳化影響
+ 即使運算順序如同原始碼, 實際執行時也可能被現代處理器打亂
    + 處理器為了加速同時執行複數指令或多層快取; ref: `out-of-order execution`

### 設計非同步訊號如何阻止編譯器改變運算順序?

+ 編譯器大多時刻都能改變運算順序或交錯運行
+ 可能解決方案
    + volatile
    + sig_atomic_t
+ C1X 可能提供更多控制機制
    + [glibc 24.4.7: Atomic Data Access and Signal Handling](https://www.gnu.org/software/libc/manual/html_node/Atomic-Data-Access.html#Atomic-Data-Access)
    + [Wiki: Memory barrier](http://en.wikipedia.org/wiki/Memory_barrier)
    + [gcc 6.40: When is a Volatile Object Accessed](https://gcc.gnu.org/onlinedocs/gcc/Volatiles.html)
    + N1256 5.1.2.3, 6.7.3, 7.14.1.1

### int i = f() + g(); 哪個函式會先被呼叫?

不一定 (N1256 6.5)

### int i = i++; 及 a[i] = i++; 和 f(i++, i++) 結果為?

**嚴重錯誤**  
1. 缺少 sequential points 保護, 修改 i 值兩次 (N1256 6.5p2)
2. a[i] 讀取 i 的值並非為了計算 i 的新值 (N1256 6.5p2)

>   ```c
>   struct _str {
>       size_t length;
>       char data[0];
>   };
>   struct _str *str = (struct _str *) malloc(sizeof(struct _str) + 100);
>   ```
>   陣列長度為 0 是否有問題?

+ C99 及 C++03 都不允許宣告大小為 0 的陣列, 上述用法僅適用特定編譯器上
+ C99 允許 struct 最後成員是大小未知的陣列(incomplete array type) (N1256 6.7.2.1p18)(gcc 6.17: Arrays of Length Zero)  
  ```c
    struct _str {
        size_t length;
        char data[];
    };
  ```

### 使用相同結構型態的指標直接參照存取?

```c
    struct t1 { int m; } a;
    struct t2 { int m; } *b;
    b = (struct t2 *)&a;
```

即使結構成員相同也不保證可用 b 去存取 a  
即使所有已知的編譯器似乎都使用相同擺放方式  
標籤不同, 結構型態就不相同也不相容  
編譯器最佳化時可能假設 b 指向的物件和 a 不同(型態不同), 改變運算順序  

```c
    b->m = 0;
    //不保證為 1 (N1256 6.2.7p1)
    a.m = 1;
```

ref. N1256 6.5.2.3p5

### union 初始化會被清空?

```c
    union {
        char c;
        int i;
    } u;
```

union 只有第一個成員的值保證會被清空(不一定為 0; 不同型態的 '0' 可能有不同表示法) (N1256 6.7.8p10)

### char 是有號數還是無號數?

標準並未規定;  
char, signed char 及 unsigned char 應視為三個不同型態 (N1256 Footnote 35)  
標準僅規定 char 的表示法, 值範圍及行為必需和 signed char 或是 unsigned char **其中之一** 相同

### char 必為 8-bit?

char 長度至少 8 位元, 但不一定剛好 (N1256 5.2.4.2.1p1)  
char 長度 16 位元不代表佔兩個位元組(bytes)  
除了 bit field 特例, char 是最終記憶體單位, **sizeof(char) 必為 1**
[comp.lang.c FAQ list · Question 8.10](https://c-faq.com/charstring/wchar.html): 標準定義 `wchar_t` 以處理寬字元  

### getchar() 溢出

```c
char c1;
while ((c1 = getchar()) != EOF) {}

int c2;
while ((c2 = getchar()) != EOF) {}
```

1. getchar() 回傳 int, char 可能裝不下 EOF  
2. 當 int 只佔一個位元組時可能裝不下所有字元外加 EOF  
3. EOF 是負值, 但不一定是 -1, 建議比較回傳值與 EOF (N1256 7.19.1p3)

### 字元編碼

C 標準並未規定字元編碼為 ASCII  
C99 僅列舉一些基本字元, 表示編碼必須以一個字元容納 (N1256 5.2.1)  
早期某些平台使用 ISO 646 或 EBCDIC 編碼, 所以 C 標準制定 trigraph  
[Wiki: Digraphs and trigraphs](http://en.wikipedia.org/wiki/Digraphs_and_trigraphs#C)  
'a' ~ 'z' 或 'A' ~ 'Z' 依採用的編碼而異, 不保證連續(e.g. EBCDIC)

### printf: right to left?

```c
volatile int x = 20;

printf("%d %d\n", x, x++);

x = 20;
printf("%d %d\n", x++, x);

x = 20;
printf("%d %d %d ", x, x++, ++x);

//result:
//  21 20
//  20 20
//  22 21 21
```

----
## 浮點數

標準並未強制規定浮點數格式, 若定義 `__STDC_IEC_559__` 則要遵照 IEC 60559:1989 標準(同 ANSI/IEEE 754-1985) (N1256 Annex F):  
+ float, double 符合定義的單精倍浮點數和雙精倍浮點數
+ long double 未強制規定 (N1256 Annex F.2)

浮點數組合運算

```c
a = b;
a += c;
a += d;
```

不保證等價

```c
a = b + c + d;
```

(N1256 5.2.4.2.2p8)  
計算 b + c + d 時可能使用精度更高的浮點數型態; 存回 a 時可能喪失額外精準度  
編譯器若遵守標準, 不會將上例組合運算最佳化成一行  
部分平台內建浮點數指令與專屬暫存器, 計算時為了符合標準而降低精度, 運算可能變慢  

若浮點數滿足 IEC 60559 標準  
a == a 未必為真(a 為 NaN 時)  

```c
if (a < b) f();
else g();
```

因可能存在 NaN, 未必等價  

```c
if (a >= b) g();
else f();
```

### 轉換浮點數成最接近的整數

+ C99 使用 lround 系列 (N1256 7.12.9.7)
+ clc FAQ 的建議(可能不可靠?)  
  ```c
  int xi = (int)(x < 0 ? x - 0.5 : x + 0.5);
  ```

----
## Memory Access Violations

### double free

case 1

```c
    int a = 20;
    int *p = (int *)malloc(sizeof(int));

    //some excecutions ...
    if (a > 0) {
        free(p);
    }
    //some excecutions ...
    free(p);    //double free
```

case 2

```c
void foo(int** arr)
{
    free(*arr);
}

int main(int argc, char* argv[])
{
    int *arr = malloc(sizeof(int) * 10);
    foo(&arr);
    free(arr);  //double free
    return 0;
}
```

### free-after-free

```c
    a = malloc(8);  //suppose allocated to 0x00
    b = a;          //b points to 0x00
    free(a);
    a = malloc(8);  //re-allocated to 0x00
    free(b);        //freed 0x00 but a is still in scope
```

### Use-after-return

+ dereference of deallocated stack object

```c
char *p;

void foo(void)
{
    char buf[8];
    x = &buf[2];
}

int main()
{
    foo();
    *x = 42;    //use-after-return
    return 0;
}
```

----
## Others

### main() 可以遞迴?

C 未規範  
C++03 規定不行(C++03 3.6.1p3, 5.2.2p9)  

### main 可以回傳 void?

C 允許實作自己增加新的形式 (N1256 5.1.2.2.1p1)  
C++ 要求回傳 int (C++03 3.6.1p2)  
C99 和 C++03 可以不回傳值 (C90 規定不同)  
為了可攜性建議加上 return 0 或 return EXIT_SUCCESS (N1256 5.1.2.2.3, C++03 3.6.1p5)

### sizeof

```c
printf("%d, %d\n", sizeof('J'), sizeof(char));

//ANSI C: 4, 1
//  sizeof('J'): default to int; equivalent to 'sizeof(int)'
//               multi-character 'sizeof(L'J')' may behaves differently
//  sizeof(char): 1 size_t  
//Cpp: 1, 1
//  sizeof('J'): equivalent to 'sizeof(char)'

struct Empty { } empty;
printf("sizeof(empty) = %d\n", sizeof(empty));

//ANSI C: 0
//Cpp: 1
//  C++ standards prohibits 'zero-sized addressable objects'; 'freestanding objects' MUST be of non-zero size
```

----
## References

ISO/IEC 9899:TC3(n1256.pdf)  
