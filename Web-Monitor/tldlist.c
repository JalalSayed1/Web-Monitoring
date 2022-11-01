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

        temp_iter->current_node = tldlist_iter_next(temp_iter);

        while (iter->current_node) {

            // double check that left and right nodes are freed:
            //! do I need this section?
            // if (iter->current_node->left != NULL) {
            //     free(iter->current_node->left->hostname);
            //     iter->current_node->left->hostname = NULL;
            //     free(iter->current_node->left);
            //     iter->current_node->left = NULL;
            // }
            // if (iter->current_node->right != NULL) {
            //     free(iter->current_node->right->hostname);
            //     iter->current_node->right->hostname = NULL;
            //     free(iter->current_node->right);
            //     iter->current_node->right = NULL;
            // }

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

        // free the dates of the tldlist:
        //! date_destroy(tld->end);
        //! date_destroy(tld->begin);

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
// for gla.com => com
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
    // create a new node
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

// don't need the date as an argument as it was already checked in tldlist_add():
// return 0 on failure, 1 on success
TLDNode *tldlist_addbynode(TLDNode *root, char *hostname) {

    int add_status = 0;
    TLDNode *new_node = NULL;

    // if root is not null:
    if (root) {
        printf("root is not null, root->hostname: %s\n", root->hostname);

        int compare_status = strcmp(hostname, root->hostname);

        if (compare_status == 0) {
            printf("hostname already exists in tree, incrementing counter now %s\n", hostname);
            // if this is the node we want:
            // increment the count:
            root->count++;
            add_status = 1;

            // free the hostname as it is not needed:
            free(hostname);
            hostname = NULL;

        } else if (compare_status < 0) {
            printf("hostname is less than root, going left %s\n", hostname);
            // go node to go left, recursive call and assign previous node as its parent:
            new_node = tldlist_addbynode(root->left, hostname);
            // if add has been successful:
            if (new_node) {
                printf("left add was successful, assigning root \"%s\" to %s \n", root->hostname, hostname);
                // link left child to new node:
                root->left = new_node;
                // link the new node to the parent:
                new_node->parent = root;
                add_status = 1;
            }

        } else {
            printf("hostname is greater than root, going right %s\n", hostname);
            // go to right:
            new_node = tldlist_addbynode(root->right, hostname);
            // if add has been successful:
            if (new_node) {
                printf("right add was successful, assigning root \"%s\" to %s \n", root->hostname, hostname);
                // link right child to new node:
                root->right = new_node;

                printf("assigning parent of %s to %s \n", root->hostname, hostname);
                // link the new node to the parent:
                new_node->parent = root;
                add_status = 1;
            }
        }

        // if root is null, make it:
    } else if ((root = (TLDNode *)malloc(sizeof(TLDNode)))) {
        printf("root now is null, creating new node for %s\n", hostname);
        root->hostname = hostname;
        root->count = 1;
        root->left = NULL;
        root->right = NULL;
        //! is the parent node getting assigned correctly in first if statement?
        root->parent = NULL;
        add_status = 1;
    }

    if (add_status == 1) {
        printf("add was successful, returning node %s\n", hostname);
        return root;
    }
    printf("add_status is 0, returning NULL\n");
    return NULL;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d) {

    // if d is not within the range:
    if (date_compare(d, tld->begin) < 0 || date_compare(d, tld->end) > 0) {
        printf("Date is not within the range of the tldlist for %s\n", hostname);
        return 0;
    }

    char *only_tld = find_tld(hostname);

    char *lower_hostname = (char *)malloc(strlen(only_tld) + 1);

    //! char lower_hostname[strlen(only_tld)];
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
        printf("first node to add is %s\n", lower_hostname);
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
        printf("Not the first node, adding node %s\n", lower_hostname);
        TLDNode *added_node = tldlist_addbynode(tld->root, lower_hostname);

        // if the node was added:
        if (added_node) {
            printf("added node is %s\n", added_node->hostname);
            if (added_node->parent) {
                printf("    added node's parent is %s\n", added_node->parent->hostname);
            }
            if (added_node->left) {
                printf("    added node's left child is %s\n", added_node->left->hostname);
            }
            if (added_node->right) {
                printf("    added node's right child is %s\n", added_node->right->hostname);
            }

            add_status = 1;
        }
    }
    //!
    //  free(lower_hostname);
    // lower_hostname = NULL;
    return add_status;
}

long tldlist_count(TLDList *tld) {

    long count = 0;

    if (!(tld) || !(tld->root))
        return 0;

    TLDIterator *iter = NULL;
    iter = tldlist_iter_create(tld);
    count += iter->current_node->count;

    while ((iter->current_node = tldlist_iter_next(iter))) {
        count += iter->current_node->count;
    }
    tldlist_iter_destroy(iter);
    return count;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
    TLDIterator *iter = NULL;

    if ((iter = malloc(sizeof(TLDIterator)))) {
        iter->current_node = tld->root;

        // Post order => Left, Right, Root
        // find the most left node:
        while (iter->current_node->left) {
            iter->current_node = iter->current_node->left;
        }

        // find the most right node:
        while (iter->current_node->right) {
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

    if (!(iter) || !(iter->current_node))
        return NULL;

    // find parent
    if (iter->current_node->parent) {
        // if this node is its left child, check if right child exist
        if (iter->current_node == iter->current_node->parent->left) {

            iter->current_node = iter->current_node->parent;
            // if right child exist, go there:
            if (iter->current_node->right) {
                iter->current_node = iter->current_node->right;
                // find the most left node:
                while (iter->current_node->left) {
                    iter->current_node = iter->current_node->left;
                }
                // if we reached the left most child in this subtree, go to right child if it exists:
                while (iter->current_node->right) {
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
    if (iter) {
        free(iter);
        iter = NULL;
    }
    // if (iter != NULL && iter->current_node != NULL) {
    // free(iter->current_node->hostname);
    // iter->current_node->hostname = NULL;
    // free(iter->current_node);
    // iter->current_node = NULL;
    //     free(iter);
    //     iter = NULL;
    // } else if (iter != NULL) {
    //     free(iter);
    //     iter = NULL;
    // }
}

char *tldnode_tldname(TLDNode *node) {
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    return node->count;
}
