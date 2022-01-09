#include <stdio.h>
#include <stdlib.h>

struct {
    int last; //last position, 1 is preserved for root, not used in smmh
    unsigned int capacity; //arr[0] is counted

    int *arr;
} smmh = { .last = 1, .capacity = 8 };


int init_smmh() {
    return smmh.arr = malloc(sizeof(int) * 8); //return whether successfully initialized
}

int insert_spacing() {
    smmh.last++;
    if (smmh.last < smmh.capacity) return 1;

    smmh.capacity <<= 1;
    int *new_arr = realloc(smmh.arr, sizeof(int) * smmh.capacity);
    if (!new_arr) {
        smmh.capacity >>= 1;
        return 0;
    }
    smmh.arr = new_arr;
    return 1;
}

#define swap(a, b, typ) {typ t = (a); (a) = (b); (b) = t;}

#define is_right(x) ((x) % 2) //even number
#define is_left(x) (!is_right(x)) //odd number
#define has_grand(x) ((x) >= 4) //3rd level

#define grand_left(x) (is_right((x)/2) ? (x)/2 - 1 : (x)/2) //left of grandparent
#define grand_right(x) (is_right((x)/2) ? (x)/2 : (x)/2 + 1) //right of grandparent

//actual value compare with index, specifically designed to check the boundary of arr
#define cmp_with_index(a, op, index) ((index) >= 2 && (index) <= smmh.last ? a op smmh.arr[index] : 0)

void adjust_up(int x, int last) {
    if (is_left(last) && cmp_with_index(x, >, last + 1)) {
        swap(smmh.arr[last], smmh.arr[last+1], int);
        adjust_up(x, last+1);
    } else if (is_right(last) && cmp_with_index(x, <, last - 1)) {
        swap(smmh.arr[last], smmh.arr[last-1], int);
        adjust_up(x, last-1);
    } else if (has_grand(last) && cmp_with_index(x, <, grand_left(last))) {
        swap(smmh.arr[last], smmh.arr[grand_left(last)], int);
        adjust_up(x, grand_left(last));
    } else if (has_grand(last) && cmp_with_index(x, >, grand_right(last))) {
        swap(smmh.arr[last], smmh.arr[grand_right(last)], int);
        adjust_up(x, grand_right(last));
    }
}

void adjust_down(int x, int last) {
    if (is_left(last) && cmp_with_index(x, >, last + 1)) {
        swap(smmh.arr[last], smmh.arr[last+1], int);
        adjust_down(x, last+1);
    } else if (is_right(last) && cmp_with_index(x, <, last - 1)) {
        swap(smmh.arr[last], smmh.arr[last-1], int);
        adjust_down(x, last-1);
    } else if (is_left(last) && (cmp_with_index(x, >, last * 2) || cmp_with_index(x, >, (last + 1) * 2))) {
        int swap_index = cmp_with_index(smmh.arr[last*2], <, (last+1)*2) ? last*2 : (last+1)*2;
        swap(smmh.arr[last], smmh.arr[swap_index], int);
        adjust_down(x, swap_index);
    } else if (is_right(last) && (cmp_with_index(x, <, (last - 1) * 2 + 1) || cmp_with_index(x, <, last * 2 + 1))) {
        int swap_index = cmp_with_index(smmh.arr[(last-1)*2+1], <, last*2 + 1) ? last*2+1 : (last-1)*2+1;
        swap(smmh.arr[last], smmh.arr[swap_index], int);
        adjust_down(x, swap_index);
    } else {
        smmh.arr[last] = x;
    }
}

int smmh_insert(int x) {
    if (!insert_spacing()) return 0;

    int last = smmh.last; //last is increased by insert_spacing
    smmh.arr[last] = x;

    adjust_up(x, last);
    return 1;
}

void smmh_delete_min() {
    if (smmh.last <= 1) return;
    adjust_down(smmh.arr[smmh.last--], 2);
}

void smmh_delete_max() {
    if (smmh.last <= 2) {
        smmh.last -= (smmh.last > 1);
        return;
    }
    adjust_down(smmh.arr[smmh.last--], 3); //as if reinsert last to max position, and then adjust
}

void smmh_lv_print() {
    printf("NULL\n");
    int lv_last = 3;
    for (int current = 2; current <= smmh.last; current++) {
        printf("%d%c", smmh.arr[current], " \n"[current==smmh.last || current == lv_last]);
        lv_last |= lv_last << (current == lv_last);
    }
}

void smmh_free() { free(smmh.arr); }

int main() {
    if (!init_smmh()) return -1;
    while(1) {
        char cmd[7];
        scanf("%s", cmd);
        if (cmd[0] == 'i') {
            int x;
            scanf("%d", &x);
            smmh_insert(x);
        } else if (cmd[0] == 'd') {
            char sel[4];
            scanf("%s", sel);
            if (sel[1] == 'i') {
                //min
                smmh_delete_min();
            } else {
                smmh_delete_max();
            }
        } else if (cmd[0] == 's') {
            smmh_lv_print();
        } else if (cmd[0] == 'q') {
            smmh_free();
            return 0;
        }
    }
}