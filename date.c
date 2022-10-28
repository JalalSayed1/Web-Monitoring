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

        // first 2 chars of datestr are for int day:
        char day[3];
        day[0] = datestr[0];
        day[1] = datestr[1];
        day[2] = '\0';
        d->day = atoi(day);

        // skip the '/' and get the next 2 chars for int month:
        char month[3];
        month[0] = datestr[3];
        month[1] = datestr[4];
        month[2] = '\0';
        d->month = atoi(month);

        // skip the '/' and get the next 4 chars for int year:
        char year[5];
        year[0] = datestr[6];
        year[1] = datestr[7];
        year[2] = datestr[8];
        year[3] = datestr[9];
        year[4] = '\0';
        d->year = atoi(year);

        return d;
    }

    return NULL;
}

// char *sep = "/";
// char *p = strtok(datestr, sep);
// char *temp = p;
// while (p != NULL) {
//     temp = p;
//     p = strtok(NULL, sep);
// }

// char *token = strtok(datestr, "/");
// int counter = 0;
// while (token != NULL) {
//     printf(" %s\n", token);

//     if (counter == 0) {
//         d->day = atoi(token);
//     } else if (counter == 1) {
//         d->month = atoi(token);
//     } else if (counter == 2) {
//         d->year = atoi(token);
//     }

//     token = strtok(NULL, "/");
// }

// d->day = datestr[0];
// d->month = datestr[1];
// d->year = datestr[2];
//     return d;
// }
// return NULL;
// }

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
    printf("date_destroy called with pointer %p\n", d);
    if (d != NULL) {
        printf("freeing pointer %p\n", d);
        free(d);
        d = NULL;
    } else{
        printf("pointer is NULL and didn't free it\n");
    }
}
