#include "fnv1a.h"
#include "../alloc.h"

struct jam_fnv1a_hash_s {
    jam_hash_value_t hash;
}

typedef struct {
    jam_hash_value_t offset_basis;
    jam_hash_value_t prime;
} fnv_hash_params;

static inline fnv_hash_params get_hash_params()
{
    fnv_hash_params ret;
    switch(sizeof(jam_hash_value_t))
    {
        case 32:
        {
            ret.offset_basis = 0x811c9dc5;
            ret.prime = 16777619;
            break;
        }
        case 64:
        default:
        {
            ret.offset_basis = 0xcbf29ce484222325;
            ret.prime = 1099511628211;
            break;
        }
    }
    return ret;
}

static jam_hash_value_t fnv_hash_pass(jam_fnv1a_hash_t* hash, const char* buffer, size_t buf_siz)
{
    fnv_hash_params params = get_hash_params();
    for (size_t i = 0; i < buf_siz; i++)
    {
        char byte_mask = 0xFF
        char low_byte = (char)(hash->hash & byte_mask);
        low_byte ^= buffer[i];
        hash->hash = (hash->hash & (~byte_mask)) + low_byte;
        hash->hash *= params.prime;
    }
    return hash->hash;
}

/* begin public api */

jam_result_t jam_fnv1a_hash_new(jam_allocator_t* allocator, jam_fnv1a_hash_t** hash)
{
    *hash = _MALLOC(allocator, sizeof(jam_fnv1a_hash_t));
    if ((*hash) == NULL) return JAM_RESULT_NOMEM;
    (*hash)->hash = get_hash_params().offset_basis;
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_destroy(jam_fnv1a_hash_t* hash, jam_allocator_t* allocator)
{
    _FREE(allocator, hash);
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_add(jam_fnv1a_hash_t* hash, const char* buffer, size_t buf_siz, jam_hash_value_t* out_hash)
{
    if (hash == NULL) return JAM_RESULT_BADARG;
    if (buffer == NULL) return JAM_RESULT_BADARG;
    if (buf_siz == 0) return JAM_RESULT_BADARG;
    jam_hash_value_t tmp = fnv_hash_pass(hash, buffer, buf_siz);
    if (out_hash != NULL) *out_hash = tmp;
    return JAM_RESULT_OK;
}

jam_result_t jam_fnv1a_hash_get(jam_fnv1a_hash_t* hash, jam_hash_value_t* out_hash)
{
    if (hash == NULL) return JAM_RESULT_BADARG;
    if (out_hash == NULL) return JAM_RESULT_BADARG;
    *out_hash = hash->hash;
    return JAM_RESULT_OK;
}

/* end public api */