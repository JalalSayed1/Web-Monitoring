#include "date.c"

// test date functions:
void make_date() {
    Date *d = date_create("20/12/2018");
    printf("Date 20/12/2018 made: %d/%d/%d\t\t%s\n", d->day, d->month, d->year, (d->day == 20 && d->month == 12 && d->year == 2018) ? "PASS" : "FAIL");
}

void test_duplicate() {
    Date *d = date_create("20/12/2018");
    Date *d2 = date_duplicate(d);
    printf("Date 20/12/2018 duplicated: %d/%d/%d\t\t%s\n", d2->day, d2->month, d2->year, (d2->day == 20 && d2->month == 12 && d2->year == 2018) ? "PASS" : "FAIL");
}

void test_compare_equal() {
    Date *d = date_create("20/12/2018");
    Date *d2 = date_create("20/12/2018");
    int result = date_compare(d, d2);
    printf("Compare 20/12/2018 Result: %d\t\t%s\n", result, (result == 0) ? "PASS" : "FAIL");
}

void test_compare_greater() {
    Date *d = date_create("20/12/2018");
    Date *d2 = date_create("21/12/2018");
    int result = date_compare(d, d2);
    printf("Compare 20/12/2018 and 21/12/2018 Result: %d\t\t%s\n", result, result < 0 ? "PASS" : "FAIL");
}

void test_compare_less() {
    Date *d = date_create("20/12/2018");
    Date *d2 = date_create("19/12/2018");
    int result = date_compare(d, d2);
    printf("Compare 20/12/2018 and 19/12/2018 Result: %d\t\t%s\n", result, result > 0 ? "PASS" : "FAIL");
}

void test_destroy() {
    Date *d = date_create("20/12/2018");
    date_destroy(d);
    date_destroy(d);
    printf("Date %p 20/12/2018 destroyed\t\t%s\n",d, (d == NULL) ? "PASS" : "FAIL");
}

int main() {
    test_destroy();
    make_date();
    test_duplicate();
    test_compare_equal();
    test_compare_greater();
    test_compare_less();
    test_destroy();
    return 0;
}
