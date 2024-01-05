#include "../../include/utils/hash_utils.h"
#include "../consts/hash_consts.h"

uint32_t MurmurOAAT_32(const char* str, uint32_t h) {
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

uint32_t hash_string_default(const char* str) {
    uint32_t hash = HASH_SEED;
    return MurmurOAAT_32(str, hash);
}



