//#include "red_black.h"

#include <stdlib.h>

typedef struct avl_node {
    int bf;
    int data;

    struct avl_node *left;
    struct avl_node *right;
    struct avl_node *parent;
    int l_h;
    int r_h;
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
    *field = lr;
    lr->parent = parent;
    adopt(l, right, lr->left);
    adopt(p, left, lr->right);
    adopt(lr, left, l);
    adopt(lr, right, p);

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
    *field = rl;
    rl->parent = parent;
    adopt(r, left, rl->right);
    adopt(p, right, rl->left);
    adopt(rl, left, p);
    adopt(rl, right, r);
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
    avl_node *ava_node = rep->left ? rep->left : rep->right;
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
typedef struct {
    rb_node *root;
} rb_tree;

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
rb_tree *new_rb_tree() {
    rb_tree *tree = malloc(sizeof(rb_tree));
    if (!tree) return NULL;
    tree->root = NULL;
    return tree;
}

rb_node *red_black_search(rb_node *root, int data) {
    if (!root || root->data == data) return root;
    return red_black_search(data < root->data ? root->left : root->right, data);
}

//parent is the one that contains field
void rb_rr(rb_node *parent, rb_node **field) {
    rb_node *p = *field;
    rb_node *r = p->right;
    *field = r;
    r->parent = parent;
    adopt(p, right, r->left);
    adopt(r, left, p);
}

void rb_rl(rb_node *parent, rb_node **field) {
    rb_node *n = *field;
    rb_node *r = n->right;
    rb_node *rl = r->left;
    *field = rl;
    rl->parent = parent;
    adopt(n, right, rl->left);
    adopt(r, left, rl->right);
    adopt(rl, left, n);
    adopt(rl, right, r);
}

void rb_ll(rb_node *parent, rb_node **field) {
    rb_node *n = *field;
    rb_node *l = n->left;
    *field = l;
    l->parent = parent;
    adopt(n, left, l->right);
    adopt(l, right, n);
}

void rb_lr(rb_node *parent, rb_node **field) {
    rb_node *n = *field;
    rb_node *l = n->left;
    rb_node *lr = l->right;
    *field = lr;
    lr->parent = parent;
    adopt(n, left, lr->right);
    adopt(l, right, lr->left);
    adopt(lr, left, l);
    adopt(lr, right, n);
}

void ins_adjust(rb_node *n, rb_node **root) {
    if (!n->parent) {
        n->color = black;
        return;
    }
    if (is_black(n) || is_black(n->parent)) return;
    rb_node *grand = n->parent->parent;
    rb_node *uncle = grand->left == n->parent ? grand->right : grand->left;
    if (!is_black(uncle)) {
        n->parent->color = uncle->color = black;
        n->parent->parent->color = red;
        ins_adjust(n->parent->parent, root);
        return;
    }
    rb_node **f = field(grand) ? field(grand) : root;
    if (grand->right && n == grand->right->right) rb_rr(grand->parent, f);
    else if (grand->left && n == grand->left->right) rb_lr(grand->parent, f);
    else if (grand->left && n == grand->left->left) rb_ll(grand->parent, f);
    else rb_rl(grand->parent, f);

    (*f)->color = black;
    (*f)->left->color = red;
    (*f)->right->color = red;
}

void insert_n(rb_node **pos, rb_node *parent, int data, rb_node **root) {
    if (!(*pos)) {
        *pos = new_rb_node(data);
        (*pos)->parent = parent;
        if (!parent) (*pos)->color = black;
        ins_adjust(*pos, root);
        return;
    }
    if ((*pos)->data == data) return;
    rb_node **p = data < (*pos)->data ? &(*pos)->left : &(*pos)->right;
    insert_n(p, *pos, data, root);
}

void red_black_insert(rb_tree *tree, int data) {
    insert_n(&tree->root, NULL, data, &tree->root);
}

#include <stdio.h>
void remove_fix(rb_node *parent, rb_node **changed) {
    //sibling of changed
    int l;
    rb_node *sib = (l = &parent->left == changed) ? parent->right : parent->left;
    rb_node **f = field(parent);
    if (sib->color == red) {
        //change representation
        l ? rb_rr(parent->parent, f) : rb_ll(parent->parent, f);
        (*f)->color = black;
        parent->color = red;
        remove_fix(parent, l ? &parent->left : &parent->right);
        return;
    }

    int p_color = parent->color;
    if (!is_black(sib->left)) {
        //rotate
        if(l) {
            rb_rl(parent->parent, f);
        } else {
            rb_ll(parent->parent, f);
            sib->left->color = black;
        }
        (*f)->color = parent->color;
        parent->color = black;
    } else if (!is_black(sib->right)) {
        //rotate
        if (l) {
            rb_rr(parent->parent, f);
            sib->right->color = black;
        } else {
            rb_lr(parent->parent, f);
        }
        (*f)->color = parent->color;
        parent->color = black;
    } else if (parent->color == red) {
        //safe_comb
        sib->color = red;
        parent->color = black;
    } else {
        //unsafe_comb
        sib->color = red;
        remove_fix(parent->parent, field(parent));
    }
}

void rb_remove(rb_tree *tree, int data) {
    if (!tree->root) return;
    rb_node *n = red_black_search(tree->root, data);
    if (!n) return;
    //here we intentionally laid out the first 5 fields of 2 node type in a same way
    //so rightmost and leftmost and be reuse
    rb_node *rep = n->left ? rightmost(n->left) : n->right ? leftmost(n->right) : n;
    if (!rep->parent) {
        free(rep);
        tree->root = NULL;
        return;
    }
    n->data = rep->data;
    rb_node *ava = rep->left ? rep->left : rep->right;
    rb_node **f = field(rep);
    *f = ava;
    if (ava) ava->parent = rep->parent;
    if (rep->color == red) {
        free(rep);
        return;
    }
    if (ava) {
        assert(ava->color == red);
        ava->color = black;
        free(rep);
        return;
    }
    remove_fix(rep->parent, f);
    free(rep);
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
        rb_tree *tree = new_rb_tree(); //new
        while (1) {
            scanf("%s", cmd);
            int x;
            switch (cmd[0]) {
                case 'i': {
                    scanf("%d", &x);
                    red_black_insert(tree, x);
                    break;
                }
                case 's': {
                    scanf("%d", &x);
                    rb_balance(red_black_search(tree->root, x));
                    break;
                }
                case 'd': {
                    scanf("%d", &x);
                    rb_remove(tree, x);
                    break;
                }
                case 'e': {scanf("%d", &x);
                    if (red_black_search(tree->root, x)) {
                        printf("exist\n");
                    } else {
                        printf("Not exist\n");
                    }
                    break;
                }
                case 'q': {
                    red_black_free(tree->root);
                    free(tree);
                    return 0;
                }
            }
        }
    }
}