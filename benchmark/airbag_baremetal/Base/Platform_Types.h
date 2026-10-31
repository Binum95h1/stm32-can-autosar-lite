#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#include <stdint.h>

typedef uint8_t  boolean;
typedef int8_t   sint8;
typedef uint8_t  uint8;
typedef int16_t  sint16;
typedef uint16_t uint16;
typedef int32_t  sint32;
typedef uint32_t uint32;
typedef int64_t  sint64;
typedef uint64_t uint64;

typedef float    float32;
typedef double   float64;

#ifndef TRUE
#define TRUE  ((boolean)1U)
#endif
#ifndef FALSE
#define FALSE ((boolean)0U)
#endif

#define CPU_TYPE_32 32U
#define CPU_TYPE    CPU_TYPE_32
#define CPU_BIT_ORDER_LSB_FIRST 0U
#define CPU_BYTE_ORDER_LITTLE_ENDIAN 0U
#define CPU_BIT_ORDER  CPU_BIT_ORDER_LSB_FIRST
#define CPU_BYTE_ORDER CPU_BYTE_ORDER_LITTLE_ENDIAN

#endif 
