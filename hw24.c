//#include "red_black.h"

#include <stdlib.h>

typedef struct avl_node {
    int bf;
    int data;

    struct avl_node *left;
    struct avl_node *right;
    int l_h;
    int r_h;
    struct avl_node *parent;
} avl_node;
typedef struct {
    avl_node *root;
} avl_tree;

avl_tree *new_avl_tree() {
    avl_tree *new = malloc(sizeof(avl_tree));
    if (!new) return NULL;
    new->root = NULL;
    return new;
}

avl_node *new_avl_node(int data) {
    avl_node *new = malloc(sizeof(avl_node));
    if (!new) return NULL;

    new -> data = data;
    new -> bf = 0;
    new -> left = NULL;
    new -> right = NULL;
    new->l_h = 0;
    new->r_h = 0;
    new->parent = NULL;

    return new;
}

#define h(n) (!(n) ? 0 : (n)->l_h > (n)->r_h ? (n)->l_h + 1 : (n)->r_h + 1)
#define adopt(p, pos, c) {(p)->pos = c; if (c) (c)->parent = p; }
#define field(n) (!n->parent ? NULL : n->parent->left == n ? &(n->parent->left) : &(n->parent->right))

void ll_rot(avl_node *, avl_node **);
void lr_rot(avl_node *, avl_node **);
void rr_rot(avl_node *, avl_node **);
void rl_rot(avl_node *, avl_node **);

void bf_height(avl_node *n, avl_node **avl_root) {
    if (!n) return;
    n->l_h = h(n->left);
    n->r_h = h(n->right);
    n->bf = n->l_h-n->r_h;
    if (n->bf < -1) { //l << r
        if (n->right->bf > 0) rl_rot(n->parent, field(n) ? field(n) : avl_root);
        else rr_rot(n->parent, field(n) ? field(n) : avl_root);
    }
    if (n->bf > 1) { //l >> r
        if (n->left->bf >= 0) ll_rot(n->parent, field(n) ? field(n) : avl_root);
        else lr_rot(n->parent, field(n) ? field(n) : avl_root);
    }
    bf_height(n->parent, avl_root);
}

void ll_rot(avl_node *parent, avl_node **field) {
    avl_node *p = *field;
    avl_node *l = p->left;
    *field = l;
    l->parent = parent;
    adopt(p, left, l->right);
    adopt(l, right, p);

    bf_height(p, field);
}

void lr_rot(avl_node *parent, avl_node **field) {
    avl_node *p = *field;
    avl_node *l = p->left;
    avl_node *lr = l->right;
    avl_node *lrl = lr->left;
    avl_node *lrr = lr->right;
    *field = lr;
    lr->parent = parent;
    adopt(lr, left, l);
    adopt(lr, right, p);
    adopt(l, right, lrl);
    adopt(p, left, lrr);

    l->r_h = h(l->right);
    l->l_h = h(l->left);
    l->bf = l->l_h - l->r_h;
    p->r_h = h(p->right);
    p->l_h = h(p->left);
    p->bf = p->l_h - p->r_h;
    bf_height(lr, field);
}

void rr_rot(avl_node *parent, avl_node **field) {
    avl_node *p = *field;
    avl_node *r = p->right;
    *field = r;
    r->parent = parent;
    avl_node *rl = r->left;
    adopt(r, left, p);
    adopt(p, right, rl);
    bf_height(p, field);
}

void rl_rot(avl_node *parent, avl_node **field) {
    avl_node *p = *field;
    avl_node *r = p->right;
    avl_node *rl = r->left;
    avl_node *rll = rl->left;
    avl_node *rlr = rl->right;
    *field = rl;
    rl->parent = parent;
    adopt(rl, left, p);
    adopt(rl, right, r);
    adopt(p, right, rll);
    adopt(r, left, rlr);
    p->l_h = h(p->left);
    p->r_h = h(p->right);
    p->bf = p->l_h - p->r_h;
    r->l_h = h(r->left);
    r->r_h = h(r->right);
    r->bf = r->l_h - r->r_h;

    bf_height(rl, field);
}

