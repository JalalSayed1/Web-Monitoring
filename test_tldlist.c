#include "date.c"
#include "tldlist.c"

// test tldlist functions:
void test_tldlist_create() {
    printf("test_tldlist_create...\n");
    Date *d1 = date_create("20/12/2018");
    Date *d2 = date_create("15/10/2019");
    TLDList *tldlist = tldlist_create(d1, d2);

    printf("TLDList created:\n");
    printf("\tbegin date: %d/%d/%d\t\t%s\n", tldlist->begin->day, tldlist->begin->month, tldlist->begin->year, (tldlist->begin->day == 20 && tldlist->begin->month == 12 && tldlist->begin->year == 2018) ? "PASS" : "FAIL");
    printf("\tend date: %d/%d/%d\t\t%s\n", tldlist->end->day, tldlist->end->month, tldlist->end->year, (tldlist->end->day == 15 && tldlist->end->month == 10 && tldlist->end->year == 2019) ? "PASS" : "FAIL");
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
    tldlist_add(tldlist, "gla.COM", d3);
    tldlist_add(tldlist, "gla.ac.net", d4);
    tldlist_add(tldlist, "gla.uk.com", d5);
    tldlist_add(tldlist, "gla.edu", d6);

    printf("tldlist root: %s with count: %d\t\t%s\n", tldlist->root->hostname, tldlist->root->count, (strcmp(tldlist->root->hostname, "com") == 0 && tldlist->root->count == 2) ? "PASS" : "FAIL");
    printf("tldlist root->left: %s with count: %d\t\t%s\n", tldlist->root->left->hostname, tldlist->root->left->count, (strcmp(tldlist->root->left->hostname, "edu") == 0 && tldlist->root->left->count == 1) ? "PASS" : "FAIL");
    printf("tldlist root->right: %s with count: %d\t\t%s\n", tldlist->root->right->hostname, tldlist->root->right->count, (strcmp(tldlist->root->right->hostname, "net") == 0 && tldlist->root->right->count == 1) ? "PASS" : "FAIL");
}

int main(int argc, char const *argv[]) {
    test_tldlist_create();
    test_tldlist_add();

    return 0;
}
