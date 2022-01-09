#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Fibonacci heap
typedef struct Node_t {
    int key;
    int data;

    int degree;
    int child_cut;
    struct Node_t *child;
    struct Node_t *parent;

    //Double link is used.
    struct Node_t *next;
    struct Node_t *prev;
} FiNode;

struct {
    FiNode *min;
    int nodes_cnt;
    //record log(n) to avoid calculation
    int max_deg;
} heap = {
    .min=NULL, .nodes_cnt=0, .max_deg=0
};
//tool
FiNode *meld(FiNode *, FiNode *);
FiNode *find_min(FiNode *);
FiNode *find(FiNode *, int, int);
//insert
void fiheap_insert(int, int);
FiNode *new_node(int, int);
//del
FiNode *fiheap_del(int, int);
void cas_cut(FiNode *);
FiNode *fiheap_del_min();
void clr_parent(FiNode *);
void merge_same_degree();
void merge_tr(FiNode *s, FiNode *n, FiNode *[]);
FiNode *merge(FiNode *, FiNode *);
//link all nodes in array, and attach to heap.min
void relink(FiNode *[], int max_deg);
//decrease
void fiheap_decr(int, int, int amount);
//freeing
void fiheap_free();
void finode_link_free(FiNode *);
void finode_free(FiNode *);

//make n2 next of n1, note it's not insert, so n1->next and n2->prev will be thrown away
//link(a->prev, a->next) has equal effect as forming a list containing only a
#define link(n1, n2) {(n1)->next=(n2);(n2)->prev=(n1);}
#define swap(a, b, typ) {typ t = (a); (a) = (b); (b) = t;}
//n is the only node in its list
#define alone(n) ((n)->next == (n))

//list1 must not be NULL
FiNode *meld(FiNode *list1, FiNode *list2) {
//    assert(list1);
    if (!list2) return list1;
    link(list2->prev, list1->next);
    link(list1, list2);
    return list1;
}

FiNode *find(FiNode *l, int key, int value) {
    if (!l || (l->key == key && l->data == value)) return l;
    l->prev->next = NULL;
    FiNode *ans = find(l->next, key, value);
    l->prev->next = l;
    return ans ? ans : find(l->child, key, value);
}

FiNode *find_min(FiNode *n) {
    if (!n) return NULL;
    n->prev->next = NULL;
    FiNode *min = find_min(n->next);
    n->prev->next = n;
    return !min ? n : min->key < n->key ? min : n;
}

void fiheap_insert(int key, int data) {
    //1. acquire new node
    FiNode *node = new_node(key, data);
    if (!node) return; //fail

    //2. meld node with top-level list and update min
    heap.min = find_min(meld(node, heap.min));
    //3. update nodes count
    heap.nodes_cnt++;
    //4.update max possible degree
    int mask = -1 << heap.max_deg;
    heap.max_deg += (mask & heap.nodes_cnt)!=0;
}

FiNode *new_node(int key, int data) {
    FiNode *node = malloc(sizeof(FiNode));
    if (!node) return NULL;
    node -> key = key;
    node -> data = data;
    node -> degree = 0;
    node -> child_cut = 0;
    node -> child = NULL;
    node -> parent = NULL;
    link(node, node);

    return node;
}

FiNode *fiheap_del(int key, int value) {
    //1. find the node to be deleted
    FiNode *target = find(heap.min, key, value); //find will return NULL if heap.min is null
    if (!target) return NULL;
    FiNode *min = heap.min;
    if (target == min) return fiheap_del_min();
    //2. update nodes count
    heap.nodes_cnt--;

    //3. remove target from list
    FiNode *parent = target->parent;
    if (alone(target)) {
        //only one in list, thus must have a parent, otherwise it will be min
        parent->child = NULL;
    } else {
        link(target->prev, target->next);
        if (parent) parent->child = target->next;
    }

    //4. melding child into top-level list, update min pointer
    clr_parent(target->child);
    heap.min = find_min(meld(heap.min, target->child));

    //5. do or update cascading cut
    if (parent) {
        if (parent->child_cut) {
            cas_cut(parent);
        } else {
            parent->child_cut = 1;
        }
    }

    return target;
}

void cas_cut(FiNode *target) {
    if (!target->parent) return; //in top-level list, cascading cut has no effect
    //1. remove target from list
    target->parent->child = alone(target) ? NULL : target->next;
    if (!alone(target)) link(target->prev, target->next);

    //2. make target itself a list
    FiNode *parent = target->parent;
    target->parent = NULL;
    link(target, target);

    //3. melding target to top-level list
    heap.min = find_min(meld(heap.min, target));
    //4. update cascading cut info on target
    target->child_cut = 0;

    //5. upward cascading cut
    if (parent->child_cut) {
        cas_cut(parent);
    } else {
        parent->child_cut = 1;
    }
}