//return how height changed
void avl_insert_p(avl_tree *tree, avl_node *parent, avl_node **pos, int data) {
    if (!(*pos)) {
        *pos = new_avl_node(data);
        (*pos) -> parent = parent;
        bf_height(*pos, &tree->root);
        return;
    }
    if (data == (*pos)->data) return;
    avl_insert_p(tree, *pos, data < (*pos)->data ? &(*pos)->left : &(*pos)->right, data);
}
void avl_insert(avl_tree *tree, int data) {
    avl_insert_p(tree, NULL, &tree->root, data);
}

avl_node *avl_search(avl_node *root, int data) {
    if (!root || root->data == data) return root;
    return data < root->data ? avl_search(root->left, data) : avl_search(root->right, data);
}

avl_node *rightmost(avl_node *n) {
    return !n->right ? n : rightmost(n->right);
}
avl_node *leftmost(avl_node *n) {
    return !n->left ? n : leftmost(n->left);
}

void avl_delete(avl_tree *tree, int data) {
    if (!tree->root) return;
    avl_node *target = avl_search(tree->root, data);
    if (!target) return;
    avl_node *rep = target->left ? rightmost(target->left) : target->right ? leftmost(target->right) : target;
    target->data = rep->data;
    avl_node *parent = rep->parent;
    avl_node *ava_node = rep->left ? rep->left : rep->right ? rep->right : NULL;
    *field(rep) = ava_node;
    if (ava_node) ava_node->parent = parent;
    free(rep);
    bf_height(parent, &(tree->root));
}






#define swap(a, b, typ) {typ t = a; a = b; b = t;}
#define is_black(n) (!n || n->color == black)
#define red 0
#define black 1
#include <assert.h>

typedef struct rb_node {
    int color;
    int data;

    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *parent;
} rb_node;

#include <stdlib.h>
rb_node *new_rb_node(int data) {
    rb_node *new = malloc(sizeof(rb_node));
    if (!new) return NULL;
    new->data = data;
    new->color = red;
    new->left = NULL;
    new->right = NULL;
    new->parent = NULL;
    return new;
}

rb_node *red_black_search(rb_node *root, int data) {
    if (!root || root->data == data) return root;
    return red_black_search(data < root->data ? root->left : root->right, data);
}

void sort2(rb_node **s, rb_node **b) {
    if ((*s)->data > (*b)->data) swap(*s, *b, rb_node *);
}
void sort(rb_node **s, rb_node **m, rb_node **l) {
    //make l be the largest of 3
    sort2(s, l);
    sort2(m, l);
    //then sort s and m
    sort2(s, m);
}

//void ins_adjust(rb_node *n) {
//    if (!n->parent) {
//        n->color = black;
//        return;
//    }
//    if (is_black(n) || is_black(n->parent)) return;
//    rb_node *uncle = n->parent->parent->left == n->parent ? n->parent->parent->right : n->parent->parent->left;
//    if (!is_black(uncle)) {
//        n->parent->color = uncle->color = black;
//        n->parent->parent->color = red;
//        ins_adjust(n->parent->parent);
//        return;
//    }
//
//}

//void inser(rb_node **root, rb_node *parent, int data) {
//    if (!(*root)) {
//        *root = new_rb_node(data);
//        (*root)->parent = parent;
//        if (!parent) (*root)->color = black;
//        ins_adjust(*root);
//    }
//    if ((*root)->data == data) return;
//    rb_node **pos = data < (*root)->data ? &(*root)->left : &(*root)->right;
//    inser(pos, *root, data);
//}

