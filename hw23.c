#include <stdlib.h>

typedef struct {
    int last; //last position, 1 is preserved for root, not used in smmh
    unsigned int capacity; //arr[0] is counted

    int *arr;
} SMMH;


SMMH* create_smmh() {
    SMMH *smmh = malloc(sizeof(SMMH));
    if (!smmh) return NULL;
    smmh->arr = malloc(sizeof(int) * 8);
    if (!smmh->arr) return NULL;
    smmh->last = 1;
    smmh->capacity = 8;
    return smmh;
}

int insert_spacing(SMMH *smmh) {
    smmh->last++;
    if (smmh->last < smmh->capacity) return 1;

    smmh->capacity <<= 1;
    int *new_arr = realloc(smmh->arr, sizeof(int) * smmh->capacity);
    if (!new_arr) {
        smmh->capacity >>= 1;
        return 0;
    }
    smmh->arr = new_arr;
    return 1;
}

#define swap(a, b, typ) {typ t = (a); (a) = (b); (b) = t;}

#define is_right(x) ((x) % 2) //even number
#define is_left(x) (!is_right(x)) //odd number
#define has_grand(x) ((x) >= 4) //3rd level

#define grand_left(x) (is_right((x)/2) ? (x)/2 - 1 : (x)/2) //left of grandparent
#define grand_right(x) (is_right((x)/2) ? (x)/2 : (x)/2 + 1) //right of grandparent

//actual value compare with index, specifically designed to check the boundary of arr
#define cmp_with_index(a, op, index) ((index) >= 2 && (index) <= smmh->last ? a op smmh->arr[index] : 0)

void adjust_up(SMMH *smmh, int x, int last) {
    if (is_left(last) && cmp_with_index(x, >, last + 1)) {
        swap(smmh->arr[last], smmh->arr[last+1], int);
        adjust_up(smmh, x, last+1);
    } else if (is_right(last) && cmp_with_index(x, <, last - 1)) {
        swap(smmh->arr[last], smmh->arr[last-1], int);
        adjust_up(smmh, x, last-1);
    } else if (has_grand(last) && cmp_with_index(x, <, grand_left(last))) {
        swap(smmh->arr[last], smmh->arr[grand_left(last)], int);
        adjust_up(smmh, x, grand_left(last));
    } else if (has_grand(last) && cmp_with_index(x, >, grand_right(last))) {
        swap(smmh->arr[last], smmh->arr[grand_right(last)], int);
        adjust_up(smmh, x, grand_right(last));
    }
}

void adjust_down(SMMH *smmh, int x, int last) {
    if (is_left(last) && cmp_with_index(x, >, last + 1)) {
        swap(smmh->arr[last], smmh->arr[last+1], int);
        adjust_down(smmh, x, last+1);
    } else if (is_right(last) && cmp_with_index(x, <, last - 1)) {
        swap(smmh->arr[last], smmh->arr[last-1], int);
        adjust_down(smmh, x, last-1);
    } else if (is_left(last) && (cmp_with_index(x, >, last * 2) || cmp_with_index(x, >, (last + 1) * 2))) {
        int swap_index = cmp_with_index(smmh->arr[last*2], <, (last+1)*2) ? last*2 : (last+1)*2;
        swap(smmh->arr[last], smmh->arr[swap_index], int);
        adjust_down(smmh, x, swap_index);
    } else if (is_right(last) && (cmp_with_index(x, <, (last - 1) * 2 + 1) || cmp_with_index(x, <, last * 2 + 1))) {
        int swap_index = cmp_with_index(smmh->arr[(last-1)*2+1], <, last*2 + 1) ? last*2+1 : (last-1)*2+1;
        swap(smmh->arr[last], smmh->arr[swap_index], int);
        adjust_down(smmh, x, swap_index);
    } else {
        smmh->arr[last] = x;
    }
}

int smmh_insert(SMMH *smmh, int x) {
    if (!insert_spacing(smmh)) return 0;

    int last = smmh->last; //last is increased by insert_spacing
    smmh->arr[last] = x;

    adjust_up(smmh, x, last);
    return 1;
}

void smmh_delete_min(SMMH *smmh) {
    if (smmh->last <= 1) return;
    adjust_down(smmh, smmh->arr[smmh->last--], 2);
}

void smmh_delete_max(SMMH *smmh) {
    if (smmh->last <= 2) {
        smmh->last -= (smmh->last > 1);
        return;
    }
    adjust_down(smmh, smmh->arr[smmh->last--], 3); //as if reinsert last to max position, and then adjust
}





//only to test correctness, unnecessary for test (according to the "1101 DataStructure Final" document)
#include <stdio.h>
void smmh_lv_print(SMMH *smmh) {
    printf("NULL\n");
    int lv_last = 3;
    for (int current = 2; current <= smmh->last; current++) {
        printf("%d%c", smmh->arr[current], " \n"[current==smmh->last || current == lv_last]);
        lv_last |= lv_last << (current == lv_last);
    }
}

void smmh_free(SMMH *smmh) {
    free(smmh->arr);
    free(smmh);
}

int main() {
    SMMH *smmh = create_smmh();
    if (!smmh) return -1;
    while(1) {
        char cmd[7];
        scanf("%s", cmd);
        if (cmd[0] == 'i') {
            int x;
            scanf("%d", &x);
            smmh_insert(smmh, x);
        } else if (cmd[0] == 'd') {
            char sel[4];
            scanf("%s", sel);
            if (sel[1] == 'i') {
                //min
                smmh_delete_min(smmh);
            } else {
                smmh_delete_max(smmh);
            }
        } else if (cmd[0] == 's') {
            smmh_lv_print(smmh);
        } else if (cmd[0] == 'q') {
            smmh_free(smmh);
            return 0;
        }
    }
}