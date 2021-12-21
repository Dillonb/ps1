#ifndef PS1_MEM_UTIL_H
#define PS1_MEM_UTIL_H

#include <util.h>
#include <string.h>

INLINE u32 u32_from_byte_array(u8* arr, u32 index) {
    u32 val;
    memcpy(&val, arr + index, sizeof(u32));
    return val;
}

#endif //PS1_MEM_UTIL_H
