#include "rbtree.h"
#include "../alloc.h"

#include <string.h>

typedef enum {
    BLACK,
    RED
} color_t;

struct jam_rbtree_node_s {
    jam_rbtree_item_t item;
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
static jam_rbtree_node_t* rbtree_node_find(jam_rbtree_node_t* node, uintptr_t key);

static color_t node_get_color(jam_rbtree_node_t* node);
static bool node_is_leaf(jam_rbtree_node_t* node);
static jam_rbtree_node_t* node_max_child(jam_rbtree_node_t* node);
static jam_rbtree_node_t* node_min_child(jam_rbtree_node_t* node);

static void node_replace(jam_rbtree_node_t* old, jam_rbtree_node_t* new);
static void node_rotate_left(jam_rbtree_node_t* node);
static void node_rotate_right(jam_rbtree_node_t* node);

static void node_swap_values(jam_rbtree_node_t* a, jam_rb_tree_node_t* b);

static bool node_insert(jam_rbtree_node_t* in_tree, jam_rbtree_node_t* node);
static void node_insert_case1(jam_rbtree_node_t* node);
static void node_insert_case2(jam_rbtree_node_t* node);
static void node_insert_case3(jam_rbtree_node_t* node);
static void node_insert_case4(jam_rbtree_node_t* node);
static void node_insert_case5(jam_rbtree_node_t* node);

static void node_delete(jam_allocator_t* allocator, jam_rbtree_item_t* node);
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

static jam_rbtree_node_t* rbtree_node_find(jam_rbtree_node_t* node, uintptr_t key)
{
    jam_rbtree_node_t* ret = NULL;
    if (node == NULL) { return NULL; }
    if (node->item.key == key) { return node; }
    return (node->item.key > key) ? rbtree_node_find(node->left, key) : rbtree_node_find(node->right, key);
}

static color_t node_get_color(jam_rbtree_node_t* node)
{
    return (node != NULL) ? node->color : BLACK;
}

static bool node_is_leaf(jam_rbtree_node_t* node)
{
    return (node->left == NULL) && (node->right == NULL);
}

static jam_rbtree_node_t* node_max_child(jam_rbtree_node_t* node)
{
    if (node->right != NULL) return node_max_child(node->right);
    return node;
}

static jam_rbtree_node_t* node_min_child(jam_rbtree_node_t* node)
{
    if (node->left != NULL) return node_min_child(node->left);
    return node;
}

static void node_replace(jam_rbtree_node_t* old, jam_rbtree_node_t* new)
{
    new->parent = old->parent;
    if (old == old->parent->left)
    {
        old->parent->left = new;
    }
    else
    {
        old->parent->right = new;
    }
}

static void node_rotate_left(jam_rbtree_node_t* node)
{
    if (node == NULL) return;
    jam_rbtree_node_t* gp = rbtree_node_grandparent(node);
    if (gp == NULL) return;

    jam_rbtree_node_t* saved_parent = gp->left;
    jam_rbtree_node_t* saved_node_left = node->left;

    // rotate nodes.
    gp->left = node;
    node->left = saved_parent;
    saved_parent->right = saved_node_left;

    // fixup parent references.
    node->parent = gp;
    saved_parent->parent = node;
    saved_node_left->parent = saved_parent;
}

static void node_rotate_right(jam_rbtree_node_t* node)
{
    if (node == NULL) return;
    jam_rbtree_node_t* gp = rbtree_node_grandparent(node);
    if (gp == NULL) return;

    jam_rbtree_node_t* saved_parent = gp->right;
    jam_rbtree_node_t* saved_node_right = node->right;

    /* rotate nodes. */
    gp->right = node;
    node->right = saved_parent;
    saved_parent->right = saved_node_right;

    /* fixup parent references. */
    node->parent = gp;
    saved_parent->parent = node;
    saved_node_right->parent = saved_parent;
}

static void node_swap_values(jam_rbtree_node_t* a, jam_rb_tree_node_t* b)
{
    jam_rbtree_item_t tmp = a->item;
    a->item = b->item;
    b->item = tmp;
}

static bool node_insert(jam_rbtree_node_t* in_tree, jam_rbtree_node_t* node)
{
    /* TODO :: report key collisions? */
    if (in_tree->item.key == node->item.key)
    {
        return false;
    }
    else if (in_tree->item.key > node->item.key)
    {
        if (in_tree->left == NULL)
        {
            in_tree->left = node;
            return true;
        }
        else
        {
            return node_insert(in_tree->left, node);
        }
    }
    else
    {
        if (in_tree->right == NULL)
        {
            in_tree->right = node;
            return true;
        }
        else
        {
            return node_insert(in_tree->right, node);
        }
    }
}

static void node_insert_case1(jam_rbtree_node_t* node)
{
    if (node->parent == NULL)
    {
        node->color = BLACK;
    }
    else
    {
        node_insert_case2(node);s
    }
}

static void node_insert_case2(jam_rbtree_node_t* node)
{
    if (node->parent->color == BLACK)
    {
        return;
    }
    else
    {
        node_insert_case3(node);
    }
}

static void node_insert_case3(jam_rbtree_node_t* node)
{
    jam_rbtree_node_t* uncle = rbtree_node_uncle(node);

    if ((uncle != NULL) && (uncle->color == RED))
    {
        node->parent->color = BLACK;
        uncle->color = BLACK;
        jam_rbtree_node_t* gp = rbtree_node_grandparent(node);
        gp->color = RED;
        node_insert_case1(gp);
    }
    else
    {
        node_insert_case4(node);
    }
}

static void node_insert_case4(jam_rbtree_node_t* node)
{
    jam_rbtree_node_t* gp = rbtree_node_grandparent(node);
    if (gp == NULL) return;

    if ((node == node->parent->right) && (node->parent == gp->left))
    {
        node_rotate_left(node);
        node = node->left;
    }
    else if ((node == node->parent->left) && (node->parent == gp->right))
    {
        node_rotate_right(node->parent);
        node = node->right;
    }
    node_insert_case5(node);
}

static void node_insert_case5(jam_rbtree_node_t* node)
{
    jam_rbtree_node_t* gp = rbtree_node_grandparent(node);
    if (gp==NULL) return;

    node->parent->color = BLACK;
    gp->color = RED;
    if (node == node->parent->left)
    {
        node_rotate_right(gp);
    }
    else
    {
        node_rotate_left(gp);
    }
}

static void node_delete(jam_allocator_t* allocator, jam_rbtree_item_t* node)
{
    int children = 0;
    if (node->left != NULL) children++;
    if (node->right != NULL) children++;
    switch(children)
    {
        case 0:
        {
            /* if the node has no children, we can just delete it. */
            jam_rbtree_node_t* parent = node->parent;
            if (parent != NULL)
            {
                if (parent->left == node) parent->left == NULL;
                else parent->right == NULL;
            }
            _FREE(tree->allocator, node);
            break;
        }
        case 1:
        {
            jam_rbtree_node_t* child = (node->right == NULL) ? node->left : node->right;
            node_replace(node, child);
            if (node->color == BLACK)
            {
                if (child->color == RED)
                {
                    child->color = BLACK;
                }
                else
                {
                    node_delete_case1(child);
                }
            }
            _FREE(allocator, node);
            break;
        }
        case 2:
        {
            jam_rbtree_node_t* child = (node->right != NULL) ? node_min_child(node->right) : node_max_child(node->left);
            /* swap our content with our closest child (which then will be deleted). */
            node_swap_values(node, child);
            node = child;
            node_delete(allocator, node);
            break;
        }
    }
}

static void node_delete_case1(jam_rbtree_item_t* node)
{
    if (node->parent != NULL)
        node_delete_case2(node);
}

static void node_delete_case2(jam_rbtree_item_t* node)
{
    jam_rbtree_node_t* sibling = rbtree_node_sibling(node);

    if (node_get_color(sibling) == RED)
    {
        node->parent->color = RED;
        sibling->color = BLACK;
        if (node == node->parent->left)
        {
            node_rotate_left(node->parent);
        }
        else
        {
            node_rotate_right(node->parent);
        }
    }
    node_delete_case3(node);
}

static void node_delete_case3(jam_rbtree_item_t* node)
{
    jam_rbtree_node_t* sibling = rbtree_node_sibling(node);

    if (node_get_color(node->parent) == BLACK) &&
        sibling != NULL &&
        node_get_color(sibling) == BLACK) &&
        node_get_color(sibling->left) == BLACK) &&
        node_get_color(sibling->right) == BLACK))
    {
        sibling->color = RED;
        node_delete_case1(node->parent);
    }
    else
    {
        node_delete_case4(node);
    }
}

