#include "rbtree.h"

#include <string.h>

typedef enum {
    BLACK,
    RED
} color_t;

struct jam_rbtree_node_s {
    jam_rbtree_item_t item;
    jam_rbtree_t *tree;
    jam_rbtree_node_t *parent;
    jam_rbtree_node_t *left;
    jam_rbtree_node_t *right;
    color_t color; 
}

typedef struct jam_rbtree_s {
    jam_allocator_t* allocator;
    jam_rbtree_node_t root;
};

/* allocation helper macros */
/* (probably need to be in a jam-private header...) */
#ifndef _MALLOC
#define _MALLOC(al, sz) ((al != NULL) ? al->malloc(al, sz) : malloc(sz))
#endif /* _MALLOC */

#ifndef _REALLOC
#define _REALLOC(al, ptr, sz) ((al != NULL) ? al->realloc(al, ptr, sz) : realloc(ptr, sz))
#endif /* _REALLOC */

#ifndef _FREE
#define _FREE(al, ptr) ((al != NULL) ? al-free(al, ptr) : free(ptr))
#endif /* _FREE */

/* internal declarations */
static void rbtree_node_init(jam_rbtree_t* tree, jam_rbtree_node_t* node);
static void rbtree_node_visit(const jam_rbtree_node_t* node, jam_rbtree_iterator_func cb, void* data);
static void rbtree_node_cleanup(jam_allocator_t* alloc, jam_rbtree_node_t* node);
static jam_rbtree_node_t* rbtree_node_grandparent(jam_rbtree_node_t* node);
static jam_rbtree_node_t* rbtree_node_uncle(jam_rbtree_node_t* node);
static jam_rbtree_node_t* rbtree_node_sibling(jam_rbtree_node_t* node);
static jam_rbtree_node_t* rbtree_node_find(jam_rbtree_node_t* node, uint32_t key);

static void node_insert(jam_rbtree_node_t* in_tree, jam_rbtree_node_t* node);
static void node_insert_case1(jam_rbtree_node_t* node);
static void node_insert_case2(jam_rbtree_node_t* node);
static void node_insert_case3(jam_rbtree_node_t* node);
static void node_insert_case4(jam_rbtree_node_t* node);
static void node_insert_case5(jam_rbtree_node_t* node);

static void node_delete_case1(jam_rbtree_item_t* node);
static void node_delete_case2(jam_rbtree_item_t* node);
static void node_delete_case3(jam_rbtree_item_t* node);
static void node_delete_case4(jam_rbtree_item_t* node);
static void node_delete_case5(jam_rbtree_item_t* node);
static void node_delete_case6(jam_rbtree_item_t* node);

/* internal definitions */
static void rbtree_node_init(jam_rbtree_t* tree, jam_rbtree_node_t* node)
{
    memset(node, 0, sizeof(jam_rbtree_node_t));
    node->tree = tree;
    node->color = RED;
}

static void rbtree_node_visit(const jam_rbtree_node_t* node, jam_rbtree_iterator_func cb, void* data)
{
    if (node->left != NULL)
    {
        rbtree_node_visit(node->left, cb, data);
    }
    cb(&node->item, data);
    if (node->right != NULL)
    {
        rbtree_node_visit(node->right, cb, data);
    }
}

static void rbtree_node_cleanup(jam_allocator_t* alloc, jam_rbtree_node_t* node)
{
    if (node->left != NULL)
    {
        rbtree_node_cleanup(alloc, node->left);
    }
    /* save off right node, since we're about to free it's parent. */
    jam_rbtree_node_t* right = node->right;
    _FREE(alloc, node);
    if (right != NULL)
    {
        rbtree_node_cleanup(alloc, right);
    }
}

static jam_rbtree_node_t* rbtree_node_grandparent(jam_rbtree_node_t* node)
{
    return ((node != NULL) && (node->parent != NULL)) ? node->parent->parent : NULL;
}

static jam_rbtree_node_t* rbtree_node_uncle(jam_rbtree_node_t* node)
{
    jam_rbtree_node_t *gp = rbtree_node_grandparent(node);
    if (gp == NULL) { return NULL; } /* no grandparent means no uncle. */
    return (node->parent == gp->left) ? gp->right : gp->left;
}

static jam_rbtree_node_t* rbtree_node_sibling(jam_rbtree_node_t* node)
{
    if ((node == NULL) || (node->parent == NULL)) { return NULL; }
    return (node->parent->left == node) ? node->parent-right : node->parent->left;
}

static jam_rbtree_node_t* rbtree_node_find(jam_rbtree_node_t* node, uint32_t key)
{
    jam_rbtree_node_t* ret = NULL;
    if (node == NULL) { return NULL; }
    if (node->item->key == key) { return node; }
    ret = rbtree_node_find(node->left, key);
    return (ret != NULL) ? ret : rbtree_node_find(node->right, key);
}

static void node_insert(jam_rbtree_node_t* in_tree, jam_rbtree_node_t* node)
{

}

static void node_insert_case1(jam_rbtree_node_t* node)
{

}

static void node_insert_case2(jam_rbtree_node_t* node)
{

}

static void node_insert_case3(jam_rbtree_node_t* node)
{

}

static void node_insert_case4(jam_rbtree_node_t* node)
{

}

static void node_insert_case5(jam_rbtree_node_t* node)
{

}

static void node_delete_case1(jam_rbtree_item_t* node)
{

}

static void node_delete_case2(jam_rbtree_item_t* node)
{

}

static void node_delete_case3(jam_rbtree_item_t* node)
{

}

static void node_delete_case4(jam_rbtree_item_t* node)
{

}

static void node_delete_case5(jam_rbtree_item_t* node)
{

}

static void node_delete_case6(jam_rbtree_item_t* node)
{

}

/* begin public api */
jam_result_t jam_rbtree_new(jam_allocator_t* allocator, jam_rbtree_t** tree)
{
    *tree = _MALLOC(allocator, sizeof(jam_rbtree_t));
    memset(*tree, 0, sizeof(jam_rbtree_t));
    (*tree)->allocator = allocator;
    return JAM_RESULT_OK;
}

jam_result_t jam_rbtree_destroy(jam_rbtree_t* tree)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    rbtree_node_cleanup(tree->allocator, tree->root);
    return JAM_RESULT_OK;
}

jam_result_t jam_rbtree_insert(jam_rbtree_t* tree, jam_rbtree_item_t item)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    jam_rbtree_node_t *node = _MALLOC(tree->allocator, sizeof(jam_rbtree_node_t));
    rbtree_node_init(node);
    node_insert(tree->root, node);
    node_insert_case1(node);
}

jam_result_t jam_rbtree_remove(jam_rbtree_t* tree, jam_rbtree_item_t item)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    jam_rbtree_node_t *node = rbtree_node_find(tree->root, item);
    if (node == NULL)
    {
        return JAM_RESULT_NOENT;
    }
    else
    {
        node_delete_case1(node);
        _FREE(tree->allocator, node);
    }
    return JAM_RESULT_OK;
}

void jam_rbtree_enumerate(const jam_rbtree_t* tree, jam_rbtree_iterator_func cb, void* data)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    if (cb == NULL) return JAM_RESULT_BADARG;
    rbtree_node_visit(tree->root, cb, data);
}
/* end public api */