//return the branch with consecutive red if root and child is red
rb_node **insert(rb_node **root, int data) {
    if (!(*root)) {
        *root = new_rb_node(data);
        return NULL;
    }
    if (data == (*root)->data) {
        return NULL;
    }

    rb_node *grand_red;
    int insert_left;
    if (data < (*root)->data) {
        rb_node *old_left = (*root)->left;
        rb_node **grand_field_red = insert(&(*root)->left, data);
        if (old_left != (*root)->left) {
            (*root)->left->parent = *root;
        }
        if (!grand_field_red) {
            if ((*root) -> color == red && (*root)->left->color == red) {
                return &(*root)->left;
            } else {
                return NULL;
            }
        }
        grand_red = *grand_field_red;
        insert_left = 1;
    } else {
        rb_node *old_right = (*root)->right;
        rb_node **grand_field_red = insert(&(*root)->right, data);
        if (old_right != (*root)->right) {
            (*root)->right->parent = *root;
        }
        if (!grand_field_red) {
            if ((*root)->color == red && (*root)->right->color == red) {
                return &(*root)->right;
            } else {
                return NULL;
            }
        }
        grand_red = *grand_field_red;
        insert_left = 0;
    }

    //consecutive red
    rb_node *parent = *root;
    if (insert_left) {
        rb_node *child = (*root)->left;
        if ((*root)->right && (*root)->right->color == red) {
            parent->left->color = black;
            parent->right->color = black;
            parent->color = red;
            return NULL;
        } else {
            sort(&child, &parent, &grand_red);

            if (parent->left != child) {
                child->right = parent->left;
            }
            if (child->right) {
                child->right->parent = child;
            }
            grand_red->left = parent->right;
            if (grand_red->left) {
                grand_red->left->parent = grand_red;
            }
            parent->left = child;
            parent->right = grand_red;
            parent->parent = (*root)->parent;
            child->parent = parent;
            grand_red->parent = parent;
            child->color = red;
            grand_red->color = red;
            parent->color = black;

            *root = parent;
            return NULL;
        }
    } else {
        rb_node *child = (*root)->right;
        if ((*root)->left && (*root)->left->color == red) {
            parent->left->color = black;
            parent->right->color = black;
            parent->color = red;
            return NULL;
        } else {
            sort(&child, &parent, &grand_red);
            child->right = parent->left;
            if (child->right) {
                child->right->parent = child;
            }
            if (parent->right != grand_red) {
                grand_red->left = parent->right;
            }
            if (grand_red->left) {
                grand_red->left->parent = grand_red;
            }
            parent->left = child;
            parent->right = grand_red;
            parent->parent = (*root)->parent;
            child->parent = parent;
            grand_red->parent = parent;
            child->color = red;
            grand_red->color = red;
            parent->color = black;

            *root = parent;
            return NULL;
        }
    }
}

void red_black_insert(rb_node **root, int data) {
    insert(root, data);
    (*root)->color = black;
}

#include <assert.h>

rb_node *get_b_node_root(rb_node *n) {
    if (n->color == red) {
        assert(n->parent->color == black);
        return n->parent;
    }
    return n;
}

//leaf here means the leaf in a 2-3-4 tree representation, not the actual leaf of rb tree
//return whether in a leaf node, in 2-3-4 tree representation
int is_leaf(rb_node *n) {
    n = get_b_node_root(n);
    assert(n->color == black);
    //node with 1 number
    if (!n->left && !n->right) {
        return 1;
    }

    //with 2 numbers
    if (n->left && n->left->color == red && !n->right) {
        if (!n->left->left && !n->left->right) {
            return 1;
        } else {
            //invalid rb tree
            //black edge number incorrect
            abort();
        }
    }
    if (n->right && n->right->color == red && !n->left) {
        if (!n->right->left && !n->right->right) {
            return 1;
        } else {
            //invalid rb tree
            //black edge number incorrect
            abort();
        }
    }

    //three numbers
    return n->left && n->left->color == red && n->right && n->right->color == red &&
           !n->left->left && !n->left->right && !n->right->left && !n->right->right;
}

//differ from find_replacer in b tree, this will return the accurate node (identical to *replace_num in original version)
//the return value of original find_replacer can be obtained by get_b_node_root
rb_node *find_replacer(rb_node *n) {
    if (n->left) {
        rb_node *current = n->left;
        while (!is_leaf(current)) {
            current = current->right;
        }
        if (current->color == black && current->right) {
            current = current->right;
        }
        return current;
    } else {
        rb_node *current = n->right;
        while(!is_leaf(current)) {
            current = current->left;
        }
        if (current->color == black && current->left) {
            current =  current->left;
        }
        return current;
    }
}

//node -> node in b tree
int node_with_one_number(rb_node *n) {
    return n->color == black && (!n->left || n->left->color == black) && (!n->right || n->right->color == black);
}

int node_with_three_numbers(rb_node *n) {
    n = get_b_node_root(n);
    return n->left && n->left->color == red && n->right && n->right->color == red;
}

int node_with_two_number(rb_node *n) {
    return !node_with_one_number(n) && !node_with_three_numbers(n);
}

