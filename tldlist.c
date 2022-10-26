#include "tldlist.h"
#include "date.h"
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

void tldlist_destroy(TLDList *tld) {

    if (tld != NULL || tld->root != NULL) {

        TLDIterator *iter = tldlist_iter_create(tld);
        TLDIterator *temp_iter = tldlist_iter_create(tld);

        temp_iter->current_node = tldlist_iter_next(temp_iter);

        while (iter->current_node != NULL) {
            // date_destroy(iter->current_node->list->begin);
            // date_destroy(iter->current_node->list->end);
            // tldlist_destroy(iter->current_node->list);

            // double check that left and right nodes are freed:
            if (iter->current_node->left != NULL) {
                free(iter->current_node->left);
            }
            if (iter->current_node->right != NULL) {
                free(iter->current_node->right);
            }

            free(iter->current_node);
            iter->current_node = NULL;

            iter = temp_iter;
            temp_iter->current_node = tldlist_iter_next(temp_iter);
        }

        tldlist_iter_destroy(iter);
        tldlist_iter_destroy(temp_iter);
        // free(tld);
        // tld = NULL;
    }
    //! possible memory leak
}

// don't need the date as an argument as it was already checked in tldlist_add():
// return 0 on failure, 1 on success
int tldlist_addbynode(TLDNode *root, char *hostname) {
    int add_status;
    if (root != NULL) {

        int compare_status = strcmp(hostname, tldnode_tldname(root));
        int add_status = 0;

        if (compare_status == 0) {
            // if this is the node we want:
            root->count++;
            add_status = 1;

        } else if (compare_status < 0) {
            // go to left:
            add_status = tldlist_addbynode(root->left, hostname);
            if (add_status)
                root->left->parent = root;

        } else {
            // go to right:
            add_status = tldlist_addbynode(root->right, hostname);
            if (add_status)
                root->right->parent = root;
        }

        // if root is null, make it:
    } else if ((root = (TLDNode *)malloc(sizeof(TLDNode))) != NULL) {
        root->hostname = hostname;
        root->count = 1; // first node
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;
        add_status = 1;
    }

    return add_status;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d) {

    // if d is not within the range:
    if (date_compare(d, tld->begin) < 0 || date_compare(d, tld->end) > 0) {
        return 0;
    }

    // hostname is case insensitive:
    for (int i = 0; i < strlen(hostname); i++) {
        hostname[i] = tolower(hostname[i]);
    }

    int add_status = tldlist_addbynode(tld->root, hostname);

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
        while ((iter->current_node = iter->current_node->left) != NULL)
            ;

        // find the most right node:
        while ((iter->current_node = iter->current_node->right) != NULL)
            ;

        return iter;
    }
    return NULL;
}

// find the next logical successor from current node:
// return iter with next node, or NULL if there is no next node:
TLDNode *tldlist_iter_next(TLDIterator *iter) {
    //! Post order = > Left, Right, Root

    if (iter == NULL || iter->current_node == NULL) {
        return NULL;
    }

    // find parent
    // if this node is its left child, check if right child exist
    // if yes, go there
    // if no, stay on parent node
    // if this node is its right child, go to parent
}
//     int left_found = 0;
//     int right_found = 0;

//     // printPostorder(node->left);
//     while (iter->current_node->left != NULL) {
//         iter->current_node = iter->current_node->left;
//         left_found = 1;
//     }

//     // printPostorder(node->right);
//     while (iter->current_node->right != NULL) {
//         iter->current_node = iter->current_node->right;
//         right_found = 1;
//     }

//     // if left and right nodes are not found then check the parent:
//     // if parent is not null:
//     if (!left_found && !right_found && iter->current_node->parent != NULL) {
//         // if parent's left node is the current node:
//         if (iter->current_node->parent->left == iter->current_node) {
//             // if parent's right node is not null:
//             if (iter->current_node->parent->right != NULL) {
//                 iter->current_node = iter->current_node->parent->right;
//                 return iter->current_node;
//             }
//         }
//     } else
//         return NULL;

//     // printf("%d ", node->data);
//     return iter->current_node;
// }

// if there is a left node:
//  if (iter->current_node->left != NULL) {
//      iter->current_node = iter->current_node->left;
//      // find the most right node:
//      while ((iter->current_node = iter->current_node->right) != NULL)
//          ;
//      return iter->current_node;
//  }

// if there is a right node:
// if (iter->current_node->right != NULL) {
//     iter->current_node = iter->current_node->right;

// find the most left node:
// while ((iter->current_node = iter->current_node->left) != NULL)
//     ;

//     return iter->current_node;
// }

// if there is no right node:
// find the first parent that is a left node:
// while (iter->current_node->parent != NULL &&
//        iter->current_node->parent->right == iter->current_node) {
//     iter->current_node = iter->current_node->parent;
// }

// if there is no parent:
// if (iter->current_node->parent == NULL) {
//     iter->current_node = NULL;
//     return NULL;
// }

// if there is a parent:
//     iter->current_node = iter->current_node->parent;

//     return iter->current_node;

// }

// TLDNode *node = iter->current_node;

// tldlist_iter_next_helper(node);
// }

// int cmp = strcmp(node->hostname, iter->current_node->hostname);

// if (cmp == 0) {
// no more nodes to return
// return NULL;
// }

// if (node->right != NULL) {
//     node = node->right;
//     while (node->left != NULL) {
//         node = node->left;
//     }
//     return node;
// }

// if we need to find parent node:
// if (node->hostname->) {
// }
// }

void tldlist_iter_destroy(TLDIterator *iter) {
    free(iter->current_node);
    iter->current_node = NULL;
    free(iter);
    iter = NULL;
}

char *tldnode_tldname(TLDNode *node) {
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    return node->count;
}
