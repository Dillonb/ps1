#ifndef PS1_MEM_UTIL_H
#define PS1_MEM_UTIL_H

#include <util.h>
#include <string.h>

INLINE void u16_to_byte_array(u8* arr, u32 index, u16 value) {
    memcpy(arr + index, &value, sizeof(u16));
}

INLINE u16 u16_from_byte_array(u8* arr, u32 index) {
    u16 val;
    memcpy(&val, arr + index, sizeof(u16));
    return val;
}

INLINE void u32_to_byte_array(u8* arr, u32 index, u32 value) {
    memcpy(arr + index, &value, sizeof(u32));
}

INLINE u32 u32_from_byte_array(u8* arr, u32 index) {
    u32 val;
    memcpy(&val, arr + index, sizeof(u32));
    return val;
}

#endif //PS1_MEM_UTIL_H