rb_node *get_left_sibling(rb_node *parent, rb_node *n) {
    parent = get_b_node_root(parent);
    if (node_with_one_number(parent)) {
        if (n == parent->left) return NULL;
        return parent->left;
    }

    //two number
    if (node_with_two_number(parent)) {
        if (parent->right->color == red) {
            if (n == parent->left) return NULL;
            if (n == parent->right->left) return parent->left;
            return parent->right->left;
        } else {
            if (n == parent->right) return parent->left->right;
            if (n == parent->left->right) return parent->left->left;
            return NULL;
        }
    }

    //three number
    if (n == parent->left->left) return NULL;
    if (n == parent->left->right) return parent->left->left;
    if (n == parent->right->left) return parent->left->right;
    return parent->right->left;
}

rb_node *get_right_sibling(rb_node *parent, rb_node *n) {
    parent = get_b_node_root(parent);
    if (node_with_one_number(parent)) {
        if (n == parent->right) return NULL;
        return parent->right;
    }

    //two number
    if (node_with_two_number(parent)) {
        if (parent->right->color == red) {
            if (n == parent->left) return parent->right->left;
            if (n == parent->right->left) return parent->right->right;
            return NULL;
        } else {
            if (n == parent->right) return NULL;
            if (n == parent->left->right) return parent->right;
            return parent->left->right;
        }
    }

    //three number
    if (n == parent->left->left) return parent->left->right;
    if (n == parent->left->right) return parent->right->left;
    if (n == parent->right->left) return parent->right->right;
    return NULL;
}

rb_node *find_rotatable_sibling(rb_node *n, int *rot_sibling_is_left) {
    n = get_b_node_root(n);
    rb_node *left_sib = get_left_sibling(n->parent, n);
    if (left_sib && !node_with_one_number(left_sib)) {
        *rot_sibling_is_left = 1;
        return left_sib;
    }
    rb_node *right_sib = get_right_sibling(n->parent, n);
    if (right_sib && !node_with_one_number(right_sib)) {
        *rot_sibling_is_left = 0;
        return right_sib;
    }
    return NULL;
}

int max_number(rb_node *n) {
    n = get_b_node_root(n);
    if (node_with_one_number(n)) {
        return n->data;
    }
    if (node_with_two_number(n)) {
        if (n->right && n->right->color == red) {
            return n->right->data;
        }
        return n->data;
    }
    return n->right->data;
}

int min_number(rb_node *n) {
    n = get_b_node_root(n);
    if (node_with_one_number(n)) {
        return n->data;
    }
    if (node_with_two_number(n)) {
        if (n->left && n->left->color == red) {
            return n->left->data;
        }
        return n->data;
    }
    return n->left->data;
}

rb_node *detach_rightmost_b(rb_node *n) {
    n = get_b_node_root(n);
    assert(!node_with_one_number(n));
    if (node_with_two_number(n)) {
        if (n->right && n->right->color == red) {
            rb_node *right = n->right;
            n->right = right->left;
            if (right->left) {
                right->left->parent = n;
            }
            rb_node *rightmost = right->right;
            free(right);
            return rightmost;
        } else {
            n->data = n->left->data;
            rb_node *rightmost = n->right;
            n->right = n->left->right;
            if (n->left->right) {
                n->left->right->parent = n;
            }
            rb_node *left = n->left;
            n->left = left->left;
            if (left->left) {
                left->left->parent = n;
            }
            free(left);
            return rightmost;
        }
    }

    rb_node *rightmost = n->right->right;
    rb_node *right = n->right;
    n->right = right->left;
    if (right->left) {
        right->left->parent = n;
    }
    free(right);
    return rightmost;
}

rb_node *detach_leftmost_b(rb_node *n) {
    n = get_b_node_root(n);
    assert(!node_with_one_number(n));
    if (node_with_two_number(n)) {
        if (n->right && n->right->color == red) {
            rb_node *leftmost = n->left;
            n->data = n->right->data;
            rb_node *right = n->right;
            n->left = right->left;
            if (right->left) {
                right->left->parent = n;
            }
            n->right = right->right;
            if (right->right) {
                right->right->parent = n;
            }
            free(right);
            return leftmost;
        } else {
            rb_node *leftmost = n->left->left;
            rb_node *left = n->left;
            n->left = left->right;
            if (left->right) {
                left->right->parent = n;
            }
            free(left);
            return leftmost;
        }
    }

    rb_node *leftmost = n->left->left;
    rb_node *left = n->left;
    n->left = left->right;
    if (left->right) {
        left->right->parent = n;
    }
    free(left);
    return leftmost;
}

