#include "tldlist.h"
#include "date.c"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tldlist {
    struct tldnode *root;
    Date *begin;
    Date *end;
};

struct tldnode {
    char *hostname;
    int count;
    struct tldnode *left;
    struct tldnode *right;
    struct tldnode *parent;
    // root points to parent node, date begins with the earliest date and end with the latest date:
    // struct tldlist *list;
};

/**
 * @brief An iterator is an object that allows you to step through the contents of another object, by providing convenient operations for getting the first element, testing when you are done, and getting the next element if you are not.
 */
struct tlditerator {
    // make an array of pointer pointing to all the nodes inorder and store the index here.
    struct tldnode *current_node;
    // struct tldlist *list;
};

TLDList *tldlist_create(Date *begin, Date *end) {
    // pointer to tldlist struct:
    TLDList *tld;
    if ((tld = (TLDList *)malloc(sizeof(TLDList))) != NULL) {
        tld->root = NULL;
        tld->begin = begin;
        tld->end = end;
        return tld;
    }
    return NULL;
}

//! it's not freeing the memory in the list but freeing it from the iter?
void tldlist_destroy(TLDList *tld) {

    if (tld != NULL || tld->root != NULL) {

        TLDIterator *iter = tldlist_iter_create(tld);
        TLDIterator *temp_iter = tldlist_iter_create(tld);

        temp_iter->current_node = tldlist_iter_next(temp_iter);

        while (iter->current_node != NULL) {

            // double check that left and right nodes are freed:
            if (iter->current_node->left != NULL) {
                free(iter->current_node->left);
                iter->current_node->left = NULL;
            }
            if (iter->current_node->right != NULL) {
                free(iter->current_node->right);
                iter->current_node->right = NULL;
            }
            // make parent node point to NULL children:
            iter->current_node->left = NULL;
            iter->current_node->right = NULL;

            free(iter->current_node);
            iter->current_node = NULL;

            iter = temp_iter;
            temp_iter->current_node = tldlist_iter_next(temp_iter);
        }

        // tldlist_iter_destroy(iter);
        // tldlist_iter_destroy(temp_iter);
        date_destroy(tld->begin);
        date_destroy(tld->end);
        free(tld);
        tld = NULL;
    } else if (tld != NULL) {
        free(tld);
        tld = NULL;
    }
}

// return pointer to next node (post order traversal) or NULL if there is no next node:
// TLDNode *find_next_node(TLDNode *current_node) {
//     if (current_node->parent == NULL){
//         return NULL;
//     }

//     if (current_node->parent->left ==current_node){
//         return find_next_node(current_node->parent->right);

//     }
//     while (current_node != NULL) {
//         if (current_node->left != NULL) {
//             current_node = current_node->left;
//         } else if (current_node->right != NULL) {
//             current_node = current_node->right;
//         } else {
//             return current_node;
//         }
//     }
//     return NULL;
// }

// void tldlist_destroy(TLDList *tld) {
//     if (tld == NULL) {
//         return;
//     }
//     if (tld->root == NULL) {
//         free(tld);
//         tld = NULL;
//         return;
//     }

//     TLDNode *current_node = tld->root;
//     TLDNode *temp_node;

//     while (current_node != NULL) {
//         if (current_node->left != NULL) {
//             current_node = current_node->left;
//         } else if (current_node->right != NULL) {
//             current_node = current_node->right;
//         } else {
//             temp_node = find_next_node(current_node);
//             if (temp_node == NULL) {
//                 free(current_node);
//                 current_node = NULL;
//                 free(tld);
//                 tld = NULL;
//                 return;
//             }

//         }
//     }
// }

// return the top level domain name of the hostname
// Works for gla.com => com
char *find_tld(char *hostname) {
    char *tld = hostname;
    char *temp = hostname;
    while (*temp != '\0') {
        if (*temp == '.') {
            tld = temp + 1;
        }
        temp++;
    }
    return tld;
}

int tldnode_create(char *hostname, TLDNode *parent) {
    // create a new node
    struct tldnode *new_node;
    if ((new_node = (struct tldnode *)malloc(sizeof(struct tldnode))) != NULL) {
        new_node->hostname = hostname;
        new_node->count = 1;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = parent;
        return 1;
    }
    return 0;
}