static void node_delete_case4(jam_rbtree_item_t* node)
{
    jam_rbtree_node_t* sibling = rbtree_node_sibling(node);

    if (node_get_color(node->parent) == RED) &&
        sibling != NULL &&
        node_get_color(sibling) == BLACK) &&
        node_get_color(sibling->left) == BLACK) &&
        node_get_color(sibling->right) == BLACK))
    {
        sibling->color = RED;
        node->parent->color = BLACK;
    }
    else
    {
        node_delete_case5(node);
    }
}

static void node_delete_case5(jam_rbtree_item_t* node)
{
    jam_rbtree_node_t* sibling = rbtree_node_sibling(node);

    if (sibling != NULL && node_get_color(sibling) == BLACK)
    {
        if ((node == node->parent->left) &&
            node_get_color(sibling->right) == BLACK) &&
            node_get_color(sibling->left) == RED))
        {
            sibling->color = RED;
            sibling->left->color = BLACK;
            node_rotate_right(sibling);
        }
        else if ((node == node->parent->right) &&
                 node_get_color(sibling->left) == BLACK) &&
                 node_get_color(sibling->right) == RED))
        {
            sibling->color = RED;
            sibling->right->color = BLACK;
            node_rotate_left(sibling);
        }
    }
    node_delete_case6(node);
}

static void node_delete_case6(jam_rbtree_item_t* node)
{
    jam_rbtree_node_t* sibling = rbtree_node_sibling(node);

    s->color = node->parent->color;
    node->parent->color = BLACK;

    if (node == node->parent->left)
    {
        sibling->right->color = BLACK;
        node_rotate_left(node->parent);
    }
    else
    {
        sibling->left->color = BLACK;
        node_rotate->right(node->parent);
    }
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
    if (node_insert(tree->root, node))
    {
        node_insert_case1(node);
        return JAM_RESULT_OK;
    }
    else
    {
        _FREE(tree->allocator, node);
        return JAM_RESULT_EXISTS;
    }
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
        node_delete(tree->allocator, node);
    }
    return JAM_RESULT_OK;
}

jam_result_t jam_rbtree_size(jam_rbtree_t* tree, uintptr_t* size)
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

jam_result_t jam_rbtree_find(jam_rbtree_t* tree, uintptr_t key, void** data)
{
    if (tree == NULL) return JAM_RESULT_BADARG;
    jam_rbtree_node_t* node = rbtree_node_find(tree->root, key);
    if (node == NULL) return JAM_RESULT_NOENT;
    *data = node->item.data;
    return JAM_RESULT_OK;
}
/* end public api */