void rotate(rb_node *n, rb_node *sibling, int is_left_sib) {
    n = get_b_node_root(n);
    assert(node_with_one_number(n));
    sibling = get_b_node_root(sibling);
    assert(!node_with_one_number(sibling));

    rb_node *via;
    if (n->parent == sibling->parent) {
        via = n->parent;
    } else {
        via = n->parent;
        while(via->color != black) {
            via = via->parent;
        }
    }
    if (is_left_sib) {
        n->data = via->data;
        via->data = max_number(sibling);
        rb_node *rightmost_b = detach_rightmost_b(sibling);
        n->right = n->left;
        n->left = rightmost_b;
        if (rightmost_b) {
            rightmost_b->parent = n;
        }
    } else {
        n->data = via->data;
        via->data = min_number(sibling);
        rb_node *leftmost_b = detach_leftmost_b(sibling);
        n->right = leftmost_b;
        if (leftmost_b) {
            leftmost_b->parent = n;
        }
    }
}

//special function for node with 2 numbers, since it has two representations in rb tree
//rotated root of the node will be returned
rb_node *change_representation(rb_node *n) {
    n = get_b_node_root(n);
    assert(node_with_two_number(n));
    if (n->right->color == red) {
        n->color = red;
        n->right->color = black;
        rb_node *f_right = n->right->left;
        n->right->left = n;
        n->right->parent = n->parent;
        n->parent = n->right;
        n->right = f_right;
        if (f_right) {
            f_right->parent = n;
        }
    } else {
        n->color = red;
        n->left->color = black;
        rb_node *f_left = n->left->right;
        n->left->right = n;
        n->left->parent = n->parent;
        n->parent = n->left;
        n->left = f_left;
        if (f_left) {
            f_left->parent = n;
        }
    }
    return n->parent;
}

//the parent of n might change, for simplicity when n->parent contains two numbers
rb_node *safe_combine(rb_node *n) {
    assert(!node_with_one_number(n->parent));
    if (node_with_three_numbers(n->parent)) {
        rb_node *sib;
        rb_node *parent = n->parent;
        if (n == parent->left) {
            sib = parent->right;
        } else {
            sib = parent->left;
        }
        parent->color = black;
        sib->color = red;
        if (n == parent->left) {
            parent->left = n->left;
            if (n->left) {
                n->left->parent = parent;
            }
        } else {
            parent->right = n->left;
            if (n->left) {
                n->left->parent = parent;
            }
        }
        free(n);
        return parent;
    }
    //two number
    rb_node *parent = n->parent;
    if (parent->color == black) {
        parent = change_representation(parent);
    }
    assert(n->parent->color == red);
    if (n == n->parent->left) {
        n->parent->color = black;
        n->parent->right->color = red;
        n->parent->left = n->left;
        if (n->left) {
            n->left->parent = n->parent;
        }
        free(n);
    } else {
        n->parent->color = black;
        n->parent->left->color = red;
        n->parent->right = n->left;
        if (n->left) {
            n->left->parent = n->parent;
        }
        free(n);
    }
    return parent;
}

rb_node *unsafe_combine(rb_node *n) {
    if (n == n->parent->left) {
        n->data = n->parent->data;
        n->right = n->parent->right;
        if (n->parent->right) {
            n->parent->right->parent = n;
        }
        n->color = black;
        n->right->color = red;
        n->parent->right = NULL;
        return n;
    } else {
        n->data = n->parent->data;
        rb_node *f_parent = n->parent->left;
        n->right = n->left;
        n->left = f_parent->right;
        if (f_parent->right) {
            f_parent->right->parent = n;
        }
        f_parent->right = n;
        n->parent = f_parent;
        f_parent->color = black;
        n->color = red;
        f_parent->parent->right = NULL;
        return f_parent;
    }
}

rb_node *try_rotate_or_combine(rb_node *root, rb_node *n) {
    int rot_sibling_is_left;
    rb_node *rotate_sibling = find_rotatable_sibling(n, &rot_sibling_is_left);
    if (rotate_sibling) {
        rotate(n, rotate_sibling, rot_sibling_is_left);
        return root;
    }

    //combine
    if (!node_with_one_number(n->parent)) {
        //safe combine
        rb_node *r = get_b_node_root(n)->parent;
        if (r->parent) {
            rb_node *parent = r->parent;
            if (r->parent->left == r) {
                r = safe_combine(n);
                parent->left = r;
                r->parent = parent;
            } else {
                r = safe_combine(n);
                parent->right = r;
                r->parent = parent;
            }
            return root;
        } else if (r == root){
            r = safe_combine(n);
            return r;
        } else {
            //invalid
            assert(0);
        }
    }

    //unsafe combine
    n = unsafe_combine(n);
    if (n->parent == root) {
        n->parent = NULL;
        return n;
    }
    return try_rotate_or_combine(root, n->parent);
}

