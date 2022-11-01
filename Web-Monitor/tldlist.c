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

// An iterator is an object that allows you to step through the contents of another object, by providing convenient operations for getting the first element, testing when you are done, and getting the next element if you are not.
struct tlditerator {
    struct tldnode *current_node;
    struct tldnode *list_root;
};

TLDList *tldlist_create(Date *begin, Date *end) {
    TLDList *tld = NULL;
    if ((tld = (TLDList *)malloc(sizeof(TLDList)))) {
        tld->root = NULL;
        tld->begin = begin;
        tld->end = end;
        return tld;
    }
    return NULL;
}

void tldlist_destroy(TLDList *tld) {

    // if tld and root are not null:
    if (tld && tld->root) {

        TLDIterator *iter = tldlist_iter_create(tld);
        TLDIterator *temp_iter = tldlist_iter_create(tld);
        
        iter->current_node = tldlist_iter_next(iter);
        temp_iter->current_node = tldlist_iter_next(temp_iter);
        temp_iter->current_node = tldlist_iter_next(temp_iter);

        while (iter->current_node) {
            // make parent node point to NULL children:
            iter->current_node->left = NULL;
            iter->current_node->right = NULL;
            // free hostname:
            free(iter->current_node->hostname);
            iter->current_node->hostname = NULL;
            // free node:
            free(iter->current_node);
            iter->current_node = NULL;
            // move to next node:
            iter->current_node = temp_iter->current_node;
            temp_iter->current_node = tldlist_iter_next(temp_iter);
        }

        // free the tldlist itself:
        free(tld);
        tld = NULL;
        // free the iterators:
        tldlist_iter_destroy(iter);
        tldlist_iter_destroy(temp_iter);

        // else if tld is not null:
    } else if (tld) {
        free(tld);
        tld = NULL;
    }
}

// return the top level domain name of the hostname
// for gla.ac.uk => uk
char *find_tld(char *hostname) {
    // find the top level domain in the hostname:
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
    struct tldnode *new_node = NULL;
    if ((new_node = (struct tldnode *)malloc(sizeof(struct tldnode)))) {
        new_node->hostname = hostname;
        new_node->count = 1;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = parent;
        return 1;
    }
    return 0;
}

// Don't need the date here as it was already checked in tldlist_add():
TLDNode *tldlist_addbynode(TLDNode *root, char *hostname) {
    // if root is null (base case):
    if (!root) {
        if ((root = (TLDNode *)malloc(sizeof(TLDNode)))) {
            printf("root now is null, creating new node for %s\n", hostname);
            root->hostname = hostname;
            root->count = 1;
            root->left = NULL;
            root->right = NULL;
            root->parent = NULL;
            // Only return the node in this case. Otherwise, return NULL
            return root;
        }
    }

    // if root is not null:
    int add_status = 0;

    int compare_status = strcmp(hostname, root->hostname);
    if (compare_status == 0) {
        // if this is the node we want:
        // increment the count:
        root->count++;
        add_status = 1;

        // free the hostname as it is not needed:
        free(hostname);
        hostname = NULL;

    } else if (compare_status < 0) {
        // go node to go left, recursive call and assign previous node as its parent:
        TLDNode *left_child = tldlist_addbynode(root->left, hostname);
        // if add has been successful:
        if (left_child) {
            root->left = left_child;
            left_child->parent = root;
            add_status = 1;
        }

    } else if (compare_status > 0) {
        // go to right:
        TLDNode *right_child = tldlist_addbynode(root->right, hostname);
        // if add has been successful:
        if (right_child) {
            // link right child to new node:
            root->right = right_child;
            // link the new node to the parent:
            right_child->parent = root;
            add_status = 1;
        }
    }
    return root;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d) {

    // if d is not within the range:
    if (date_compare(d, tld->begin) < 0 || date_compare(d, tld->end) > 0) {
        return 0;
    }

    char *only_tld = find_tld(hostname);

    char *lower_hostname = (char *)malloc(strlen(only_tld) + 1);
    // hostname is case insensitive:
    // lowercase hostname and store it in a new variable which size is the same as the original:
    int i = 0;
    while (only_tld[i] != '\0') {
        lower_hostname[i] = tolower(only_tld[i]);
        i++;
    }
    lower_hostname[i] = '\0';
    printf("only_tld after lowering: %s\n", lower_hostname);

    int add_status = 0;

    // if this is the first node:
    if (!(tld->root)) {
        // if root is null, make it:
        if ((tld->root = (TLDNode *)malloc(sizeof(TLDNode)))) {
            tld->root->hostname = lower_hostname;
            tld->root->count = 1; // first node added
            tld->root->left = NULL;
            tld->root->right = NULL;
            tld->root->parent = NULL;
            add_status = 1;
        }
    } else {
        TLDNode *added_node = tldlist_addbynode(tld->root, lower_hostname);

        // if the node was added:
        if (added_node) {
            add_status = 1;
        }
    }

    return add_status;
}

long tldlist_count(TLDList *tld) {

    if (!(tld) || !(tld->root)) {
        return 0;
    }

    long count = 0;
    TLDIterator *iter = NULL;
    iter = tldlist_iter_create(tld);
    TLDNode *n = NULL;
    // find the first node:
    n = tldlist_iter_next(iter);

    // until n becomes NULL (end of list):
    while (n) {
        count += n->count;
        n = tldlist_iter_next(iter);
    }
    tldlist_iter_destroy(iter);
    return count;
}

TLDNode *find_most_left(TLDNode *root) {
    while (root->left) {
        root = root->left;
    }
    return root;
}

TLDNode *find_most_right(TLDNode *root) {
    while (root->right) {
        root = root->right;
    }
    return root;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {

    TLDIterator *iter = NULL;

    if ((iter = malloc(sizeof(TLDIterator)))) {
        iter->current_node = NULL;
        iter->list_root = tld->root;
        return iter;
    }
    return NULL;
}

// find the first node in list which iter points to:
TLDNode *find_first_iter_node(TLDIterator *iter) {
    printf("finding the first node in list which iter points to\n");
    TLDNode *n = iter->list_root;

    // if n has left or right children:
    while (n->left || n->right) {

        if (n->left) {
            n = find_most_left(n);

        } else if (n->right) {
            n = find_most_right(n);
        }
    }
    iter->current_node = n;
    return iter->current_node;
}

// find the next logical successor from current node:
// return iter with next node, or NULL if there is no next node:
// Post order = > Left, Right, Root
TLDNode *tldlist_iter_next(TLDIterator *iter) {

    if (!(iter))
        return NULL;

    // if iter doesn't have current node yet (i.e this is the first time tldlist_iter_next is called):
    if (!(iter->current_node)) {
        iter->current_node = find_first_iter_node(iter);
        return iter->current_node;
    }

    // find parent
    if (iter->current_node->parent) {
        // if this node is its left child, check if right child exist
        if (iter->current_node == iter->current_node->parent->left) {
            iter->current_node = iter->current_node->parent;

            // if right child exist, go there:
            if (iter->current_node->right) {
                iter->current_node = find_most_left(iter->current_node->right);
                iter->current_node = find_most_right(iter->current_node);
            }

            // if no, stay on parent node:
        } else if (iter->current_node == iter->current_node->parent->right) {
            // if this node is its right child, go to parent
            iter->current_node = iter->current_node->parent;
        }

        return iter->current_node;
    }
    return NULL;
}

void tldlist_iter_destroy(TLDIterator *iter) {
    if (iter) {
        free(iter);
        iter = NULL;
    }
}

char *tldnode_tldname(TLDNode *node) {
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    return node->count;
}
