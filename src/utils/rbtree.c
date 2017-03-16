#include "rbtree.h"
#include "../alloc.h"

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

struct jam_rbtree_s {
    jam_allocator_t* allocator;
    jam_rbtree_node_t root;
};

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
    return (node->item->key > key) ? rbtree_node_find(node->left, key) : rbtree_node_find(node->right, key);
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

jam_result_t jam_rbtree_size(jam_rbtree_t* tree, uint32_t* size)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    if (length == NULL) return JAM_RESULT_BADARG;
    if (tree->root == NULL) { *size = 0; return JAM_RESULT_OK; }
    // TODO :: finsh.
}

jam_result_t jam_rbtree_enumerate(const jam_rbtree_t* tree, jam_rbtree_iterator_func cb, void* data)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    if (cb == NULL) return JAM_RESULT_BADARG;
    rbtree_node_visit(tree->root, cb, data);
    return JAM_RESULT_OK;
}

jam_result_t jam_rbtree_find(jam_rbtree_t* tree, uint32_t key, void** data)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    jam_rbtree_node_t* node = rbtree_node_find(tree->root, key);
    if (node == NULL) return JAM_RESULT_NOENT;
    *data = node->item->data;
    return JAM_RESULT_OK;
}
/* end public api */