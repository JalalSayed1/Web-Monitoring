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
    struct tldnode *list_root;
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

    // if root is null (base case):
    if (!root) {
        if ((root = (TLDNode *)malloc(sizeof(TLDNode)))) {
            printf("root now is null, creating new node for %s\n", hostname);
            root->hostname = hostname;
            root->count = 1;
            root->left = NULL;
            root->right = NULL;
            root->parent = NULL;
            return root;
        }
    }

    // if root is not null:
    int add_status = 0;
    printf("root is not null, root->hostname: %s\n", root->hostname);

    int compare_status = strcmp(hostname, root->hostname);
    if (compare_status == 0) {
        printf("hostname already exists in tree, incrementing counter of %s now. \n", hostname);
        // if this is the node we want:
        // increment the count:
        root->count++;
        printf("    New %s count is: %d\n", hostname, root->count);
        add_status = 1;

        // free the hostname as it is not needed:
        free(hostname);
        hostname = NULL;

    } else if (compare_status < 0) {
        printf("hostname is less than root, going left %s\n", hostname);
        // go node to go left, recursive call and assign previous node as its parent:
        TLDNode *left_child = tldlist_addbynode(root->left, hostname);
        // if add has been successful:
        if (left_child) {
            printf("left add was successful, assigning root's left child \"%s\" to %s \n", root->hostname, hostname);
            root->left = left_child;
            printf("assigning parent of %s to be %s \n", left_child->hostname, root->hostname);
            left_child->parent = root;
            add_status = 1;
            printf("returning left child %s with count %d, parent %s and root->left (should be left child %s) %s\n", left_child->hostname, left_child->count, left_child->parent->hostname, left_child->hostname, root->left->hostname);
            //! return root;
        }

    } else if (compare_status > 0) {
        printf("hostname is greater than root, going right %s\n", hostname);
        // go to right:
        TLDNode *right_child = tldlist_addbynode(root->right, hostname);
        // if add has been successful:
        if (right_child) {
            printf("right add was successful, assigning root's right child \"%s\" to %s \n", root->hostname, hostname);
            // link right child to new node:
            root->right = right_child;

            printf("assigning parent of %s to be %s \n", right_child->hostname, root->hostname);
            // link the new node to the parent:
            right_child->parent = root;
            add_status = 1;
            printf("returning right child %s with count %d, parent %s and root->right (should be right child %s) %s\n", right_child->hostname, right_child->count, right_child->parent->hostname, right_child->hostname, root->right->hostname);
            //! return root;
        }
    }
    //! was NULL
    return root;
    //!
    // printf("add_status: %d and addbynode will return node %s with count %d\n", add_status, root->hostname, root->count);
    // return root;

    // if add was successful:
    // if (add_status) {
    //     printf("add was successful, returning node %s\n", hostname);
    // }
    // printf("add_status is 0, returning NULL\n");
    // return NULL;
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
            printf("first node added successfully %s with count %d\n", tld->root->hostname, tld->root->count);
            add_status = 1;
        }
    } else {
        printf("Not the first node, adding node %s\n", lower_hostname);
        TLDNode *added_node = tldlist_addbynode(tld->root, lower_hostname);

        // if the node was added:
        if (added_node) {
            //!
            // printf("ADD: added node %s to list\n", added_node->hostname);
            // if (added_node->parent) {
            //     printf("    added node's parent is %s\n", added_node->parent->hostname);
            // }
            // if (added_node->left) {
            //     printf("    added node's left child is %s\n", added_node->left->hostname);
            // }
            // if (added_node->right) {
            //     printf("    added node's right child is %s\n", added_node->right->hostname);
            // }

            add_status = 1;
        }
    }
    //!
    //  free(lower_hostname);
    // lower_hostname = NULL;
    return add_status;
}

