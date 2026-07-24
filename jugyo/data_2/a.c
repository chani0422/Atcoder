
#include <stdio.h>
#include <stdlib.h>


typedef struct BinarySearchTree {
   int key;
   int value;
   struct BinarySearchTree *left;
   struct BinarySearchTree *right;
}BinarySearchTree;

int search(struct BinarySearchTree *r, int key) {
    BinarySearchTree *p = r;
    int cur = p->key;
    while(p != NULL) {
    cur = p->key;
    if(cur == key) return p->value;
    if(cur > key) {
        cur = p->left->key;
        p = p->left;
    }else if(cur < key) {
        cur = p->right->key;
        p = p->right;
    }
    }
    return -1;
}

struct BinarySearchTree *regist(struct BinarySearchTree *r, int key, int value) {
    BinarySearchTree *new =(BinarySearchTree *)malloc(sizeof(BinarySearchTree));
    new->key = key;
    new->value = value;
    new->left = NULL;
    new->right = NULL;
    BinarySearchTree *p = r;
    if (r == NULL) {
        /* 初回登録の場合 */
        return new;
    }
    BinarySearchTree *parent = NULL;

    while(p != NULL) {
        parent = p;
        if(p->key > key) {
            p = p->left;
        }else if(p->key < key) {
            p = p->right;
        }else {
            p->value = value;
            free(new);
            return r;
        }
    }
    if(key < parent->key) {
        parent->left = new;
    }else if(key > parent->key) {
        parent->right = new;
    }
    return r;
}

int main() {
    int n, m, i, key, value;
    struct BinarySearchTree *root = NULL;

    /* 二分探索木への登録 */
    scanf("%d", &n);


    printf("レコード数(%d)\n", n);
    for (i = 0; i < n; i++) {
        scanf("%d", &key);
        scanf("%d", &value);
        printf("キー(%d), データ(%d)を登録\n", key, value);
        if (root == NULL) {
            /* 初回登録の場合 */
            root = regist(root, key, value);
        } else {
            /* 二回目以降の登録の場合 */
            regist(root, key, value);
        }
    }

    /* 二分探索木の探索 */
    scanf("%d", &m);
    for (i = 0; i < m; i++) {
        scanf("%d", &key);
        printf("キー(%d)を探索．\n", key);
        value = search(root, key);
        if (value == -1) {
            printf("キー(%d)を持つレコードは登録されていません．\n", key);
        } else {
            printf("キー(%d)の値は%dです．\n", key, value);
        }
    }
}