// don't need the date as an argument as it was already checked in tldlist_add():
// return 0 on failure, 1 on success
TLDNode *tldlist_addbynode(TLDNode *root, char *hostname) {

    int add_status = 0;
    TLDNode *new_node = NULL;

    if (root != NULL) {

        int compare_status = strcmp(hostname, tldnode_tldname(root));

        if (compare_status == 0) {
            // if this is the node we want:
            root->count++;
            add_status = 1;

        } else if (compare_status < 0) {
            // go node to go left, recursive call and assign previous node as its parent:
            new_node = tldlist_addbynode(root->left, hostname);
            if (new_node != NULL) {
                root->left = new_node;
                new_node->parent = root;
                add_status = 1;
            }

        } else {
            // go to right:
            new_node = tldlist_addbynode(root->right, hostname);
            if (new_node != NULL) {
                root->right = new_node;
                new_node->parent = root;
                add_status = 1;
            }
        }

        // if root is null, make it:
    } else if ((root = (TLDNode *)malloc(sizeof(TLDNode))) != NULL) {
        root->hostname = hostname;
        root->count = 1;
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;
        add_status = 1;
    }

    if (add_status == 1) {
        return root;
    }
    return NULL;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d) {

    // if d is not within the range:
    if (date_compare(d, tld->begin) < 0 || date_compare(d, tld->end) > 0) {
        return 0;
    }

    hostname = find_tld(hostname);

    // hostname is case insensitive:
    // lowercase hostname and store it in a new variable which size is the same as the original:
    //! how can I free this memory?
    char *lower_hostname = (char *)malloc(strlen(hostname) + 1);
    int i = 0;
    while (hostname[i] != '\0') {
        lower_hostname[i] = tolower(hostname[i]);
        i++;
    }
    lower_hostname[i] = '\0';

    int add_status = 0;

    // if this is the first node:
    if (tld->root == NULL) {

        // if root is null, make it:
        if ((tld->root = (TLDNode *)malloc(sizeof(TLDNode))) != NULL) {
            tld->root->hostname = lower_hostname;
            tld->root->count = 1; // first node added
            tld->root->left = NULL;
            tld->root->right = NULL;
            tld->root->parent = NULL;
            add_status = 1;
        }
    } else {

        TLDNode *added_node = tldlist_addbynode(tld->root, lower_hostname);
        if (added_node != NULL) {
            add_status = 1;
        }
    }

    return add_status;
}

long tldlist_count(TLDList *tld) {

    long count = 0;

    if (tld == NULL || tld->root == NULL) {
        return 0;
    }

    TLDIterator *iter = tldlist_iter_create(tld);
    count += iter->current_node->count;

    while ((iter->current_node = tldlist_iter_next(iter)) != NULL) {
        count += iter->current_node->count;
    }

    return count;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
    TLDIterator *iter;
    // TLDNode *node = tld->root;

    if ((iter = malloc(sizeof(TLDIterator))) != NULL) {
        iter->current_node = tld->root;
        // node = tld->root;

        // Post order => Left, Right, Root
        // find the most left node:
        while (iter->current_node->left != NULL) {
            iter->current_node = iter->current_node->left;
        }

        // find the most right node:
        while (iter->current_node->right != NULL) {
            iter->current_node = iter->current_node->right;
        }

        return iter;
    }
    return NULL;
}

// find the next logical successor from current node:
// return iter with next node, or NULL if there is no next node:
// Post order = > Left, Right, Root
TLDNode *tldlist_iter_next(TLDIterator *iter) {

    if (iter == NULL || iter->current_node == NULL) {
        return NULL;
    }

    // find parent
    if (iter->current_node->parent != NULL) {
        // if this node is its left child, check if right child exist
        if (iter->current_node == iter->current_node->parent->left) {
            iter->current_node = iter->current_node->parent;
            // if right child exist, go there:
            if (iter->current_node->right != NULL) {
                iter->current_node = iter->current_node->right;
                // find the most left node:
                while (iter->current_node->left != NULL) {
                    iter->current_node = iter->current_node->left;
                }
                // if we reached the left most child in this subtree, go to right child if it exists:
                while (iter->current_node->right != NULL) {
                    iter->current_node = iter->current_node->right;
                }
            }

            // if no, stay on parent node
        } else if (iter->current_node == iter->current_node->parent->right) {
            // if this node is its right child, go to parent
            iter->current_node = iter->current_node->parent;
        }

        return iter->current_node;
    }
    return NULL;
}

void tldlist_iter_destroy(TLDIterator *iter) {
    free(iter->current_node);
    iter->current_node = NULL;
    // free(iter);
    // iter = NULL;
}

char *tldnode_tldname(TLDNode *node) {
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    return node->count;
}

//------- tests:
// makes a list in range "20/12/2018" - "15/10/2019"
TLDList *make_list() {
    Date *d1 = date_create("20/12/2018");
    Date *d2 = date_create("15/10/2019");
    TLDList *tldlist = tldlist_create(d1, d2);

    return tldlist;
}

