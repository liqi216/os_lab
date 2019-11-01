#include <linux/unistd.h>

#define __NR_claimed 341
#define __NR_free 342

extern long int syscall (long int __sysno, ...) __THROW;
