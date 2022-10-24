#include "date.h"
#include "tldlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// create the date struct:
struct date {
    int year;
    int month;
    int day;
};

Date *date_create(char *datestr) {
    // pointer to date struct:
    struct date *d;

    if ((d = (struct date *)malloc(sizeof(struct date))) != NULL) {

        char sep[] = "/";
        char *p = strtok(datestr, sep);
        while (p != NULL) {
            p = strtok(NULL, sep);
        }
        d->day = datestr[0];
        d->month = datestr[1];
        d->year = datestr[2];
        return d;
    }
    return NULL;
}

Date *date_duplicate(Date *d) {
    Date *new;

    if ((new = (struct date *)malloc(sizeof(struct date))) != NULL) {

        new->day = d->day;
        new->month = d->month;
        new->year = d->year;
        return new;
    }
    return NULL;
}

int date_compare(Date *date1, Date *date2) {
    if (date1->year > date2->year) {
        return 1;
    } else if (date1->year < date2->year) {
        return -1;
    } else {
        if (date1->month > date2->month) {
            return 1;
        } else if (date1->month < date2->month) {
            return -1;
        } else {
            if (date1->day > date2->day) {
                return 1;
            } else if (date1->day < date2->day) {
                return -1;
            } else {
                return 0;
            }
        }
    }
}

void date_destroy(Date *d) {
    if (d != NULL) {
        free(d);
        d = NULL;
    }
}
