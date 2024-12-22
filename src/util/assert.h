
#ifndef _assert
#define _assert

#include <stdio.h>
#include <stdlib.h>

#define assert(x) if (!(x)) abort();
#define assertWithReturn(x, v) if (!(x)) return v;
#define assertWithLog(x, t) if (!(x)) {printf("错误: %s\n", t); abort();}
#define assertWithWarn(x, t) if (!(x)) printf("警告: %s\n", t);
#define assertWithLogReturn(x, t, v) if (!(x)) {printf("错误: %s\n", t); return v;}

#endif /* _assert */