long tldlist_count(TLDList *tld) {

    printf("--------------------counting tldlist---------------------\n");


    if (!(tld) || !(tld->root)){
        printf("tldlist is empty\n");
        return 0;
    }

    printf("tldlist is not empty, making iter..\n");
    long count = 0;
    TLDIterator *iter = NULL;
    iter = tldlist_iter_create(tld);
    printf("iter created, first node is %s\n", iter->list_root->hostname);
    //! count += iter->current_node->count;
    //!
    TLDNode *n = NULL;
    n = tldlist_iter_next(iter);
    //! count += n->count;

    // until n becomes NULL (end of list):
    while (n) {
        count += n->count;
        printf("    iterating, current node is %s | its count is %d and total count is %ld\n", n->hostname, n->count, count);
        n = tldlist_iter_next(iter);
    }
    tldlist_iter_destroy(iter);
    printf("Final count is %ld\n", count);
    return count;
}

TLDNode *find_most_left(TLDNode *root) {
    printf("finding the most left node\n");
    while (root->left) {
        root = root->left;
    }
    printf("    most left node is %s\n", root->hostname);
    return root;
}

TLDNode *find_most_right(TLDNode *root) {
    printf("finding the most right node\n");
    while (root->right) {
        root = root->right;
    }
    printf("    most right node is %s\n", root->hostname);
    return root;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
    
    printf("--------------------creating iter---------------------\n");

    TLDIterator *iter = NULL;

    if ((iter = malloc(sizeof(TLDIterator)))) {
        printf("iter created\n");
        iter->current_node = NULL;
        iter->list_root = tld->root;
        printf("iter's list root is %s\n", iter->list_root->hostname);

        //!
        // Post order => Left, Right, Root
        // find the most left node:
        // printf("finding the most left node\n");
        // iter->current_node = find_most_left(iter->current_node);
        // printf("    most left node is %s\n", iter->current_node->hostname);

        // // find the most right node:
        // printf("finding the most right node\n");
        // iter->current_node = find_most_right(iter->current_node);
        // printf("    most right node is %s\n", iter->current_node->hostname);

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
            //!
            // printf("    n node is %s\n", n->hostname);

        } else if (n->right) {
            n = find_most_right(n);
            // printf("    n node is %s\n", n->hostname);
        }
    }
    iter->current_node = n;
    printf("    iter's first node is %s\n", iter->current_node->hostname);
    return iter->current_node;
}

// find the next logical successor from current node:
// return iter with next node, or NULL if there is no next node:
// Post order = > Left, Right, Root
TLDNode *tldlist_iter_next(TLDIterator *iter) {

    printf("--------------------iterating next---------------------\n");

    if (!(iter))
        return NULL;

    // if iter doesn't have current node yet (i.e this is the first time tldlist_iter_next is called):
    if (!(iter->current_node)){
        printf("iter doesn't have current node\n");
        iter->current_node = find_first_iter_node(iter);
        printf("    this iter's current node (after calling next) is %s\n", iter->current_node->hostname);
        return iter->current_node;
    }

    // find parent
    if (iter->current_node->parent) {
        // if this node is its left child, check if right child exist
        if (iter->current_node == iter->current_node->parent->left) {

            printf("    %s was its left child\n", iter->current_node->hostname);

            iter->current_node = iter->current_node->parent;
            printf("    went to its parent %s\n", iter->current_node->hostname);
            
            // if right child exist, go there:
            if (iter->current_node->right) {
                printf("    %s has right child\n", iter->current_node->hostname);
                iter->current_node = find_most_left(iter->current_node->right);
                iter->current_node = find_most_right(iter->current_node);
                printf("    iter's current node is %s\n", iter->current_node->hostname);

                // iter->current_node = iter->current_node->right;
                // find the most left node:
                // while (iter->current_node->left) {
                //     iter->current_node = iter->current_node->left;
                // }
                // if we reached the left most child in this subtree, go to right child if it exists:
                // while (iter->current_node->right) {
                //     iter->current_node = iter->current_node->right;
                // }
            }

            // if no, stay on parent node:
        } else if (iter->current_node == iter->current_node->parent->right) {
            printf("    %s was right child\n", iter->current_node->hostname);
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
    printf("tldnode_tldname returning %s\n", node->hostname);
    return node->hostname;
}
long tldnode_count(TLDNode *node) {
    printf("tldnode_count returning %d for %s\n", node->count, node->hostname);
    return node->count;
}
