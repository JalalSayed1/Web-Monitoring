#include "date.h"
#include "tldlist.h"
#include <stdio.h>
#include <string.h>

#define USAGE "usage: %s begin_datestamp end_datestamp [file] ...\n"

//! FILE is not defined:
static void process(FILE *fd, TLDList *tld) {
    char bf[1024], sbf[1024];
    Date *d;
    while (fgets(bf, sizeof(bf), fd) != NULL) {
        // strchr searches for the first occurrence of the character c in the string s, returns a pointer to the first occurrence of c in s, or NULL if c is not found.:
        char *q, *p = strchr(bf, ' ');
        if (p == NULL) {
            fprintf(stderr, "Illegal input line: %s", bf);
            return;
        }
        // copy the string from bf to sbf:
        strcpy(sbf, bf);
        *p++ = '\0';
        while (*p == ' ')
            p++;
        q = strchr(p, '\n');
        if (q == NULL) {
            fprintf(stderr, "Illegal input line: %s", sbf);
            return;
        }
        *q = '\0';
        d = date_create(bf);
        //! why casting this?
        (void)tldlist_add(tld, p, d);
        date_destroy(d);
    }
}

int main(int argc, char *argv[]) {
    Date *begin = NULL, *end = NULL;
    int i;
    FILE *fd;
    TLDList *tld = NULL;
    TLDIterator *it = NULL;
    TLDNode *n;
    double total;

    if (argc < 3) {
        fprintf(stderr, USAGE, argv[0]);
        return -1;
    }
    begin = date_create(argv[1]);
    if (begin == NULL) {
        fprintf(stderr, "Error processing begin date: %s\n", argv[1]);
        goto error;
    }
    end = date_create(argv[2]);
    if (end == NULL) {
        fprintf(stderr, "Error processing end date: %s\n", argv[2]);
        goto error;
    }
    if (date_compare(begin, end) > 0) {
        fprintf(stderr, "%s > %s\n", argv[1], argv[2]);
        goto error;
    }
    tld = tldlist_create(begin, end);
    if (tld == NULL) {
        fprintf(stderr, "Unable to create TLD list\n");
        goto error;
    }
    // if no filename is specified:
    if (argc == 3)
        process(stdin, tld);
    // if filename(s) are specified as input log files:
    else {
        for (i = 3; i < argc; i++) {
            // if filename is '-', use stdin:
            if (strcmp(argv[i], "-") == 0)
                fd = stdin;
            else
                fd = fopen(argv[i], "r");
            if (fd == NULL) {
                fprintf(stderr, "Unable to open %s\n", argv[i]);
                continue;
            }
            process(fd, tld);
            if (fd != stdin)
                fclose(fd);
        }
    }
    total = (double)tldlist_count(tld);
    it = tldlist_iter_create(tld);
    if (it == NULL) {
        fprintf(stderr, "Unable to create iterator\n");
        goto error;
    }
    while ((n = tldlist_iter_next(it))) {
        printf("%6.2f %s\n", 100.0 * (double)tldnode_count(n) / total, tldnode_tldname(n));
    }
    tldlist_iter_destroy(it);
    tldlist_destroy(tld);
    date_destroy(begin);
    date_destroy(end);
    return 0;
error:
    if (it != NULL)
        tldlist_iter_destroy(it);
    if (tld != NULL)
        tldlist_destroy(tld);
    if (end != NULL)
        date_destroy(end);
    if (begin != NULL)
        date_destroy(begin);
    return -1;
}
