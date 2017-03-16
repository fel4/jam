#include "fnv1a.h"
#include "../alloc.h"

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

jam_result_t jam_fnv1a_hash_new(jam_allocator_t* allocator, jam_fnv1a_hash_t** hash)
{
    *hash = _MALLOC(allocator, sizeof(jam_fnv1a_hash_t));
    if ((*hash) == NULL) return JAM_RESULT_NOMEM;
    (*hash)->hash = fnv_offset_basis;
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_destroy(jam_fnv1a_hash_t* hash, jam_allocator_t* allocator)
{
    _FREE(allocator, hash);
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_add(jam_fnv1a_hash_t* hash, const char* buffer, size_t buf_siz, uint64_t* out_hash)
{
    if (hash == NULL) return JAM_RESULT_BADARG;
    if (buffer == NULL) return JAM_RESULT_BADARG;
    if (buf_siz == 0) return JAM_RESULT_BADARG;
    uint64_t tmp = fnv_hash_pass(hash, buffer, buf_siz);
    if (out_hash != NULL) *out_hash = tmp;
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_get(jam_fnv1a_hash_t* hash, uint64_t* out_hash)
{
    if (hash == NULL) return JAM_RESULT_BADARG;
    if (out_hash == NULL) return JAM_RESULT_BADARG;
    *out_hash = hash->hash;
    return JAM_RESULT_OK;
}

/* end public api */