//since it requires returning, freeing node is caller's duty
FiNode *fiheap_del_min() {
    if (!heap.min) return NULL;

    //1. update nodes count
    FiNode *min = heap.min;
    heap.nodes_cnt--;
    //2. clear the parent of children of min, preparing for melding with top-level list
    clr_parent(min->child);

    //3. remove min from top-level list, meld children with the list if necessary
    if (alone(min)) {
        heap.min = find_min(min -> child);
        if (!min -> child) return min; //just return, since there is no children or siblings
    } else {
        link(min->prev, min->next); //remove min from list
        heap.min = meld(min->next, min->child);
    }

    //4. do merging
    merge_same_degree();
    //5. update min after merge
    heap.min = find_min(heap.min);

    return min;
}

void clr_parent(FiNode *n) {
    if (!n) return;
    n->parent = NULL;
    n->prev->next = NULL;
    clr_parent(n->next);
    n->prev->next = n;
}

void merge_same_degree() {
    FiNode *deg_table[heap.max_deg + 1];
    memset(deg_table, 0, sizeof(FiNode *) * (heap.max_deg + 1));

    FiNode *pre = heap.min;
    deg_table[pre->degree] = pre;
    //1. traversely merge the list, put merged nodes into table
    merge_tr(pre, pre->next, deg_table);
    //2. link all nodes in table together, and attach the appropriate node to min
    relink(deg_table, heap.max_deg);
}

void merge_tr(FiNode *s, FiNode *n, FiNode *table[]) {
    if (n == s) return;
    FiNode *tar;
    while ((tar = table[n->degree]) && tar != n) {
        n = merge(n, tar);
        s = s==tar ? n : s; //update s if s is tar because s might be merged under n and the loop won't terminate
        table[n->degree-1]=NULL;
    }
    table[n->degree] = n;
    merge_tr(s, n->next, table);
}

//a and b must be in top most list
FiNode *merge(FiNode *a, FiNode *b) {
//    assert(a != b);
//    assert(a->parent == NULL);
//    assert(b->parent == NULL);

    if (a->key > b->key) {
        swap(a, b, FiNode *);
    }
    link(b->prev, b->next);
    link(b, b);
    a->child = meld(b, a->child);
    b->parent = a;
    a->degree++;
    return a;
}

void relink(FiNode *deg_table[], int max_deg) {
    int i = 0;
    for (; i <= max_deg && !deg_table[i]; i++) ; //find first index with value
    if (i > max_deg) return; //nothing to link
    FiNode *start = deg_table[i];
    FiNode *previous = deg_table[i];

    for (i++; i <= max_deg; i++) {
        FiNode *n = deg_table[i];
        if (!n) continue;
        n->parent = NULL;
        link(previous, n);
        previous = n;
    }
    link(previous, start);
    heap.min = find_min(start);
}

void fiheap_decr(int key, int value, int amount) {
    //1. find the node to be decreased
    FiNode *target = find(heap.min, key, value);
    if (!target) return;

    //2. decrease
    target -> key -= amount;
    //3. if target has a smaller key than parent, meld it with top-level list
    if (target->parent && target->key < target->parent->key) {
        if (alone(target)) {
            target->parent->child = NULL;
        } else {
            //4. remove target from its list
            link(target->prev, target->next);
            target->parent->child = target->next;
        }
        link(target, target);
        //5. meld target with top-level list
        meld(heap.min, target);

        //6. cascading cut
        if (target->parent->child_cut) {
            cas_cut(target->parent);
        } else {
            target->parent->child_cut = 1;
        }
        target->parent = NULL; //moved to top-level list, shouldn't have a parent
    }
    //7. update min
    heap.min = find_min(heap.min);
}

void fiheap_free() {
    finode_link_free(heap.min);
}

void finode_link_free(FiNode *n) {
    if (!n) return;
    n->prev->next = NULL;
    finode_link_free(n->next);
    finode_free(n);
}

void finode_free(FiNode *node) {
    finode_link_free(node->child);
    free(node);
}


int main() {
    char in[9];
    while (1) {
        scanf("%s", in);
        int x, val;
        if (in[2] == 's') {
            scanf("%d %d", &x, &val);
            fiheap_insert(x, val);
        } else if (in[2] == 't') {
            FiNode *min = fiheap_del_min();
            if (min) {
                printf("(%d)%d\n", min->key, min->data);
                free(min);
            }
        } else if (in[2] == 'l') {
            scanf("%d %d", &x, &val);
            fiheap_del(x, val);
        } else if (in[2] == 'c') {
            int y;
            scanf("%d %d %d", &x, &val, &y);
            fiheap_decr(x, val, y);
        } else if (in[2] == 'i') {
            fiheap_free();
            return 0;
        }
    }
}