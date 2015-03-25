// Common typedefs and global functions

#ifndef COMMON_H
#define COMMON_H

// NULL
#define NULL ((void *)0)

// standard sizes
// for x86
typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;
typedef unsigned long  size_t;
typedef          long  ssize_t;

// page size and mask for checking page alignment
#define PAGE_SIZE 0x1000
#define PAGE_MASK (~(PAGE_SIZE - 1))

// warn about unused results
#define WARN_UNUSED __attribute__((warn_unused_result))

#endif // COMMON_H
