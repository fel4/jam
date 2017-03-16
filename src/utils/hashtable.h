#pragma once

#ifndef JAM_UTILS_HASHTABLE_H
#define JAM_UTILS_HASHTABLE_H

#include "jam/jam.h"

BEGIN_C_EXTERN

typedef struct jam_hashtable_s jam_hashtable_t;

jam_result_t jam_hashtable_new(jam_allocator_t*, jam_hashtable_t**);
jam_result_t jam_hashtable_destroy(jam_hashtable_t*);
jam_result_t jam_hashtable_add(jam_hashtable_t*, const char*, void*);
jam_result_t jam_hashtable_remove(jam_hashtable_t*, const char*);
jam_result_t jam_hashtable_get(jam_hashtable_t*, const char*, void **);

END_C_EXTERN
#endif /* JAM_UTILS_HASHTABLE_H */