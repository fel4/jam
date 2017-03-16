#include "hashtable.h"
#include "rbtree.h"
#include <stdint.h>

#define NUM_BUCKETS sizeof(uint8_t)

typedef struct {
    jam_rbtree_t tree;
} bucket_t;