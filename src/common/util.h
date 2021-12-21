#ifndef PS1_UTIL_H
#define PS1_UTIL_H
#include <stdint.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#ifdef PS1_WIN
#define ASSERTWORD(type) _Static_assert(sizeof(type) == 4, "must be 32 bits")
#define ASSERTDWORD(type) _Static_assert(sizeof(type) == 8, "must be 64 bits")
#elif defined(__cplusplus)
#define ASSERTWORD(type) static_assert(sizeof(type) == 4, #type " must be 32 bits")
#define ASSERTDWORD(type) static_assert(sizeof(type) == 8, #type " must be 64 bits")
#else
#define ASSERT32(type) _Static_assert(sizeof(type) == 4, #type " must be 32 bits")
#define ASSERT64(type) _Static_assert(sizeof(type) == 8, #type " must be 64 bits")
#endif

#define popcount(x) __builtin_popcountll(x)
#define FAKELITTLE_HALF(h) ((((h) >> 8u) & 0xFFu) | (((h) << 8u) & 0xFF00u))
#define FAKELITTLE_WORD(w) (FAKELITTLE_HALF((w) >> 16u) | (FAKELITTLE_HALF((w) & 0xFFFFu)) << 16u)

#define unlikely(exp) __builtin_expect(exp, 0)
#define likely(exp) __builtin_expect(exp, 1)

#define INLINE static inline __attribute__((always_inline))
#define PACKED __attribute__((__packed__))

#endif //PS1_UTIL_H
