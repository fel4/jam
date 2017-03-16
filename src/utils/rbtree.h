#pragma once

#ifndef JAM_UTILS_RBTREE_H
#define JAM_UTILS_RBTREE_H

#include "jam/jam.h"
#include <stdint.h>

BEGIN_C_EXTERN

typedef struct {
    uintptr_t key;
    void* data;
} jam_rbtree_item_t;

typedef void (*jam_rbtree_iterator_func)(const jam_rbtree_item_t* item, void* data);
typedef struct jam_rbtree_node_s jam_rbtree_node_t;
typedef struct jam_rbtree_s jam_rbtree_t;

jam_result_t jam_rbtree_new(jam_allocator_t*, jam_rbtree_t**);
jam_result_t jam_rbtree_destroy(jam_rbtree_t*);
jam_result_t jam_rbtree_insert(jam_rbtree_t*, jam_rbtree_item_t);
jam_result_t jam_rbtree_remove(jam_rbtree_t*, jam_rbtree_item_t);
jam_result_t jam_rbtree_size(jam_rbtree_t*, uintptr_t* size);
jam_result_t jam_rbtree_enumerate(jam_rbtree_t*, jam_rbtree_iterator_func, void*);
jam_result_t jam_rbtree_find(jam_rbtree_t*, uintptr_t, void**);

END_C_EXTERN
#endif /* JAM_UTILS_RBTREE_H */