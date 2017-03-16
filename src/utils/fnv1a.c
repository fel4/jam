#include "fnv1a.h"
#include "../alloc.h"
#include <stdint.h>

static const uint64_t fnv_offset_basis = 0xcbf29ce484222325;
static const uint64_t fnv_prime = 0x100000001b3;

struct jam_fnv1a_hash_s {
    uint64_t hash;
}

static unit64_t fnv_hash_pass(jam_fnv1a_hash_t* hash, const char* buffer, size_t buf_siz)
{
    for (size_t i = 0; i < buf_siz; i++)
    {
        char low_byte = (char)(hash->hash & 0xFF);
        low_byte ^= buffer[i];
        hash->hash = (hash->hash & 0xFFFFFFFFFFFFFF00) + low_byte;
        hash->hash *= fnv_prime;
    }
    return hash->hash;
}

/* begin public api */



/* end public api */