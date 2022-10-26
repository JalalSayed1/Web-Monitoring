#include "date.c"
#include "tldlist.c"

// test tldlist functions:
void test_tldlist_create() {
    printf("test_tldlist_create...\n");
    Date *d1 = date_create("20/12/2018");
    Date *d2 = date_create("15/10/2019");
    TLDList *tldlist = tldlist_create(d1, d2);
    printf("TLDList created:\n      begin date: %d/%d/%d\n      end date: %d/%d/%d", tldlist->begin->day, tldlist->begin->month, tldlist->begin->year, tldlist->end->day, tldlist->end->month, tldlist->end->year);
}

void test_tldlist_add() {
    printf("test_tldlist_add...\n");
    Date *d1 = date_create("20/12/2018");
    Date *d2 = date_create("15/10/2019");
    TLDList *tldlist = tldlist_create(d1, d2);
    // random within and outwith d1 and d2 dates:
    Date *d3 = date_create("21/12/2018");
    Date *d4 = date_create("14/10/2019");
    Date *d5 = date_create("19/12/2018");
    Date *d6 = date_create("16/10/2019");
    // add to tldlist random tlds:
    tldlist_add(tldlist, "com", d3);
    tldlist_add(tldlist, "net", d4);
    tldlist_add(tldlist, "com", d5);
    tldlist_add(tldlist, "edu", d6);

    printf("tldlist root (com): %s with count (2): %d\n", tldlist->root->hostname, tldlist->root->count);
    printf("tldlist root->left (edu): %s with count (1): %d\n", tldlist->root->left->hostname, tldlist->root->left->count);
    printf("tldlist root->right (net): %s with count (1): %d\n", tldlist->root->right->hostname, tldlist->root->right->count);
}