// add random nodes within and outside the range:
void add_random_nodes(TLDList *tldlist) {
    Date *d3 = date_create("21/12/2018");
    Date *d4 = date_create("14/10/2019");
    Date *d5 = date_create("13/10/2019");
    Date *d6 = date_create("01/10/2019");
    // outwith the range:
    Date *d7 = date_create("16/10/2019");
    Date *d8 = date_create("19/12/2018");
    Date *d9 = date_create("19/12/2017");
    Date *d10 = date_create("20/12/2020");
    // add to tldlist random tlds:
    //      com
    //    au   net
    //            sy
    // within the range:
    tldlist_add(tldlist, "gla.COM", d3);
    tldlist_add(tldlist, "gla.ac.net", d4);
    tldlist_add(tldlist, "gla.uk.com", d5);
    tldlist_add(tldlist, "gla.au", d6);
    tldlist_add(tldlist, "gla.sy", d6);
    // outwith the range:
    tldlist_add(tldlist, "gla.uk.ac.net", d7);
    tldlist_add(tldlist, "gla.au", d8);
    tldlist_add(tldlist, "gla.uk.ac.net", d9);
    tldlist_add(tldlist, "gla.uk.ac.sy", d10);
}

void test_tldlist_create() {
    printf("test_tldlist_create...\n");
    Date *d1 = date_create("20/12/2018");
    Date *d2 = date_create("15/10/2019");
    TLDList *tldlist = tldlist_create(d1, d2);

    printf("\tbegin date: %d/%d/%d\t\t%s\n", tldlist->begin->day, tldlist->begin->month, tldlist->begin->year, (tldlist->begin->day == 20 && tldlist->begin->month == 12 && tldlist->begin->year == 2018) ? "PASS" : "FAIL");
    printf("\tend date: %d/%d/%d\t\t%s\n", tldlist->end->day, tldlist->end->month, tldlist->end->year, (tldlist->end->day == 15 && tldlist->end->month == 10 && tldlist->end->year == 2019) ? "PASS" : "FAIL");
}

void test_tldlist_add() {
    printf("test_tldlist_add...\n");

    TLDList *tldlist = make_list();
    // random within and outwith d1 and d2 dates:
    // within the range:
    Date *d3 = date_create("21/12/2018");
    Date *d4 = date_create("14/10/2019");
    Date *d5 = date_create("13/10/2019");
    Date *d6 = date_create("01/10/2019");
    // outwith the range:
    Date *d7 = date_create("16/10/2019");
    Date *d8 = date_create("19/12/2018");
    Date *d9 = date_create("19/12/2017");
    Date *d10 = date_create("20/12/2020");
    // add to tldlist random tlds:
    //  com
    //    au   net
    //            sy
    // within the range:
    tldlist_add(tldlist, "gla.COM", d3);
    tldlist_add(tldlist, "gla.ac.net", d4);
    tldlist_add(tldlist, "gla.uk.com", d5);
    tldlist_add(tldlist, "gla.au", d6);
    tldlist_add(tldlist, "gla.sy", d6);
    // outwith the range:
    tldlist_add(tldlist, "gla.uk.ac.net", d7);
    tldlist_add(tldlist, "gla.au", d8);
    tldlist_add(tldlist, "gla.uk.ac.net", d9);
    tldlist_add(tldlist, "gla.uk.ac.sy", d10);

    printf("TLDList root hostname: %s\t\t\t%s\n", tldlist->root->hostname, (strcmp(tldlist->root->hostname, "com") == 0) ? "PASS" : "FAIL");
    printf("TLDList root count: %d\t\t\t\t%s\n", tldlist->root->count, (tldlist->root->count == 2) ? "PASS" : "FAIL");
    printf("TLDList root left hostname: %s\t\t\t%s\n", tldlist->root->left->hostname, (strcmp(tldlist->root->left->hostname, "au") == 0) ? "PASS" : "FAIL");
    printf("TLDList root left count: %d\t\t\t%s\n", tldlist->root->left->count, (tldlist->root->left->count == 1) ? "PASS" : "FAIL");
    printf("TLDList root right hostname: %s\t\t%s\n", tldlist->root->right->hostname, (strcmp(tldlist->root->right->hostname, "net") == 0) ? "PASS" : "FAIL");
    printf("TLDList root right count: %d\t\t\t%s\n", tldlist->root->right->count, (tldlist->root->right->count == 1) ? "PASS" : "FAIL");
    printf("TLDList root right right hostname: %s\t\t%s\n", tldlist->root->right->right->hostname, (strcmp(tldlist->root->right->right->hostname, "sy") == 0) ? "PASS" : "FAIL");
    printf("TLDList root right right count: %d\t\t%s\t\t%s\n", tldlist->root->right->right->count, (tldlist->root->right->right->count == 1) ? "PASS" : "FAIL", (tldlist->root->right->right->count == 1) ? "" : "FAIL");
}

