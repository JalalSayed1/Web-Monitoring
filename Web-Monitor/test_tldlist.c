#include "date.c"
#include "tldlist.c"

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

    // free memory:
    date_destroy(d1);
    date_destroy(d2);
    tldlist_destroy(tldlist);
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

    // free memory:
    date_destroy(d3);
    date_destroy(d4);
    date_destroy(d5);
    date_destroy(d6);
    date_destroy(d7);
    date_destroy(d8);
    date_destroy(d9);
    date_destroy(d10);
    // tldlist_destroy(tldlist);
}

// test tldlist_iter_create
void test_iter_create() {
    // test_tldlist_add(tldlist);
    TLDList *list = make_list();
    add_random_nodes(list);

    printf("test_iter_create...\n");

    TLDIterator *iter = tldlist_iter_create(list);

    printf("iter->current_node: %s with count: %d\t\t%s\n", iter->current_node->hostname, iter->current_node->count, (strcmp(iter->current_node->hostname, "au") == 0 && iter->current_node->count == 1) ? "PASS" : "FAIL");

    // free memory:
    tldlist_iter_destroy(iter);
    // tldlist_destroy(list);
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

    // free memory:
    tldlist_iter_destroy(iter);
    // tldlist_destroy(list);
}

// test tldlist_destroy:
void test_tldlist_destroy_one_node() {
    // make a list with 1 node only:
    Date *begin = date_create("01/01/2018");
    Date *end = date_create("01/01/2019");
    TLDList *list = tldlist_create(begin, end);
    Date *d1 = date_create("05/10/2018");
    tldlist_add(list, "gla.ac.uk", d1);

    // copy of list and dates:
    Date *begin_copy = date_create("01/01/2018");
    Date *end_copy = date_create("01/01/2019");
    TLDList *list_copy = tldlist_create(begin_copy, end_copy);
    Date *d1_copy = date_create("05/10/2018");
    tldlist_add(list_copy, "gla.ac.uk", d1_copy);

    printf("test_tldlist_destroy...\n");

    tldlist_destroy(list);

    printf("list after destroying: %p\t\t\t%s\n", list_copy, (list_copy == NULL) ? "PASS" : "FAIL");
    printf("list_copy->root: %p\t\t\t\t%s\n", list_copy->root, (list_copy->root == NULL) ? "PASS" : "FAIL");
    printf("list_copy->begin: %p\t\t\t\t%s\n", list_copy->begin, (list_copy->begin == NULL) ? "PASS" : "FAIL");
    printf("list_copy->end: %p\t\t\t\t%s\n", list_copy->end, (list_copy->end == NULL) ? "PASS" : "FAIL");
    printf("list_copy->root->parent: %p\t\t\t%s\n", list_copy->root->parent, (list_copy->root->parent == NULL) ? "PASS" : "FAIL");
    printf("list_copy->root->left: %p\t\t\t%s\n", list_copy->root->left, (list_copy->root->left == NULL) ? "PASS" : "FAIL");
    printf("list_copy->root->right: %p\t\t\t\t%s\n", list_copy->root->right, (list_copy->root->right == NULL) ? "PASS" : "FAIL");

    // free memory:
    date_destroy(d1);
    tldlist_destroy(list_copy);
}

void test_find_ltd() {
    printf("test_find_tld...\n");

    char *tld = find_tld("gla.ac.UK");

    int compare = strcmp(tld, "UK");

    printf("tld: %s\t\t\t\t\t\t%s\n", tld, (compare == 0) ? "PASS" : "FAIL");

    // free memory:
    free(tld);
    tld = NULL;
}

void test_iter_destroy() {
    TLDList *list = make_list();
    add_random_nodes(list);

    printf("test_iter_destroy...\n");

    TLDIterator *iter = tldlist_iter_create(list);
    //! TLDNode *current_node = iter->current_node;

    // copy of iter and current_node:
    //! TLDIterator *iter_copy = tldlist_iter_create(list);
    //! TLDNode *current_node_copy = iter_copy->current_node;

    tldlist_iter_destroy(iter);

    //! printf("iter after destroy: %p\t\t\t\t%s\n", iter, (iter == NULL) ? "PASS" : "FAIL");
    //! printf("current_node after destroy: %p\t\t\t%s\n", current_node_copy, (current_node_copy == NULL) ? "PASS" : "FAIL");

    printf("iter destroyed");

    // free memory:
    // tldlist_destroy(list);
    //! tldlist_iter_destroy(iter_copy);
}

int main(int argc, char const *argv[]) {
    test_tldlist_create();

    test_tldlist_add();

    test_iter_create();

    test_iter_next();

    test_tldlist_destroy_one_node();

    // test_find_ltd();

    test_iter_destroy();

    return 0;
}
