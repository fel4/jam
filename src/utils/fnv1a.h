#pragma once

#ifndef JAM_UTILS_FNV1A_H
#define JAM_UTILS_FNV1A_H

#include "jam/jam.h"
#include <stdint.h>

BEGIN_C_EXTERN

typedef struct jam_fnv1a_hash_s jam_fnv1a_hash_t;

jam_result_t jam_fnv1a_hash_new(jam_allocator_t*, jam_fnv1a_hash_t**);
jam_result_t jam_fnv1a_hash_destroy(jam_fnv1a_hash_t*, jam_allocator_t*);
jam_result_t jam_fnv1a_hash_add(jam_fnv1a_hash_t*, const char*, size_t, uint64_t*);
jam_result_t jam_fnv1a_hash_get(jam_fnv1a_hash_t*, uint64_t*);

END_C_EXTERN
#endif /* JAM_UTILS_FNV1A_H */