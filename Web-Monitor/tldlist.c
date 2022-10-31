#include "tldlist.h"
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
};

/**
 * @brief An iterator is an object that allows you to step through the contents of another object, by providing convenient operations for getting the first element, testing when you are done, and getting the next element if you are not.
 */
struct tlditerator {
    // make an array of pointer pointing to all the nodes inorder and store the index here.
    struct tldnode *current_node;
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

        tldlist_iter_destroy(iter);
        tldlist_iter_destroy(temp_iter);
        date_destroy(tld->begin);
        date_destroy(tld->end);
        free(tld);
        tld = NULL;
    } else if (tld != NULL) {
        free(tld);
        tld = NULL;
    }
}

// return the top level domain name of the hostname
// Works for gla.com => com
char *find_tld(char *hostname) {
    // find the last dot in the hostname and return the substring after it
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

    if (tld == NULL || tld->root == NULL)
        return 0;

    TLDIterator *iter = tldlist_iter_create(tld);
    count += iter->current_node->count;

    while ((iter->current_node = tldlist_iter_next(iter)) != NULL) {
        count += iter->current_node->count;
    }

    return count;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
    TLDIterator *iter;

    if ((iter = malloc(sizeof(TLDIterator))) != NULL) {
        iter->current_node = tld->root;

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

    if (iter == NULL || iter->current_node == NULL)
        return NULL;

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
    free(iter);
    iter = NULL;
}

char *tldnode_tldname(TLDNode *node) {
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    return node->count;
}