// test tldlist_iter_create
void test_iter_create() {
    // test_tldlist_add(tldlist);
    TLDList *list = make_list();
    add_random_nodes(list);

    printf("test_iter_create...\n");

    TLDIterator *iter = tldlist_iter_create(list);

    printf("iter->current_node: %s with count: %d\t\t%s\n", iter->current_node->hostname, iter->current_node->count, (strcmp(iter->current_node->hostname, "au") == 0 && iter->current_node->count == 1) ? "PASS" : "FAIL");
}

// test tldlist_iter_next
void test_iter_next() {
    // test_tldlist_add(tldlist);
    TLDList *list = make_list();
    add_random_nodes(list);

    printf("test_iter_next...\n");

    TLDIterator *iter = tldlist_iter_create(list);

    tldlist_iter_next(iter);

    printf("next iter->current_node: %s with count: %d\t%s\n", iter->current_node->hostname, iter->current_node->count, (strcmp(iter->current_node->hostname, "sy") == 0 && iter->current_node->count == 1) ? "PASS" : "FAIL");
}

// test tldlist_destroy:
void test_tldlist_destroy_one_node() {
    // TLDList *list = make_list();
    // add_random_nodes(list);
    // TLDNode *root = list->root;
    // TLDNode *left = list->root->left;
    // TLDNode *right = list->root->right;
    // TLDNode *right_right = list->root->right->right;
    // // left left should already be NULL:
    // TLDNode *left_left = list->root->left->left;

    // make a list with 1 node only:
    Date *begin = date_create("01/01/2018");
    Date *end = date_create("01/01/2019");
    TLDList *list = tldlist_create(begin, end);
    Date *d1 = date_create("05/10/2018");
    tldlist_add(list, "gla.ac.uk", d1);

    printf("test_tldlist_destroy...\n");

    tldlist_destroy(list);

    printf("list after destroying: %p\t\t\t%s\n", list, (list == NULL) ? "PASS" : "FAIL");
    printf("list->root: %p\t\t\t\t%s\n", list->root, (list->root == NULL) ? "PASS" : "FAIL");
    printf("list->begin: %p\t\t\t\t%s\n", list->begin, (list->begin == NULL) ? "PASS" : "FAIL");
    printf("list->end: %p\t\t\t\t%s\n", list->end, (list->end == NULL) ? "PASS" : "FAIL");
    printf("list->root->parent: %p\t\t\t%s\n", list->root->parent, (list->root->parent == NULL) ? "PASS" : "FAIL");
    printf("list->root->left: %p\t\t\t%s\n", list->root->left, (list->root->left == NULL) ? "PASS" : "FAIL");
    printf("list->root->right: %p\t\t\t\t%s\n", list->root->right, (list->root->right == NULL) ? "PASS" : "FAIL");

    // test if list and nodes are all NULL:
    // printf("list after destroy: %p\t\t\t\t\t%s\n", list, (list == NULL) ? "PASS" : "FAIL");
    // printf("root after destroy: %p\t\t\t\t\t%s\n", root, (root == NULL) ? "PASS" : "FAIL");
    // printf("left after destroy: %p\t\t\t\t\t%s\n", left, (left == NULL) ? "PASS" : "FAIL");
    // printf("right after destroy: %p\t\t\t\t\t%s\n", right, (right == NULL) ? "PASS" : "FAIL");
    // printf("right_right after destroy: %p\t\t\t\t%s\n", right_right, (right_right == NULL) ? "PASS" : "FAIL");
    // printf("left_left after destroy: %p\t\t\t\t%s\n", left_left, (left_left == NULL) ? "PASS" : "FAIL");
}

void test_find_ltd() {
    printf("test_find_tld...\n");

    char *tld = find_tld("gla.ac.UK");

    int compare = strcmp(tld, "UK");

    printf("tld: %s\t\t\t\t\t\t%s\n", tld, (compare == 0) ? "PASS" : "FAIL");
}

void test_iter_destroy(){
    TLDList *list = make_list();
    add_random_nodes(list);

    printf("test_iter_destroy...\n");

    TLDIterator *iter = tldlist_iter_create(list);
    TLDNode *current_node = iter->current_node;

    tldlist_iter_destroy(iter);

    printf("iter after destroy: %p\t\t\t\t%s\n", iter, (iter == NULL) ? "PASS" : "FAIL");
    printf("current_node after destroy: %p\t\t\t%s\n", current_node, (current_node == NULL) ? "PASS" : "FAIL");

}

    int
    main(int argc, char const *argv[]) {
    test_tldlist_create();

    test_tldlist_add();

    test_iter_create();

    test_iter_next();

    test_tldlist_destroy_one_node();

    test_find_ltd();

    test_iter_destroy();

    return 0;
}