//same function as remove_leaf_node in b tree
rb_node *delete_leaf_node(rb_node *root, rb_node *n) {
    if (!node_with_one_number(n)) {
        if (n->color == red) {
            //just delete
            if (n == n->parent->left) {
                n->parent->left = NULL;
            } else {
                n->parent->right = NULL;
            }
            free(n);
            return root;
        } else {
            if (n->left && n->left->color == red) {
                n->data = n->left->data;
                free(n->left);
                n->left = NULL;
            } else {
                n->data = n->right->data;
                free(n->right);
                n->right = NULL;
            }
            return root;
        }
    }

    if (n == root) {
        free(n);
        return NULL;
    }
    return try_rotate_or_combine(root, n);
}

//Since rb tree is an alternative representation of b tree,
//delete will be implemented in a structure similar to that of b tree deletion
void red_black_delete(rb_node **root, int data) {
    if (!*root)  return;

    rb_node *n = red_black_search(*root, data);
    if (!n) return;

    if (data == 63554) {
        goto n;
    }
    n:
    if (!is_leaf(n)) {
        rb_node *replacer = find_replacer(n);
        n->data = replacer->data;
        n = replacer;
    }

    *root = delete_leaf_node(*root, n);
}

void red_black_free(rb_node *root) {
    if (!root) return;
    red_black_free(root->left);
    red_black_free(root->right);
    free(root);
}















void free_avl(avl_node *root) {
    if (!root) return;
    free_avl(root->left);
    free_avl(root->right);
    free(root);
}

#include <stdio.h>

void avl_balance(void *n) {
    if (n) {
        printf("%d\n", *(int *)n);
    } else {
        printf("Not found\n");
    }
}

void avl_inorder(avl_node *root) {
    if (!root) {
        printf("NULL ");
        return;
    }
    avl_inorder(root->left);
    printf("%d ", root->data);
    avl_inorder(root->right);
}

void rb_balance(void *n) {
    if (n) {
        printf("%s\n", (*(int *)n) ? "black" : "red");
    } else {
        printf("Not found\n");
    }
}


int main() {
    char tree_type[10];
    scanf("%s", tree_type);

    if (tree_type[0] == 'A') {
        avl_tree *tree = new_avl_tree();
        char cmd[7];
        while (1) {
            scanf("%s", cmd);
            int x;
            switch (cmd[0]) {
                case 'i': {
                    scanf("%d", &x);
                    avl_insert(tree, x);
                    break;
                }
                case 's': {
                    scanf("%d", &x);
                    avl_balance(avl_search(tree->root, x));
                    break;
                }
                case 'd': {
                    scanf("%d", &x);
                    avl_delete(tree, x);
                    break;
                }
                case 'e': {
                    scanf("%d", &x);
                    if (avl_search(tree->root, x)) {
                        printf("exist\n");
                    } else {
                        printf("Not exist\n");
                    }
                    break;
                }
                case 'q': {
                    free_avl(tree->root);
                    free(tree);
                    return 0;
                }
            }
        }
    } else {
        char cmd[7];
        rb_node *tree = NULL;
        while (1) {
            scanf("%s", cmd);
            int x;
            switch (cmd[0]) {
                case 'i': {
                    scanf("%d", &x);
                    red_black_insert(&tree, x);
                    break;
                }
                case 's': {
                    scanf("%d", &x);
                    rb_balance(red_black_search(tree, x));
                    break;
                }
                case 'd': {
                    scanf("%d", &x);
                    red_black_delete(&tree, x);
                    break;
                }
                case 'e': {
                    scanf("%d", &x);
                    if (red_black_search(tree, x)) {
                        printf("exist\n");
                    } else {
                        printf("Not exist\n");
                    }
                    break;
                }
                case 'q': {
                    red_black_free(tree);
                    return 0;
                }
            }
        }
    }
}